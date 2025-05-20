#include "WorkpieceCoarseLocalization.h"

#include "ui_WorkpieceCoarseLocalization.h"
// std::string inferencePath = "./data/toInfer"; //推理路径
std::string inferencePath = "./data/workpieceCoaLoc/Test"; //推理路径
std::string calibCameraNum = "./data/config/calibCamera_SN.json";
std::string trackFilePath = "./data/config/getTrackDirection.json";//地轨单位向量保存路径
std::string configFilePath = "./data/config/workpiece_localization_calib.json";//相机参数保存路径

WorkpieceCoarseLocalization::WorkpieceCoarseLocalization(QWidget *parent) : QWidget(parent),
    ui(new Ui::MainWindow){
    ui->setupUi(this);

    // 初始化PLOG
    plog::init(plog::debug, "./data/log/log.csv", 1000000000, 10);
    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
    plog::get()->addAppender(&consoleAppender);  // Also add logging to the console.
    //拟合工件手动控件
    connect(ui->mapView,&ScalableGraphicsView::senderSignalPixelCoordinates,fittingWorkpieceCoordinate,&FittingWorkpieceCoordinate::handleClickEvent);//
    connect(ui->mapView,&ScalableGraphicsView::senderSignalPixelCoordinates,this,&WorkpieceCoarseLocalization::whenViewWorldCoordinateLabel);
    // 注册Qt没有的数据类型
    qRegisterMetaType<cv::Mat>("cv::Mat");
    qRegisterMetaType<std::string>("std::string");
    qRegisterMetaType<std::vector<seg::Object>>("std::vector<seg::Object>");
    qRegisterMetaType<std::vector<det::Object>>("std::vector<det::Object>");
    qRegisterMetaType<QString>("QString");
    qRegisterMetaType<std::vector<std::string>>("std::vector<std::string>");
    qRegisterMetaType<std::vector<cv::Mat>>("std::vector<cv::Mat>");
    qRegisterMetaType<std::vector<cv::Point3d>>("std::vector<cv::Point3d>");
    qRegisterMetaType<std::vector<std::vector<std::array<double, 4>>>> ("std::vector<std::vector<std::array<double, 4>>>");
    // 深度学习与主线程
    yolo11SegInference->moveToThread(inferenceSubThread);
    yolo11RectInference->moveToThread(inferenceSubThread);
    fittingWorkpieceCoordinate->moveToThread(fittingWorkpieceSubThread);
    connect(this, &WorkpieceCoarseLocalization::sendCommandToInferPath, yolo11SegInference, &Yolo11SegInference::whenPathNeedToInfer);
    connect(yolo11SegInference, &Yolo11SegInference::sendInferResultToMainWindow, this, &WorkpieceCoarseLocalization::whenGetInferResult);
    connect(yolo11RectInference, &Yolo11RectInference::sendInferResultToMainWindow, this, &WorkpieceCoarseLocalization::whenGetInferResult);

    //深度学习-拟合工件坐标坐标
    connect(yolo11SegInference,&Yolo11SegInference::sendCoordinateTofit, fittingWorkpieceCoordinate,&FittingWorkpieceCoordinate::whenFittingWorkpieceCoordinate );
    connect(yolo11SegInference,&Yolo11SegInference::sendSignalTocalculate, fittingWorkpieceCoordinate,&FittingWorkpieceCoordinate::whenFinishInferrence );
    connect(yolo11SegInference, &Yolo11SegInference::sendAppendInferLog,this, &WorkpieceCoarseLocalization::whenAppendLog);
    connect(fittingWorkpieceCoordinate, &FittingWorkpieceCoordinate::sendWorkpieceResultToMainWindow, this, &WorkpieceCoarseLocalization::whenGetWorkpieceRailMap);
    connect(fittingWorkpieceCoordinate, &FittingWorkpieceCoordinate::appendFittingLog,this, &WorkpieceCoarseLocalization::whenAppendLog);
    connect(this,&WorkpieceCoarseLocalization::sendVerifyCoordinatesInManual,fittingWorkpieceCoordinate,&FittingWorkpieceCoordinate::whenVerifyWorkpieceCoordinates);
    connect(fittingWorkpieceCoordinate, &FittingWorkpieceCoordinate::sendFinalInfoToMain,this, &WorkpieceCoarseLocalization::whenGetResultInfo);
    connect(fittingWorkpieceCoordinate, &FittingWorkpieceCoordinate::sendWorkpieceMaskImageInWorld,yolo11RectInference, &Yolo11RectInference::whenRecieveWpMaskInWorld);
    connect(yolo11RectInference, &Yolo11RectInference::sendBoxInfoToDisplay, fittingWorkpieceCoordinate, &FittingWorkpieceCoordinate::whenDisplayWeldSeamArea);
    connect(yolo11RectInference, &Yolo11RectInference::sendBoxInfoToDisplay,this,&WorkpieceCoarseLocalization::whenGetWeldBoxInfo);

    //相机
    baslerControl->moveToThread(cameraControlSubThread);
    connect(this, &WorkpieceCoarseLocalization::sendOpenCamera, baslerControl, &BaslerControl::openCamera);
    connect(this, &WorkpieceCoarseLocalization::sendDisconnectCamera, baslerControl, &BaslerControl::closeCamera);
    connect(baslerControl,&BaslerControl::sendImageToView,this,&WorkpieceCoarseLocalization::whenGetImage);
    connect(baslerControl, &BaslerControl::sendSerialNumber, this, &WorkpieceCoarseLocalization::whenUpdateComboBox);
    connect(baslerControl, &BaslerControl::sendCvImagesToInfer,yolo11SegInference, &Yolo11SegInference::whenImageNeedToInfer);
    connect(baslerControl, &BaslerControl::appendCameraLog, this, &WorkpieceCoarseLocalization::whenAppendLog);
    //相机标定线程
    calibratateCamera->moveToThread(cameraCalibrationSubThread);
    eyeToHandCalibration->moveToThread(eyeToHandCalibrationSubThread);
    connect(this, &WorkpieceCoarseLocalization::sendSignalToCalibratate, calibratateCamera, &CalibratateCamera::whenNeedCalibratateCamera);
    connect(calibratateCamera, &CalibratateCamera::appendCalibrationLog,this, &WorkpieceCoarseLocalization::whenAppendLog);
    connect(calibratateCamera, &CalibratateCamera::sendSignalToTransmitCalibPara,fittingWorkpieceCoordinate,&FittingWorkpieceCoordinate::loadCalibrationParameters);
    connect(this, &WorkpieceCoarseLocalization::sendEyeToHandCalib, eyeToHandCalibration, &HandEyeCalibrationLogic::whenCalibrateEye2Hand);
    connect(this, &WorkpieceCoarseLocalization::sendGetTrackHcg, eyeToHandCalibration, &HandEyeCalibrationLogic::whenGetTrackHcg);
    connect(eyeToHandCalibration, &HandEyeCalibrationLogic::sendSignalToTransmitCalibPara,fittingWorkpieceCoordinate,&FittingWorkpieceCoordinate::loadCalibrationParameters);
    connect(eyeToHandCalibration, &HandEyeCalibrationLogic::appendHandEyeLog,this, &WorkpieceCoarseLocalization::whenAppendLog);
    connect(eyeToHandCalibration, &HandEyeCalibrationLogic::sendSaveHcg, calibratateCamera, &CalibratateCamera::whenSaveHcg);
    //机器人线程
    robot->moveToThread(robotControlSubThread);
    connect(this,&WorkpieceCoarseLocalization::sendRobotConnect, robot, &RobotController::whenRobotConnect);
    connect(this, &WorkpieceCoarseLocalization::sendRobotDisconnect, robot, &RobotController::whenRobotDisconnect);
    //connect(this, &MainWindow::sendGetCurrentWaypoint, robot, &RobotController::whenGetCurrentWaypoint);
    connect(baslerControl, &BaslerControl::sendGetCurrentWaypoint, robot, &RobotController::whenGetCurrentWaypoint);
    connect(robot, &RobotController::appendMessageLog,this, &WorkpieceCoarseLocalization::whenAppendLog);


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

void WorkpieceCoarseLocalization::whenGetImage(cv::Mat res) {
    ui->qImageWidget->setOpenCVImage(res);
}

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
void WorkpieceCoarseLocalization::whenGetWorkpieceResult(cv::Mat res) {

    ui->qImageWidget->setOpenCVImage(res);
}
void WorkpieceCoarseLocalization::whenGetWorkpieceRailMap(cv::Mat res) {
    QImage img = QImage(res.data, res.cols, res.rows, res.step, QImage::Format_RGB888);

    // 获取 QGraphicsScene
    QGraphicsScene* scene = ui->mapView->scene();  // 获取 mapView 的场景
    // 创建 QGraphicsPixmapItem 并将图像添加到场景中
    //QPixmap pixmap("D:/YOLO/yolo11_Seg_C++/data/workpieceCoaLoc/Test/camera_0_20250402_131335.bmp");
    QGraphicsPixmapItem* pixmapItem = new QGraphicsPixmapItem(QPixmap::fromImage(img));

    // 清空场景并添加新的图像项到场景
    scene->clear();  // 清空场景上的所有项
    scene->addItem(pixmapItem);  // 添加图像项到场景
    ui->mapView->setOriginalImageInfo(res.cols, res.rows, railMapRotationAngle);

    // 更新视图（如果没有立即显示，尝试刷新视图）
    ui->mapView->setScene(scene);  // 确保场景设置正确
}
void WorkpieceCoarseLocalization::whenViewWorldCoordinateLabel(int x,int y){
    ui->coordinateLabel->setText(QString("X: %1, Y: %2").arg(x).arg(y));
}

void WorkpieceCoarseLocalization::whenGetResultInfo(const std::vector<cv::Point3d> resultCenters,
                                                    const std::vector<cv::Point3d> resultLeftTop){
    workpieceBoxInfoInWorld.workpieceAreaRect.clear();
    // MyMatrixTrackDirection Track;
    // workpieceBoxInfoInWorld.TrackDirection = Track.readTrackDirectionFromJsonFile(configFilePath);
    // std::cout <<    "TrackDirection"
    //           <<    workpieceBoxInfoInWorld.TrackDirection  <<std::endl;
    for (size_t i = 0; i < resultCenters.size() && i < resultLeftTop.size(); ++i) {
        const cv::Point3d& center = resultCenters[i];
        const cv::Point3d& topLeft = resultLeftTop[i];

        //std::cout << "center: " << center << ", topLeft: " << topLeft << std::endl;

        workpieceBoxInfoInWorld.workpieceAreaRect.emplace_back(center, topLeft);
    }
}
void WorkpieceCoarseLocalization::whenGetWeldBoxInfo(const std::vector<std::vector<std::array<double, 4> > > &boxInfos){
    workpieceBoxInfoInWorld.weldAreaRect.clear();
    int i =0;
    for (const auto& objBoxes : boxInfos) {
        std::vector<cv::Rect_<double>> weldRectsForOneObj;
        //std::cout << "Object " << i++ << " boxes"<<std::endl;
        int j = 0 ;
        for (const auto& box : objBoxes) {
            // box 格式为 {centerX, centerY, width, height}
            double cx = box[0];
            double cy = box[1];
            double w  = box[2];
            double h  = box[3];

            double x = cx - w / 2.0;
            double y = cy - h / 2.0;

            cv::Rect_<double> weldRect(x, y, w, h);
            weldRectsForOneObj.push_back(weldRect);
            // std::cout << "  Box " << j++ << ": center=(" << cx << ", " << cy
            //           << "), size=(" << w << ", " << h << "), Rect=("
            //           << x << ", " << y << ", " << w << ", " << h << std::endl;
        }

        workpieceBoxInfoInWorld.weldAreaRect.push_back(weldRectsForOneObj);
    }
    if(workpieceBoxInfoInWorld.workpieceAreaRect.size() != 0){
        // 排序
        sortWorkpieceBoxInfo(workpieceBoxInfoInWorld);
    }
    auto b = getLocalizationResult();
    computeIOUsWithOverlap(workpieceBoxInfoInWorld);
    sortWorkpieceBoxInfo(workpieceBoxInfoInWorld);

    auto a = getLocalizationResult();
}
void WorkpieceCoarseLocalization::sortWorkpieceBoxInfo(workpieceBoxInWorld& boxInfo) {
    size_t n = boxInfo.workpieceAreaRect.size();
    if (n != boxInfo.weldAreaRect.size()) {
        throw std::runtime_error("workpieceAreaRect 与 weldAreaRect 长度不一致");
    }

    // 构造索引数组
    std::vector<size_t> indices(n);
    for (size_t i = 0; i < n; ++i)
        indices[i] = i;

    // 根据 center.y 从大到小排序 indices
    std::sort(indices.begin(), indices.end(),
              [&](size_t a, size_t b) {
                  return boxInfo.workpieceAreaRect[a].first.y > boxInfo.workpieceAreaRect[b].first.y;
              });

    // 根据排序索引重排两个 vector
    std::vector<std::pair<cv::Point3d, cv::Point3d>> sortedWorkpieceAreaRect(n);
    std::vector<std::vector<cv::Rect_<double>>> sortedWeldAreaRect(n);

    for (size_t i = 0; i < n; ++i) {
        sortedWorkpieceAreaRect[i] = boxInfo.workpieceAreaRect[indices[i]];
        sortedWeldAreaRect[i] = boxInfo.weldAreaRect[indices[i]];
    }

    // 写回原数据
    boxInfo.workpieceAreaRect = std::move(sortedWorkpieceAreaRect);
    boxInfo.weldAreaRect = std::move(sortedWeldAreaRect);

}
void WorkpieceCoarseLocalization::computeIOUsWithOverlap(workpieceBoxInWorld& boxInfo)
{
    const auto& rects = boxInfo.workpieceAreaRect;
    const auto& welds = boxInfo.weldAreaRect;
    size_t n = rects.size();

    std::vector<bool> mergedFlags(n, false);
    std::vector<std::pair<cv::Point3d,cv::Point3d>> mergedRects;
    std::vector<std::vector<cv::Rect_<double>>> mergedWelds;

    for (size_t i = 0; i < n; ++i) {
        if(mergedFlags[i]) continue;

        const auto& boxA = rects[i];
        const cv::Point3d& centerA = boxA.first;
        const cv::Point3d& topleftA = boxA.second;

        double widthA  = std::abs(centerA.x - topleftA.x) * 2.0;
        double heightA = std::abs(centerA.y - topleftA.y) * 2.0;
        double xA = centerA.x - widthA / 2.0;
        double yA = centerA.y - heightA / 2.0;
        cv::Rect2d rectA(xA, yA, widthA, heightA);

        bool merged = false;

        for (size_t j = 1; j < 5 && (i + j) < n  ; ++j) {

            const auto& boxB = rects[i + j];
            const cv::Point3d& centerB = boxB.first;
            const cv::Point3d& topleftB = boxB.second;

            double widthB  = std::abs(centerB.x - topleftB.x) * 2.0;
            double heightB = std::abs(centerB.y - topleftB.y) * 2.0;
            double xB = centerB.x - widthB / 2.0;
            double yB = centerB.y - heightB / 2.0;
            cv::Rect2d rectB(xB, yB, widthB, heightB);

            // IOU
            double areaA = rectA.area();
            double areaB = rectB.area();
            double interArea = (rectA & rectB).area();
            double unionArea = areaA + areaB - interArea;
            if (unionArea > 0.0) {
                double iou = interArea / unionArea;
                std::cout << "IOU between " << i << " and " << i + j << ": " << iou << std::endl;

                if (iou > 0.0) {
                    cv::Rect2d mergedRect = rectA | rectB;
                    cv::Point3d newCenter(mergedRect.x + mergedRect.width / 2.0,
                                          mergedRect.y + mergedRect.height / 2.0,
                                          0.0);
                    cv::Point3d newTopLeft(mergedRect.x , mergedRect.y, 0.0);
                    mergedRects.emplace_back(newCenter , newTopLeft);

                    //合并weld区域
                    std::vector<cv::Rect_<double>> newWelds = welds[i];
                    newWelds.insert(newWelds.end(), welds[i + j].begin(), welds[i + j].end());
                    mergedWelds.emplace_back(std::move(newWelds));

                    mergedFlags[i] = true;
                    mergedFlags[i + j] = true;

                    merged = true;
                    break; // 一个 box 只合并一次
                }
            }
        }
        if (!merged) {
            // 没有被合并，原样保留
            mergedRects.push_back(rects[i]);
            mergedWelds.push_back(welds[i]);
        }
    }
    // 替换原始数据
    boxInfo.workpieceAreaRect = std::move(mergedRects);
    boxInfo.weldAreaRect = std::move(mergedWelds);
}
std::shared_ptr<workpieceBoxInWorld> WorkpieceCoarseLocalization::getLocalizationResult () {
    auto resultPtr = std::make_shared<workpieceBoxInWorld>(workpieceBoxInfoInWorld);



    // 打印排序后的工件中心坐标

    std::cout << "Sorted workpieceAreaRect centers:\n";
    for (size_t i = 0; i < resultPtr->workpieceAreaRect.size(); ++i) {
        const auto& center = resultPtr->workpieceAreaRect[i].first;
        std::cout << "  Rect " << i << ": center=("
                  << center.x << ", " << center.y << ", " << center.z << std::endl;
    }

    // 打印排序后对应的焊缝区域中心坐标
    std::cout << "Sorted weldAreaRect centers:\n";
    for (size_t i = 0; i < resultPtr->weldAreaRect.size(); ++i) {
        const auto& weldRects = resultPtr->weldAreaRect[i];
        std::cout << " Object " << i << ":\n";
        for (size_t j = 0; j < weldRects.size(); ++j) {
            const auto& rect = weldRects[j];
            double center_x = rect.x + rect.width / 2.0;
            double center_y = rect.y + rect.height / 2.0;
            std::cout << "   Weld Rect " << j << ": center=("
                      << center_x << ", " << center_y << std::endl;
        }
    }

    return resultPtr;
}
void WorkpieceCoarseLocalization::whenAppendLog(const QString message) {
    ui->textCalibratation->append(message);
}
void WorkpieceCoarseLocalization::whenUpdateComboBox(const std::vector<std::string>& SerialNumbers) {
    ui->comboCameras->clear();  // 清空现有项

    // 将每个序列号添加到 QComboBox
    for (const auto& serial : SerialNumbers) {
        ui->comboCameras->addItem(QString::fromStdString(serial));  // 将 std::string 转为 QString
    }
}
void WorkpieceCoarseLocalization::startCoarseLocalization(){
    baslerControl->imageSaverToInfer++;
}
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
void WorkpieceCoarseLocalization::on_btnConnectCamera_clicked() {
    baslerControl->cameraFlag = true;
    emit sendOpenCamera();
}
void WorkpieceCoarseLocalization::on_btnDisconnectCamera_clicked(){
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
        //emit sendGetCurrentWaypoint();
    }
    else if (selectedOption == "平面拟合") {
        saveType = 1;
    }
    else if (selectedOption == "地轨标定") {
        saveType = 2;
    }
    else if (selectedOption == "保存图像") {
    }
    else {
        qDebug() << "未选择有效操作！";
        validOption = false;
    }

    if (validOption) {
        baslerControl->imageNumberToSaveInCalibration++;
        baslerControl->saveTypeEnable = saveType;
    }
}

