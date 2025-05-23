#include "WorkpieceCoarseLocalization.h"

#include "ui_WorkpieceCoarseLocalization.h"
// std::string inferencePath = "./data/toInfer"; //推理路径
std::string inferencePath = "./data/workpieceCoaLoc/Test";  // 推理路径
std::string calibCameraNum = "./data/config/calibCamera_SN.json";
std::string trackFilePath = "./data/config/getTrackDirection.json";              // 地轨单位向量保存路径
std::string configFilePath = "./data/config/workpiece_localization_calib.json";  // 相机参数保存路径

WorkpieceCoarseLocalization::WorkpieceCoarseLocalization(QWidget* parent) : QWidget(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // 初始化PLOG
    plog::init(plog::debug, "./data/log/log.csv", 1000000000, 10);
    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
    plog::get()->addAppender(&consoleAppender);  // Also add logging to the console.
    // 拟合工件手动控件
    connect(ui->mapView, &ScalableGraphicsView::senderSignalPixelCoordinates, fittingWorkpieceCoordinate,
            &FittingWorkpieceCoordinate::handleClickEvent);  //
    connect(ui->mapView, &ScalableGraphicsView::senderSignalPixelCoordinates, this,
            &WorkpieceCoarseLocalization::whenViewWorldCoordinateLabel);
    // 注册Qt没有的数据类型
    qRegisterMetaType<cv::Mat>("cv::Mat");
    qRegisterMetaType<std::string>("std::string");
    qRegisterMetaType<std::vector<seg::Object>>("std::vector<seg::Object>");
    qRegisterMetaType<std::vector<det::Object>>("std::vector<det::Object>");
    qRegisterMetaType<QString>("QString");
    qRegisterMetaType<std::vector<std::string>>("std::vector<std::string>");
    qRegisterMetaType<std::vector<cv::Mat>>("std::vector<cv::Mat>");
    qRegisterMetaType<std::vector<cv::Point3d>>("std::vector<cv::Point3d>");
    qRegisterMetaType<std::vector<std::vector<std::array<double, 4>>>>("std::vector<std::vector<std::array<double, 4>>>");
    qRegisterMetaType<workpieceBoxInWorld>("workpieceBoxInWorld");
    // 深度学习与主线程
    yolo11SegInference->moveToThread(inferenceSubThread);
    yolo11RectInference->moveToThread(inferenceSubThread);
    fittingWorkpieceCoordinate->moveToThread(fittingWorkpieceSubThread);
    connect(this, &WorkpieceCoarseLocalization::sendCommandToInferPath, yolo11SegInference,
            &Yolo11SegInference::whenPathNeedToInfer);
    connect(yolo11SegInference, &Yolo11SegInference::sendInferResultToMainWindow, this,
            &WorkpieceCoarseLocalization::whenGetInferResult);
    connect(yolo11RectInference, &Yolo11RectInference::sendInferResultToMainWindow, this,
            &WorkpieceCoarseLocalization::whenGetInferResult);

    // 深度学习-拟合工件坐标坐标
    connect(yolo11SegInference, &Yolo11SegInference::sendCoordinateTofit, fittingWorkpieceCoordinate,
            &FittingWorkpieceCoordinate::whenFittingWorkpieceCoordinate);
    connect(yolo11SegInference, &Yolo11SegInference::sendSignalTocalculate, fittingWorkpieceCoordinate,
            &FittingWorkpieceCoordinate::whenFinishInferrence);
    connect(yolo11SegInference, &Yolo11SegInference::sendAppendInferLog, this, &WorkpieceCoarseLocalization::whenAppendLog);
    connect(fittingWorkpieceCoordinate, &FittingWorkpieceCoordinate::sendWorkpieceResultToMainWindow, this,
            &WorkpieceCoarseLocalization::whenGetWorkpieceRailMap);
    connect(fittingWorkpieceCoordinate, &FittingWorkpieceCoordinate::appendFittingLog, this,
            &WorkpieceCoarseLocalization::whenAppendLog);
    connect(this, &WorkpieceCoarseLocalization::sendVerifyCoordinatesInManual, fittingWorkpieceCoordinate,
            &FittingWorkpieceCoordinate::whenVerifyWorkpieceCoordinates);
    connect(fittingWorkpieceCoordinate, &FittingWorkpieceCoordinate::sendFinalInfoToMain, this,
            &WorkpieceCoarseLocalization::getLocalizationResult);
    connect(fittingWorkpieceCoordinate, &FittingWorkpieceCoordinate::sendWorkpieceMaskImageInWorld, yolo11RectInference,
            &Yolo11RectInference::whenRecieveWpMaskInWorld);
    connect(yolo11RectInference, &Yolo11RectInference::sendBoxInfoToDisplay, fittingWorkpieceCoordinate,
            &FittingWorkpieceCoordinate::whenGetWeldBoxInfo);

    // 相机
    baslerControl->moveToThread(cameraControlSubThread);
    connect(this, &WorkpieceCoarseLocalization::sendOpenCamera, baslerControl, &BaslerControl::openCamera);
    connect(this, &WorkpieceCoarseLocalization::sendDisconnectCamera, baslerControl, &BaslerControl::closeCamera);
    connect(baslerControl, &BaslerControl::sendImageToView, this, &WorkpieceCoarseLocalization::whenGetImage);
    connect(baslerControl, &BaslerControl::sendSerialNumber, this, &WorkpieceCoarseLocalization::whenUpdateComboBox);
    connect(baslerControl, &BaslerControl::sendCvImagesToInfer, yolo11SegInference,
            &Yolo11SegInference::whenImageNeedToInfer);
    connect(baslerControl, &BaslerControl::appendCameraLog, this, &WorkpieceCoarseLocalization::whenAppendLog);
    // 相机标定线程
    calibratateCamera->moveToThread(cameraCalibrationSubThread);
    eyeToHandCalibration->moveToThread(eyeToHandCalibrationSubThread);
    connect(this, &WorkpieceCoarseLocalization::sendSignalToCalibratate, calibratateCamera,
            &CalibratateCamera::whenNeedCalibratateCamera);
    connect(calibratateCamera, &CalibratateCamera::appendCalibrationLog, this, &WorkpieceCoarseLocalization::whenAppendLog);
    connect(calibratateCamera, &CalibratateCamera::sendSignalToTransmitCalibPara, fittingWorkpieceCoordinate,
            &FittingWorkpieceCoordinate::loadCalibrationParameters);
    connect(this, &WorkpieceCoarseLocalization::sendEyeToHandCalib, eyeToHandCalibration,
            &HandEyeCalibrationLogic::whenCalibrateEye2Hand);
    connect(this, &WorkpieceCoarseLocalization::sendGetTrackHcg, eyeToHandCalibration,
            &HandEyeCalibrationLogic::whenGetTrackHcg);
    connect(eyeToHandCalibration, &HandEyeCalibrationLogic::sendSignalToTransmitCalibPara, fittingWorkpieceCoordinate,
            &FittingWorkpieceCoordinate::loadCalibrationParameters);
    connect(eyeToHandCalibration, &HandEyeCalibrationLogic::appendHandEyeLog, this,
            &WorkpieceCoarseLocalization::whenAppendLog);
    connect(eyeToHandCalibration, &HandEyeCalibrationLogic::sendSaveHcg, calibratateCamera, &CalibratateCamera::whenSaveHcg);
    // 机器人线程
    robot->moveToThread(robotControlSubThread);
    connect(this, &WorkpieceCoarseLocalization::sendRobotConnect, robot, &RobotController::whenRobotConnect);
    connect(this, &WorkpieceCoarseLocalization::sendRobotDisconnect, robot, &RobotController::whenRobotDisconnect);
    // connect(this, &MainWindow::sendGetCurrentWaypoint, robot, &RobotController::whenGetCurrentWaypoint);
    connect(baslerControl, &BaslerControl::sendGetCurrentWaypoint, robot, &RobotController::whenGetCurrentWaypoint);
    connect(robot, &RobotController::appendMessageLog, this, &WorkpieceCoarseLocalization::whenAppendLog);

    inferenceSubThread->start();
    robotControlSubThread->start();
    cameraControlSubThread->start();
    fittingWorkpieceSubThread->start();
    cameraCalibrationSubThread->start();
    eyeToHandCalibrationSubThread->start();
}

