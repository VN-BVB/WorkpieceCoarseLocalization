#ifndef BASLERCONTROL_H
#define BASLERCONTROL_H

#include <QVTKWidget.h>
#include <plog/Init.h>
#include <plog/Initializers/ConsoleInitializer.h>
#include <plog/Initializers/RollingFileInitializer.h>
#include <plog/Log.h>
#include <pylon/BaslerUniversalInstantCamera.h>
#include <pylon/PylonIncludes.h>
#include <pylon/gige/BaslerGigECamera.h>

#include <QColor>
#include <QDebug>
#include <QObject>
#include <QTimer>
#include <QVector>
#include <ctime>
#include <iomanip>
#include <opencv2/opencv.hpp>
#include <sstream>

#include "src/yoloInference/Yolo11Inference.h"
#include "utils/StateLight.h"
#pragma execution_character_set("utf-8")
class BaslerControl : public QObject {
    Q_OBJECT

public:
    BaslerControl();
    ~BaslerControl();
    void openCamera();   // 开启相机并进行采图
    void closeCamera();  // 关闭相机
    void loadCalibConfigFromFile(const std::string &filename);

signals:
    void sendImageToView(cv::Mat image);
    void sendCameraState(int color);
    void sendSerialNumber(std::vector<std::string> SerialNumbers);
    void sendCvImagesToInfer(std::vector<cv::Mat> cvImages);
    void appendCameraLog(QString message);
    void sendGetCurrentWaypoint();

public:
    int imageSaverToInfer = 0;               // 需要保存的图象数
    int imageNumberToSaveInCalibration = 0;  // 需要保存的图象数
    std::string currentS_N;                  // 当前选择相机编号
    std::vector<std::string> S_Ns;           // 相机编号群
    int saveTypeEnable;                      // 图像保存类型
    bool cameraFlag = true;                  // 相机开启使能
private:
    double exposure = 40000;   // 相机曝光率
    int savedCalibImages = 0;  // 已经保存的图像数
    int savedPlaneImages = 0;
    int savedTrackImages = 0;
    int savednNormalImages = 0;
    int calibCameraIndex = 0;

    QTimer *photo_capture_timer = new QTimer();                    // 采集图像定时器
    QString messenges;                                             // 调试信息
    Pylon::DeviceInfoList_t device;                                // pylon相机设备
    std::array<Pylon::CBaslerUniversalInstantCamera, 10> cameras;  // pylon相机对象群
    Pylon::CBaslerUniversalInstantCamera camera;                   // pylon相机对象
    Pylon::CImageFormatConverter formatConverter;                  // pylon格式对象
    Pylon::CPylonImage pylonImage;                                 // PylonImage对象
    Pylon::CGrabResultPtr ptrGrabResult;                           // 抓取结果数据指针
    std::vector<std::string> serialNum;
};

#endif  // BASLERCONTROL_H
