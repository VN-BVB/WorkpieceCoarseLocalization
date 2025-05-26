#include "WorkpieceCoarseLocalization.h"

#include "ui_WorkpieceCoarseLocalization.h"
// std::string inferencePath = "./data/toInfer"; //推理路径
std::string inferencePath = "./data/workpieceCoaLoc/Test";                                           // 推理路径
std::string trackFilePath = "./data/config/getTrackDirection.json";                                  // 地轨单位向量保存路径
std::string configFilePath = "./data/config/workpiece_localization_calib.json";                      // 相机参数保存路径
CoordinateMapper::CoordMappingType g_coordMappingType = CoordinateMapper::CoordMappingType::X_NegY;  // 机器人与像素坐标系之间的关系
workpieceBoxInWorld workpieceFinalInfoInWorld;             // 保存确认前的结果，结果进度为分割工件
workpieceBoxInWorld workpieceFinalInfoInWorldAfterVerify;  // 保存确认后的结果，结果进度为检测焊缝
workpieceBoxInWorld workpieceFinalInfoInWorldAfterIOU;     // 保存iou合并，排序的数据，结果进度为全部

WorkpieceCoarseLocalization::WorkpieceCoarseLocalization(QWidget* parent) : QWidget(parent), ui(new Ui::WorkpieceCoarseLocalization) {
    ui->setupUi(this);

    // 初始化PLOG
    plog::init(plog::debug, "./data/log/log.csv", 1000000000, 10);
    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
    plog::get()->addAppender(&consoleAppender);  // Also add logging to the console.
    // 拟合工件手动控件
    connect(ui->mapView, &ScalableGraphicsView::senderSignalPixelCoordinates, fittingWorkpieceCoordinate,
            &FittingWorkpieceCoordinate::handleClickEvent);  //
    connect(ui->mapView, &ScalableGraphicsView::senderSignalPixelCoordinates, this, &WorkpieceCoarseLocalization::whenViewWorldCoordinateLabel);
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
    connect(this, &WorkpieceCoarseLocalization::sendCommandToInferPath, yolo11SegInference, &Yolo11SegInference::whenPathNeedToInfer);
    connect(yolo11SegInference, &Yolo11SegInference::sendInferResultToMainWindow, this, &WorkpieceCoarseLocalization::whenGetInferResult);
    connect(yolo11RectInference, &Yolo11RectInference::sendInferResultToMainWindow, this, &WorkpieceCoarseLocalization::whenGetInferResult);

    // 深度学习-拟合工件坐标坐标
    connect(yolo11SegInference, &Yolo11SegInference::sendCoordinateTofit, fittingWorkpieceCoordinate,
            &FittingWorkpieceCoordinate::whenFittingWorkpieceCoordinate);
    connect(yolo11SegInference, &Yolo11SegInference::sendSignalTocalculate, fittingWorkpieceCoordinate,
            &FittingWorkpieceCoordinate::whenFinishInferrence);
    connect(yolo11SegInference, &Yolo11SegInference::sendAppendInferLog, this, &WorkpieceCoarseLocalization::whenAppendLog);
    connect(fittingWorkpieceCoordinate, &FittingWorkpieceCoordinate::sendWorkpieceResultToMainWindow, this,
            &WorkpieceCoarseLocalization::whenGetWorkpieceRailMap);
    connect(fittingWorkpieceCoordinate, &FittingWorkpieceCoordinate::appendFittingLog, this, &WorkpieceCoarseLocalization::whenAppendLog);
    connect(this, &WorkpieceCoarseLocalization::sendVerifyCoordinatesInManual, fittingWorkpieceCoordinate,
            &FittingWorkpieceCoordinate::whenVerifyWorkpieceCoordinates);
    connect(fittingWorkpieceCoordinate, &FittingWorkpieceCoordinate::sendFinalInfoToMain, this, &WorkpieceCoarseLocalization::getLocalizationResult);
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
    connect(baslerControl, &BaslerControl::sendCvImagesToInfer, yolo11SegInference, &Yolo11SegInference::whenImageNeedToInfer);
    connect(baslerControl, &BaslerControl::appendCameraLog, this, &WorkpieceCoarseLocalization::whenAppendLog);
<<<<<<< HEAD
    // 相机标定线程
    calibratateCamera->moveToThread(cameraCalibrationSubThread);
    eyeToHandCalibration->moveToThread(eyeToHandCalibrationSubThread);
    connect(this, &WorkpieceCoarseLocalization::sendSignalToCalibratate, calibratateCamera, &CalibratateCamera::whenNeedCalibratateCamera);
    connect(calibratateCamera, &CalibratateCamera::appendCalibrationLog, this, &WorkpieceCoarseLocalization::whenAppendLog);
    connect(calibratateCamera, &CalibratateCamera::sendSignalToTransmitCalibPara, fittingWorkpieceCoordinate,
            &FittingWorkpieceCoordinate::loadCalibrationParameters);
    connect(this, &WorkpieceCoarseLocalization::sendEyeToHandCalib, eyeToHandCalibration, &HandEyeCalibrationLogic::whenCalibrateEye2Hand);
    connect(this, &WorkpieceCoarseLocalization::sendGetTrackHcg, eyeToHandCalibration, &HandEyeCalibrationLogic::whenGetTrackHcg);
    connect(eyeToHandCalibration, &HandEyeCalibrationLogic::sendSignalToTransmitCalibPara, fittingWorkpieceCoordinate,
            &FittingWorkpieceCoordinate::loadCalibrationParameters);
    connect(eyeToHandCalibration, &HandEyeCalibrationLogic::appendHandEyeLog, this, &WorkpieceCoarseLocalization::whenAppendLog);
    connect(eyeToHandCalibration, &HandEyeCalibrationLogic::sendSaveHcg, calibratateCamera, &CalibratateCamera::whenSaveHcg);

    // 机器人线程
    // 连接、断联、获取姿态、获取字符信息
    // connect(this, &WorkpieceCoarseLocalization::sendRobotConnect, robot, &RobotController::whenRobotConnect);
    // connect(this, &WorkpieceCoarseLocalization::sendRobotDisconnect, robot, &RobotController::whenRobotDisconnect);
    // connect(baslerControl, &BaslerControl::sendGetCurrentWaypoint, robot, &RobotController::whenGetCurrentWaypoint);
    // connect(robot, &RobotController::appendMessageLog, this, &WorkpieceCoarseLocalization::whenAppendLog);

=======
>>>>>>> 14f67258beab6f9128e2af30f1270ba52e11fde4
    inferenceSubThread->start();
    cameraControlSubThread->start();
    fittingWorkpieceSubThread->start();
}