void WorkpieceCoarseLocalization::on_btnInferPath_clicked() {
    emit sendCommandToInferPath(inferencePath);
}

void WorkpieceCoarseLocalization::on_btnCalibratateCamera_clicked(){
    emit sendSignalToCalibratate();
    whenAppendLog("Start Camera Calibration...");
}



void WorkpieceCoarseLocalization::on_btnStartInfer_clicked(){
    startCoarseLocalization();
}


void WorkpieceCoarseLocalization::on_btn_VerifyCoordinates_clicked(){
    emit sendVerifyCoordinatesInManual();
}


void WorkpieceCoarseLocalization::on_comboCameras_currentTextChanged(const QString &currentCamera){
    std::string CurrentCamera = currentCamera.toStdString();
    baslerControl->currentS_N = CurrentCamera;
    if(currentCamera == "未选择相机"){
        cv::Mat res(1, 1, CV_8UC3, cv::Scalar(0, 0, 0));  // 创建一个 1x1 黑色图像
        disconnect(baslerControl, &BaslerControl::sendImageToView, this, &WorkpieceCoarseLocalization::whenGetImage);
        ui->qImageWidget->setOpenCVImage(res);  // 设置黑色图像
    } else {
        connect(baslerControl, &BaslerControl::sendImageToView, this, &WorkpieceCoarseLocalization::whenGetImage);
    }
}



void WorkpieceCoarseLocalization::on_btn_calibEyetoHand_clicked(){
    emit sendEyeToHandCalib();

}


void WorkpieceCoarseLocalization::on_btn_calibTrack_clicked(){
    emit sendGetTrackHcg();
}



void WorkpieceCoarseLocalization::on_btnRobotConnect_clicked(){
    emit sendRobotConnect();
}


void WorkpieceCoarseLocalization::on_btnRobotDisConnect_clicked(){
    emit sendRobotDisconnect();
}