WorkpieceCoarseLocalization::~WorkpieceCoarseLocalization() {
    delete ui;
    delete yolo11SegInference;
    delete yolo11RectInference;
    delete fittingWorkpieceCoordinate;
    delete baslerControl;
    delete calibratateCamera;
    delete eyeToHandCalibration;
    delete robot;
}

void WorkpieceCoarseLocalization::whenGetImage(cv::Mat res) { ui->qImageWidget->setOpenCVImage(res); }

void WorkpieceCoarseLocalization::whenGetInferResult(cv::Mat res) {
    if (res.empty()) {
        qDebug() << "推理结果 res 为空";
        return;
    }
    if (res.channels() != 3) {
        qDebug() << "推理结果通道数异常: " << res.channels();
    }
    ui->qImageWidget->setOpenCVImage(res);
    QString displayText = QString::number(yolo11SegInference->inferTime) + "ms";
    ui->lineEditInferTime->setText(displayText);
}
void WorkpieceCoarseLocalization::whenGetWorkpieceResult(cv::Mat res) { ui->qImageWidget->setOpenCVImage(res); }
void WorkpieceCoarseLocalization::whenGetWorkpieceRailMap(cv::Mat res) {
    QImage img = QImage(res.data, res.cols, res.rows, res.step, QImage::Format_RGB888);

    // 获取 QGraphicsScene
    QGraphicsScene* scene = ui->mapView->scene();  // 获取 mapView 的场景
    // 创建 QGraphicsPixmapItem 并将图像添加到场景中
    // QPixmap pixmap("D:/YOLO/yolo11_Seg_C++/data/workpieceCoaLoc/Test/camera_0_20250402_131335.bmp");
    QGraphicsPixmapItem* pixmapItem = new QGraphicsPixmapItem(QPixmap::fromImage(img));

    // 清空场景并添加新的图像项到场景
    scene->clear();              // 清空场景上的所有项
    scene->addItem(pixmapItem);  // 添加图像项到场景
    ui->mapView->setOriginalImageInfo(res.cols, res.rows, railMapRotationAngle);

    // 更新视图（如果没有立即显示，尝试刷新视图）
    ui->mapView->setScene(scene);  // 确保场景设置正确
}
void WorkpieceCoarseLocalization::whenViewWorldCoordinateLabel(int x, int y) {
    ui->coordinateLabel->setText(QString("X: %1, Y: %2").arg(x).arg(y));
}
void WorkpieceCoarseLocalization::getLocalizationResult(const workpieceBoxInWorld& workpieceBoxInfoInWorld) {
    auto resultPtr = std::make_shared<workpieceBoxInWorld>(workpieceBoxInfoInWorld);  // std::shared_ptr<workpieceBoxInWorld>

    // std::cout << "Sorted workpiece and weld centers:"<<std::endl;;
    for (size_t i = 0; i < resultPtr->workpieceAreaRect.size(); ++i) {
        const auto& center = resultPtr->workpieceAreaRect[i].first;
        // std::cout << "Object " << i << std::endl;
        // std::cout << "  Workpiece Center: ("
        //           << center.x << ", " << center.y << ", " << center.z << std::endl;

        if (i < resultPtr->weldAreaRect.size()) {
            const auto& weldRects = resultPtr->weldAreaRect[i];
            for (size_t j = 0; j < weldRects.size(); ++j) {
                const auto& rect = weldRects[j];
                double center_x = rect.x + rect.width / 2.0;
                double center_y = rect.y + rect.height / 2.0;
                // std::cout << "   Weld Rect " << j << " Center: ("
                //           << center_x << ", " << center_y << std::endl;
            }
        } else {
            std::cout << "   [No weld area data for this workpiece]" << std::endl;
            ;
        }
    }
    // printWorkpieceBoxInfo(resultPtr.get());

    // return resultPtr;
}
void WorkpieceCoarseLocalization::whenAppendLog(const QString message) { ui->textCalibratation->append(message); }
void WorkpieceCoarseLocalization::whenUpdateComboBox(const std::vector<std::string>& SerialNumbers) {
    ui->comboCameras->clear();  // 清空现有项

    // 将每个序列号添加到 QComboBox
    for (const auto& serial : SerialNumbers) {
        ui->comboCameras->addItem(QString::fromStdString(serial));  // 将 std::string 转为 QString
    }
}
void WorkpieceCoarseLocalization::startCoarseLocalization() { baslerControl->imageSaverToInfer++; }
void WorkpieceCoarseLocalization::printWorkpieceBoxInfo(const workpieceBoxInWorld* info) {
    if (!info) {
        std::cout << "[printWorkpieceBoxInfo] nullptr received!" << std::endl;
        return;
    }

    std::cout << "========== workpieceBoxInWorld Debug Info ==========" << std::endl;

    // cameraOriginalMat
    std::cout << "[cameraOriginalMat] count: " << info->cameraOriginalMat.size() << std::endl;
    for (size_t i = 0; i < info->cameraOriginalMat.size(); ++i) {
        std::string winName = "cameraOriginalMat_" + std::to_string(i);
        std::cout << "  Showing window: " << winName << std::endl;
        cv::imshow(winName, info->cameraOriginalMat[i]);
        cv::waitKey(100);  // 小停顿让窗口刷新
    }

    // cameraSegMat
    std::cout << "[cameraSegMat] count: " << info->cameraSegMat.size() << std::endl;
    for (size_t i = 0; i < info->cameraSegMat.size(); ++i) {
        std::string winName = "cameraSegMat_" + std::to_string(i);
        std::cout << "  Showing window: " << winName << std::endl;
        cv::imshow(winName, info->cameraSegMat[i]);
        cv::waitKey(100);
    }

    // workpiece_weld_Mask
    std::cout << "[workpiece_weld_Mask] count: " << info->workpiece_weld_Mask.size() << std::endl;
    for (size_t i = 0; i < info->workpiece_weld_Mask.size(); ++i) {
        std::string winNameBefore = "weldMask_before_" + std::to_string(i);
        std::string winNameAfter = "weldMask_after_" + std::to_string(i);
        std::cout << "  Showing windows: " << winNameBefore << " & " << winNameAfter << std::endl;
        cv::imshow(winNameBefore, info->workpiece_weld_Mask[i].first);
        cv::imshow(winNameAfter, info->workpiece_weld_Mask[i].second);
        cv::waitKey(100);
    }

    // workpiece_weld_Obj
    std::cout << "[workpiece_weld_Obj] count: " << info->workpiece_weld_Obj.size() << std::endl;
    for (size_t i = 0; i < info->workpiece_weld_Obj.size(); ++i) {
        const auto& segObj = info->workpiece_weld_Obj[i].first;
        const auto& detObjs = info->workpiece_weld_Obj[i].second;
        std::cout << "  SegObject[" << i << "]: label=" << segObj.label << ", prob=" << segObj.prob << ", rect=("
                  << segObj.rect.x << "," << segObj.rect.y << "," << segObj.rect.width << "," << segObj.rect.height << ")"
                  << ", boxMask size: " << segObj.boxMask.size() << std::endl;

        std::string winMask = "workpiece_weld_Obj_boxMask_" + std::to_string(i);
        if (!segObj.boxMask.empty()) {
            cv::imshow(winMask, segObj.boxMask);
            cv::waitKey(100);
        }

        std::cout << "    Detected Objects count: " << detObjs.size() << std::endl;
        for (size_t j = 0; j < detObjs.size(); ++j) {
            const auto& d = detObjs[j];
            std::cout << "      DetObject[" << j << "]: label=" << d.label << ", prob=" << d.prob << ", rect=(" << d.rect.x
                      << "," << d.rect.y << "," << d.rect.width << "," << d.rect.height << ")"
                      << ", rotated_rect.angle=" << d.rotated_rect.angle << std::endl;
        }
    }

    // workpieceAreaRect
    std::cout << "[workpieceAreaRect] count: " << info->workpieceAreaRect.size() << std::endl;
    for (size_t i = 0; i < info->workpieceAreaRect.size(); ++i) {
        const auto& center = info->workpieceAreaRect[i].first;
        const auto& topleft = info->workpieceAreaRect[i].second;
        std::cout << "  Workpiece[" << i << "]: center=(" << center.x << "," << center.y << "," << center.z << "), topleft=("
                  << topleft.x << "," << topleft.y << "," << topleft.z << ")" << std::endl;
    }

    // weldAreaRect
    std::cout << "[weldAreaRect] count: " << info->weldAreaRect.size() << std::endl;
    for (size_t i = 0; i < info->weldAreaRect.size(); ++i) {
        const auto& rects = info->weldAreaRect[i];
        std::cout << "  WeldRects[" << i << "] count: " << rects.size() << std::endl;
        for (size_t j = 0; j < rects.size(); ++j) {
            const auto& r = rects[j];
            std::cout << "    Rect[" << j << "]: x=" << r.x << ", y=" << r.y << ", w=" << r.width << ", h=" << r.height
                      << std::endl;
        }
    }

    // trackDirection
    std::cout << "[trackDirection] size: " << info->trackDirection.rows << "x" << info->trackDirection.cols << std::endl;
    if (!info->trackDirection.empty()) {
        std::cout << info->trackDirection << std::endl;
        cv::imshow("trackDirection", info->trackDirection);
        cv::waitKey(100);
    }

    // finalRailMap
    std::cout << "[finalRailMap] size: " << info->finalRailMap.rows << "x" << info->finalRailMap.cols << std::endl;
    if (!info->finalRailMap.empty()) {
        cv::imshow("finalRailMap", info->finalRailMap);
        cv::waitKey(100);
    }

    std::cout << "====================================================" << std::endl;
}
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
void WorkpieceCoarseLocalization::on_btnConnectCamera_clicked() {
    baslerControl->cameraFlag = true;
    emit sendOpenCamera();
}
void WorkpieceCoarseLocalization::on_btnDisconnectCamera_clicked() {
    baslerControl->cameraFlag = false;
    emit sendDisconnectCamera();
    ui->comboCameras->clear();
    cv::Mat res(1, 1, CV_8UC3, cv::Scalar(0, 0, 0));
    disconnect(baslerControl, &BaslerControl::sendImageToView, this, &WorkpieceCoarseLocalization::whenGetImage);
    ui->qImageWidget->setOpenCVImage(res);
}
void WorkpieceCoarseLocalization::on_btnSaveImage_clicked() {
    QString selectedOption = ui->comboSaveImage->currentText();
    int saveType = -1;
    bool validOption = true;
    if (selectedOption == "相机手眼标定") {
        saveType = 0;
        // emit sendGetCurrentWaypoint();
    } else if (selectedOption == "平面拟合") {
        saveType = 1;
    } else if (selectedOption == "地轨标定") {
        saveType = 2;
    } else if (selectedOption == "保存图像") {
    } else {
        qDebug() << "未选择有效操作！";
        validOption = false;
    }

    if (validOption) {
        baslerControl->imageNumberToSaveInCalibration++;
        baslerControl->saveTypeEnable = saveType;
    }
}