WorkpieceCoarseLocalization::~WorkpieceCoarseLocalization() {
    delete ui;
    delete yolo11SegInference;
    delete yolo11RectInference;
    delete fittingWorkpieceCoordinate;
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
void WorkpieceCoarseLocalization::whenViewWorldCoordinateLabel(int x, int y) { ui->coordinateLabel->setText(QString("X: %1, Y: %2").arg(x).arg(y)); }
void WorkpieceCoarseLocalization::getLocalizationResult(const workpieceBoxInWorld& workpieceBoxInfoInWorld) {
    resultPtr = std::make_shared<workpieceBoxInWorld>(workpieceBoxInfoInWorld);  // std::shared_ptr<workpieceBoxInWorld>
    whenUpdateComboWp(static_cast<int>(workpieceBoxInfoInWorld.workpieceInfoInWorld.size()));
    cv::Mat res = resultPtr->workpieceInfoInWorld[0].workpiece_weld_Mask.second;
    ui->qImageWidget->setOpenCVImage(res);
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
void WorkpieceCoarseLocalization::whenUpdateComboWp(const int size) {
    ui->comboWorkpieceNum->clear();  // 清空现有项

    for (int i = 0; i < size; ++i) {
        ui->comboWorkpieceNum->addItem(QString::number(i));
    }
}
void WorkpieceCoarseLocalization::startCoarseLocalization() { baslerControl->imageSaverToInfer++; }
void WorkpieceCoarseLocalization::printWorkpieceBoxInfo(const workpieceBoxInWorld* info, int workpieceIndex) {
    if (!info) {
        std::cout << "[printWorkpieceBoxInfo] nullptr received!" << std::endl;
        return;
    }

    const auto& vec = info->workpieceInfoInWorld;
    // qDebug() << "---vec---" << vec.size();
    if (workpieceIndex < 0 || workpieceIndex >= vec.size()) {
        std::cout << "[printWorkpieceBoxInfo] Invalid index: " << workpieceIndex << " (size: " << vec.size() << ")" << std::endl;
        return;
    }

    const auto& wp = vec[workpieceIndex];
    std::cout << "========== workpiece[" << workpieceIndex << "] Debug Info ==========" << std::endl;

    // 原图和掩膜图
    std::cout << "[cameraOriginalMat] size: " << wp.cameraOriginalMat.size() << std::endl;
    if (!wp.cameraOriginalMat.empty()) cv::imshow("cameraOriginalMat_" /*+ std::to_string(workpieceIndex) */, wp.cameraOriginalMat);

    std::cout << "[cameraSegMat] size: " << wp.cameraSegMat.size() << std::endl;
    if (!wp.cameraSegMat.empty()) cv::imshow("cameraSegMat_" /*+ std::to_string(workpieceIndex) */, wp.cameraSegMat);

    // 焊缝掩膜
    std::cout << "[workpiece_weld_Mask] before/after:" << wp.workpiece_weld_Mask.first.size() << " / " << wp.workpiece_weld_Mask.second.size()
              << std::endl;
    if (!wp.workpiece_weld_Mask.first.empty()) cv::imshow("weldMask_before_" /*+ std::to_string(workpieceIndex) */, wp.workpiece_weld_Mask.first);
    if (!wp.workpiece_weld_Mask.second.empty()) cv::imshow("weldMask_after_" /*+ std::to_string(workpieceIndex) */, wp.workpiece_weld_Mask.second);

    // 焊缝检测对象
    const auto& segObj = wp.workpiece_weld_Obj.first;
    const auto& detObjs = wp.workpiece_weld_Obj.second;
    std::cout << "[workpiece_weld_Obj] segLabel: " << segObj.label << ", prob: " << segObj.prob << std::endl;
    std::cout << "  Detected objects: " << detObjs.size() << std::endl;
    for (size_t j = 0; j < detObjs.size(); ++j) {
        const auto& d = detObjs[j];
        std::cout << "    Det[" << j << "]: label=" << d.label << ", prob=" << d.prob << ", rect=(" << d.rect.x << "," << d.rect.y << ","
                  << d.rect.width << "," << d.rect.height << ")" << ", rotated_rect.angle=" << d.rotated_rect.angle << std::endl;
    }

    // 区域坐标
    const auto& center = wp.workpieceAreaRect.first;
    const auto& topleft = wp.workpieceAreaRect.second;
    std::cout << "[workpieceAreaRect] center=(" << center.x << "," << center.y << "," << center.z << "), topleft=(" << topleft.x << "," << topleft.y
              << "," << topleft.z << ")" << std::endl;

    // 焊缝区域矩形框
    std::cout << "[weldAreaRect] count: " << wp.weldAreaRect.size() << std::endl;
    for (size_t j = 0; j < wp.weldAreaRect.size(); ++j) {
        const auto& r = wp.weldAreaRect[j];
        std::cout << "  Rect[" << j << "]: x=" << r.x << ", y=" << r.y << ", w=" << r.width << ", h=" << r.height << std::endl;
    }

    // IOU 信息
    const auto& iou = wp.workpieceIouInfo;
    if (!iou.cameraOriginalMat.empty()) {
        std::cout << "========== [workpieceIouInfo] ==========" << std::endl;

        std::cout << "  [cameraOriginalMat] size: " << iou.cameraOriginalMat.size() << std::endl;
        if (!iou.cameraOriginalMat.empty()) cv::imshow("iou_cameraOriginalMat_" /*+ std::to_string(workpieceIndex) */, iou.cameraOriginalMat);

        std::cout << "  [cameraSegMat] size: " << iou.cameraSegMat.size() << std::endl;
        if (!iou.cameraSegMat.empty()) cv::imshow("iou_cameraSegMat_" /*+ std::to_string(workpieceIndex) */, iou.cameraSegMat);

        std::cout << "  [workpiece_weld_Mask] before size: " << iou.workpiece_weld_Mask.first.size()
                  << ", after size: " << iou.workpiece_weld_Mask.second.size() << std::endl;
        if (!iou.workpiece_weld_Mask.first.empty())
            cv::imshow("iou_weldMask_before_" /*+ std::to_string(workpieceIndex) */, iou.workpiece_weld_Mask.first);
        if (!iou.workpiece_weld_Mask.second.empty())
            cv::imshow("iou_weldMask_after_" /*+ std::to_string(workpieceIndex) */, iou.workpiece_weld_Mask.second);

        const auto& iouSegObj = iou.workpiece_weld_Obj.first;
        const auto& iouDetObjs = iou.workpiece_weld_Obj.second;
        std::cout << "  [workpiece_weld_Obj] segLabel: " << iouSegObj.label << ", prob: " << iouSegObj.prob << std::endl;
        std::cout << "    Detected objects: " << iouDetObjs.size() << std::endl;
        for (size_t j = 0; j < iouDetObjs.size(); ++j) {
            const auto& d = iouDetObjs[j];
            std::cout << "    Det[" << j << "]: label=" << d.label << ", prob=" << d.prob << ", rect=(" << d.rect.x << "," << d.rect.y << ","
                      << d.rect.width << "," << d.rect.height << ")" << ", rotated_rect.angle=" << d.rotated_rect.angle << std::endl;
        }
    }
    // 全局信息
    std::cout << "[trackDirection] size: " << info->trackDirection.rows << "x" << info->trackDirection.cols << std::endl;
    if (!info->trackDirection.empty()) {
        std::cout << info->trackDirection << std::endl;
        // cv::imshow("trackDirection", info->trackDirection);
    }

    std::cout << "[finalRailMap] size: " << info->finalRailMap.rows << "x" << info->finalRailMap.cols << std::endl;
    if (!info->finalRailMap.empty()) {
        cv::imshow("finalRailMap", info->finalRailMap);
    }

    cv::waitKey(0);
}
void WorkpieceCoarseLocalization::debugProjectPointOnlyY(const cv::Mat& trackDirection, const cv::Point3d& pt) {
    if (trackDirection.empty() || trackDirection.rows != 3 || trackDirection.cols != 1) {
        std::cerr << "Invalid trackDirection vector!" << std::endl;
        return;
    }

    // 读取trackDirection的x,y,z
    double dx = trackDirection.at<double>(0, 0);
    double dy = trackDirection.at<double>(1, 0);
    double dz = trackDirection.at<double>(2, 0);

    if (dy == 0) {
        std::cerr << "trackDirection.y is zero, cannot divide by zero." << std::endl;
        return;
    }

    // 只用y求t
    double t = pt.y / dy;

    // 计算对应的投影点坐标
    double projected_x = t * dx;  // 这个是相对的，要叠加到原来
    double projected_y = t * dy;
    double projected_z = t * dz;  // 这个是相对的，要叠加到原来

    // 计算点到原点的距离
    double distance = std::sqrt(pt.x * pt.x + pt.y * pt.y + pt.z * pt.z);

    std::cout << "Original Point: " << pt << "\n";
    std::cout << "Projected Point on trackDirection: " << cv::Point3d(projected_x, projected_y, projected_z) << "\n";
    std::cout << "Distance to origin: " << distance << "\n";
    std::cout << "-----------------------------\n";
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

void WorkpieceCoarseLocalization::on_btnGetWorkpieceInfo_clicked() {
    int selectedIndex = ui->comboWorkpieceNum->currentIndex();  // 获取当前选中项的索引
    printWorkpieceBoxInfo(resultPtr.get(), selectedIndex);      // 使用这个索引替代原来的固定值
}

void WorkpieceCoarseLocalization::on_comboWorkpieceNum_currentIndexChanged(int index) {
    cv::Mat res = resultPtr->workpieceInfoInWorld[index].workpiece_weld_Mask.second;
    cv::Mat resIOU = resultPtr->workpieceInfoInWorld[index].workpieceIouInfo.workpiece_weld_Mask.second;
    if (!resIOU.empty()) {
        // 调整尺寸一致性（如果有需要）
        if (res.size() != resIOU.size()) {
            cv::resize(resIOU, resIOU, res.size());
        }

        // 左右拼接图像
        cv::Mat concatResult;
        cv::hconcat(res, resIOU, concatResult);
        ui->qImageWidget->setOpenCVImage(concatResult);
    } else {
        ui->qImageWidget->setOpenCVImage(res);
    }
}
