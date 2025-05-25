#include "BaslerControl.h"

#include <QObject>
// #define Test
int cameraIndex = 0;  // 相机索引
BaslerControl::BaslerControl() {
    // CalibConfig configOut;
    // saveCalibConfigToFile("./data/config/calibCamera_config.json", configOut);
    loadCalibConfigFromFile(calibCameraNum);
}

BaslerControl::~BaslerControl() { this->closeCamera(); }

void BaslerControl::openCamera() {
    S_Ns.clear();
    calibCameraIndex = 0;
    S_Ns.push_back("未选择相机");
#ifdef Test
    // qDebug()<<"test";
    // std::thread::id this_id = std::this_thread::get_id();
    // std::cout << "Current thread id: " << this_id << std::endl;
    S_Ns.push_back("1");
    S_Ns.push_back("2");
    S_Ns.push_back("3");
    emit sendSerialNumber(S_Ns);
    currentS_N = S_Ns[0];  // 初始化为未采图。
    while (cameraFlag) {
        int cameraIndexPtr = std::find(S_Ns.begin(), S_Ns.end(), currentS_N) - S_Ns.begin();
        if (cameraIndex != cameraIndexPtr) {
            cameraIndex = cameraIndexPtr;
        }
    }
#else
    PLOGD << L"正在连接Basler相机... ...";
    emit appendCameraLog(QString("正在连接Basler相机... ..."));
    Pylon::PylonInitialize();  // 初始化Pylon对象

    // 判断相机连接情况
    if (!Pylon::CTlFactory::GetInstance().EnumerateDevices(device)) {
        emit sendCameraState(MY_COLOR::RED);
        PLOGE << L"未找到Basler相机，请检查相机连接情况";
        emit appendCameraLog(QString("未找到Basler相机，请检查相机连接情况"));
        return;
    }
    for (size_t i = 0; i < device.size(); ++i) {
        std::string devSerial = device[i].GetSerialNumber();

        if (std::find(serialNum.begin(), serialNum.end(), devSerial) != serialNum.end()) {
            PLOGD << L"尝试连接第" << i + 1 << L"台相机...";
            // 创建并连接相机
            // Pylon::CBaslerUniversalInstantCamera& camera = camerasa[i];
            cameras[calibCameraIndex].Attach(Pylon::CTlFactory::GetInstance().CreateDevice(device[i]));

            try {
                PLOGD << L"尝试打开" << L"第" << i + 1 << L"台Basler相机...";
                emit appendCameraLog(QString("尝试打开第%1台Basler相机...").arg(i + 1));
                cameras[i].Open();  // 打开相机
                PLOGD << L"已经打开" << L"第" << i + 1 << L"台Basler相机";
                emit appendCameraLog(QString("已经打开第%1台Basler相机").arg(i + 1));
            } catch (...) {
                emit sendCameraState(MY_COLOR::RED);
                PLOGE << L"Basler相机连接失败，可能存在其它程序正在使用相机";
                emit appendCameraLog(QString("Basler相机连接失败，可能存在其它程序正在使用相机"));
                return;
            }
            // 设置相机曝光时间
            GenApi::INodeMap& cameraNodeMap = cameras[i].GetNodeMap();
            const GenApi::CFloatPtr exposureTime = cameraNodeMap.GetNode("ExposureTimeAbs");
            exposureTime->SetValue(exposure);

            cameras[i].StartGrabbing(Pylon::GrabStrategy_LatestImageOnly);  // 启动抓取模式
            formatConverter.OutputPixelFormat = Pylon::PixelType_BGR8packed;
            PLOGD << L"第" << i + 1 << L"台Basler相机连接成功";
            emit appendCameraLog(QString("第%1台Basler相机连接成功").arg(i + 1));
            // std::string S_N = cameras[calibCameraIndex].GetDeviceInfo().GetSerialNumber();
            S_Ns.push_back(devSerial);
            calibCameraIndex++;
        }
    }
    emit sendCameraState(MY_COLOR::GREEN);
    emit sendSerialNumber(S_Ns);

    currentS_N = S_Ns[0];  // 初始化为未采图。
    int* savedImages = nullptr;

    while (cameraFlag) {
        bool isImageCaptured = (currentS_N != "未选择相机");
        cv::Mat cvImage = cv::Mat(1, 1, CV_8UC3, cv::Scalar(0, 0, 0));

        if (imageSaverToInfer > 0) {
            std::vector<cv::Mat> cvImages;
            for (int j = 0; j < calibCameraIndex; j++) {
                cameras[j].RetrieveResult(5000, ptrGrabResult, Pylon::TimeoutHandling_ThrowException);
                if (ptrGrabResult->GrabSucceeded()) {
                    formatConverter.Convert(pylonImage, ptrGrabResult);  // 将抓取的缓冲数据转化成pylonImage
                    cv::Mat cvImage =
                        cv::Mat(ptrGrabResult->GetHeight(), ptrGrabResult->GetWidth(), CV_8UC3, (uint8_t*)pylonImage.GetBuffer());

                    // 在inference统一保存原图与分割结果
                    //  获取当前时间
                    std::time_t now = std::time(nullptr);
                    std::tm* localTime = std::localtime(&now);
                    // 创建字符串流
                    std::ostringstream dateTimeStream;
                    // 格式化年月日时分秒，添加前导零
                    dateTimeStream << std::put_time(localTime, "%Y%m%d_%H%M%S");
                    cv::imwrite("./data/workpieceCoaLoc/infer/camera" + std::to_string(j) + "_" + dateTimeStream.str() + ".bmp",
                                cvImage);
                    std::cout << "camera" + std::to_string(j + 1) + " Save image in workpieceCoaLoc succ." << std::endl;
                    // cv::imshow("Chessboard Image with Subpixel Corners", cvImage);  // 显示处理后的图像
                    // cv::waitKey(0);  // 防止采图卡顿
                    cvImages.push_back(cvImage.clone());
                }
            }
            emit sendCvImagesToInfer(cvImages);
            // PLOGE << L"图片保存成功";
            // savedImages++;
            imageSaverToInfer--;
        }
        if (isImageCaptured) {
            int cameraIndexPtr = std::find(S_Ns.begin(), S_Ns.end(), currentS_N) - S_Ns.begin();
            if (cameraIndex != cameraIndexPtr) {
                cameraIndex = cameraIndexPtr;
            }
            try {
                cameras[cameraIndex - 1].RetrieveResult(5000, ptrGrabResult, Pylon::TimeoutHandling_ThrowException);
                if (ptrGrabResult->GrabSucceeded()) {
                    formatConverter.Convert(pylonImage, ptrGrabResult);  // 将抓取的缓冲数据转化成pylonImage
                    cvImage =
                        cv::Mat(ptrGrabResult->GetHeight(), ptrGrabResult->GetWidth(), CV_8UC3, (uint8_t*)pylonImage.GetBuffer());
                    emit sendImageToView(cvImage);
                }

                if (imageNumberToSaveInCalibration > 0) {
                    std::string saveTypePath;
                    cv::Size boardSize = cv::Size(BOARD_HEIGHT, BOARD_WIDTH);
                    std::vector<cv::Point2f> imagePointsBuf;
                    cv::Mat a = cvImage.clone();
                    switch (saveTypeEnable) {
                        case 0:
                            saveTypePath = "img";
                            savedImages = &savedCalibImages;
                            break;
                        case 1:
                            saveTypePath = "plane";
                            savedImages = &savedPlaneImages;
                            break;
                        case 2:
                            saveTypePath = "compare";
                            savedImages = &savedTrackImages;
                            break;
                        default:
                            saveTypePath = "normal";
                            savedImages = &savednNormalImages;
                            break;
                    }
                    if (saveTypeEnable == 0) {
                        if (0 == cv::findChessboardCornersSB(a, boardSize, imagePointsBuf)) {
                            // cv::bitwise_not(a, a);  // 反转灰度图像
                            // if(0 == CameraAndLaserPlaneCalibration::calculate_Image_Points(a, boardSize, imagePointsBuf)){
                            appendCameraLog(QString("未找到角点"));
                            imageNumberToSaveInCalibration--;
                            continue;
                        } else {
                            appendCameraLog(QString("当前有角点"));
                            emit sendGetCurrentWaypoint();
                        }
                    }

                    if ((*savedImages) < 10) {
                        cv::imwrite("./data/calib/camera" + std::to_string(cameraIndex) + "/" + saveTypePath + "/image0" +
                                        std::to_string((*savedImages)) + ".bmp",
                                    cvImage);
                    } else {
                        cv::imwrite("./data/calib/camera" + std::to_string(cameraIndex) + "/" + saveTypePath + "/image" +
                                        std::to_string((*savedImages)) + ".bmp",
                                    cvImage);
                    }

                    // std::cout << "camera"+std::to_string(cameraIndex)+"Save image" + std::to_string(savedImages) + " succ." <<
                    // std::endl;
                    QString logMessage = QString("Camera %1: Saved image %2 successfully at path: %3")
                                             .arg(cameraIndex)
                                             .arg((*savedImages))
                                             .arg(QString::fromStdString(saveTypePath));
                    appendCameraLog(logMessage);
                    PLOGE << L"图片保存成功";
                    (*savedImages)++;
                    imageNumberToSaveInCalibration--;
                }

                cv::waitKey(5);  // 防止采图卡顿

            } catch (...) {
                PLOGE << L"采图失败，退出采图程序。";
                emit sendCameraState(MY_COLOR::RED);
                break;
            }
        }
    }
#endif
}
void BaslerControl::closeCamera() {
    // std::thread::id this_id = std::this_thread::get_id();
    // std::cout << "Current thread id: " << this_id << std::endl;
    for (auto& camera : cameras) {
        camera.Close();
        camera.DetachDevice();
    }
    Pylon::PylonTerminate();

    PLOGD << L"Basler相机断开连接";
    emit appendCameraLog(QString("Basler相机断开连接"));
    emit sendCameraState(MY_COLOR::RED);
}