void WorkpieceCoarseLocalization::on_btnInferPath_clicked() { emit sendCommandToInferPath(inferencePath); }

void WorkpieceCoarseLocalization::on_btnCalibratateCamera_clicked() {
    emit sendSignalToCalibratate();
    whenAppendLog("Start Camera Calibration...");
}

void WorkpieceCoarseLocalization::on_btnStartInfer_clicked() { startCoarseLocalization(); }

void WorkpieceCoarseLocalization::on_btn_VerifyCoordinates_clicked() { emit sendVerifyCoordinatesInManual(); }

void WorkpieceCoarseLocalization::on_comboCameras_currentTextChanged(const QString& currentCamera) {
    std::string CurrentCamera = currentCamera.toStdString();
    baslerControl->currentS_N = CurrentCamera;
    if (currentCamera == "未选择相机") {
        cv::Mat res(1, 1, CV_8UC3, cv::Scalar(0, 0, 0));  // 创建一个 1x1 黑色图像
        disconnect(baslerControl, &BaslerControl::sendImageToView, this, &WorkpieceCoarseLocalization::whenGetImage);
        ui->qImageWidget->setOpenCVImage(res);  // 设置黑色图像
    } else {
        connect(baslerControl, &BaslerControl::sendImageToView, this, &WorkpieceCoarseLocalization::whenGetImage);
    }
}

void WorkpieceCoarseLocalization::on_btn_calibEyetoHand_clicked() { emit sendEyeToHandCalib(); }

void WorkpieceCoarseLocalization::on_btn_calibTrack_clicked() { emit sendGetTrackHcg(); }

void WorkpieceCoarseLocalization::on_btnRobotConnect_clicked() { emit sendRobotConnect(); }

void WorkpieceCoarseLocalization::on_btnRobotDisConnect_clicked() { emit sendRobotDisconnect(); }