// void BaslerControl::whenExposureTimeRenew(double exposure) {
//     // 设置相机曝光时间
//     GenApi::INodeMap &cameraNodeMap = camera.GetNodeMap();
//     const GenApi::CFloatPtr exposureTime = cameraNodeMap.GetNode("ExposureTimeAbs");
//     exposureTime->SetValue(exposure);
// }

// void BaslerControl::whenNeedToSaveImage(int number) { imageNumberToSave = number; }
void BaslerControl::saveCalibConfigToFile(const std::string& filename, CalibConfig config) {
    config.primaryCameraSerialNum = "21158836";
    config.secondaryCameraSerialNum = "22256419";
    config.thirdaryCameraSerialNum = "22301065";
    std::ofstream os(filename);
    cereal::JSONOutputArchive archive(os);
    archive(cereal::make_nvp("calib_config", config));
}
void BaslerControl::loadCalibConfigFromFile(const std::string& filename) {
    CalibConfig config;
    std::ifstream os(filename);
    if (!os.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        throw std::runtime_error("无法打开配置文件：" + filename);
    }

    cereal::JSONInputArchive archive(os);
    archive(cereal::make_nvp("calib_config", config));

    serialNum.clear();
    if (!config.primaryCameraSerialNum.empty()) serialNum.push_back(config.primaryCameraSerialNum);
    if (!config.secondaryCameraSerialNum.empty()) serialNum.push_back(config.secondaryCameraSerialNum);
    if (!config.thirdaryCameraSerialNum.empty()) serialNum.push_back(config.thirdaryCameraSerialNum);

    return;
}
