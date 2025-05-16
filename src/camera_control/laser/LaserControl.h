#ifndef LASERCONTROL_H
#define LASERCONTROL_H

#include <plog/Init.h>
#include <stdio.h>

#include <QDebug>
#include <QObject>
#include <iostream>

#include "LS_HD6.h"
#include "Windows.h"
#include "hand_eye_calibration/CalibrationDataType.h"
#include "image_processing/NormalCentroid.h"
#include "utils/StateLight.h"
#include "welding/WeldSeamType.h"

#define LASER_NUMBER 1       // 相机序号
#define PROFILE_MODE 0       // 轮廓模式
#define ORIGIN_IMAGE_MODE 1  // 原始图模式

#pragma execution_character_set("utf-8")

extern QVector<QPair<QVector<double>, QVector<int>>>
    centerLineSegResultInImageWithInliers;  // 图像中的条纹中心线直线分割结果(带内点)
extern QVector<QVector<double>> centerLineSegResultInImageWithoutInliers;  // 图像中的条纹中心线直线分割结果(不带内点)
extern Eigen::Vector3d featurePointInImage;                                // 图像中的特征点
extern int weldSeamType;                                                   // 焊缝类型

using namespace LshdProfile;

class LaserControl : public QObject {
    Q_OBJECT

public:
    LaserControl();
    ~LaserControl();

    void openCamera();         // 开启相机并进行采图
    void laserStartGetData();  // 开始采图
    void laserStopGetData();   // 停止采图
    void laserLaserLineOn();   // 打开激光线
    void laserLaserLineOff();  // 关闭线激光

    void saveImage(BYTE* pImage, int imgWidth, int imgHeight);            // 保存图片
    void saveProfile(QVector<QPointF> profileData);                       // 保存点云
    void saveProfileToCalib(QVector<QPointF> profileData);                // 保存标定用点云
    void saveProfileToPath(QVector<QPointF>& profileData, QString path);  // 点云保存公用函数
    void laserSetOriginImageMode();                                       // 设置原始图模式
    void laserSetProfileMode();                                           // 设置轮廓模式
    void laserSetExposureTime();                                          // 设置曝光时间
    void closeCamera();                                                   // 关闭相机
    static void singleCallBack(double* profileX, double* profileZ, int count, BYTE* pImage, int imgWidth, int imgHeight,
                               unsigned int timeStamp, void* userDefine);  // 数据获取回调函数
    void drawFittedLineOnImage(cv::Mat& image);                             // 在图像上绘制拟合出来的直线

signals:
    void sendImageToMainWindow(QImage image);          // 发送qImage图像数据
    void sendCVImageToMainWindow(cv::Mat image);       // 发送cv::Mat图像数据
    void sendProfile(QVector<QPointF> profileData);    // 发送轮廓数据
    void sendCenterLine(QVector<QPointF> centerLine);  // 发送图像中心线
    void sendCameraState(int color);                   // 发送相机状态
    void sendCalibDataToCalibWidget(int calibDataType);

public slots:
    void whenExposureTimeRenew(double exposureTime);
    void whenNeedToSaveImage(int number);
    void whenNeedToSaveCalibData(int calibDataType, QString filePath);

private:
    double exposureTime = 10;  // 曝光时间

    int savedImages = 0;          // 1.已经保存的图像数
    int imageNumberToSave = 0;    // 1.需要保存的图象数
    int savedProfiles = 0;        // 2.需要保存的轮廓数
    int profileNumberToSave = 0;  // 2.已经保存的轮廓数

    int savedCalibProfiles = 0;        // 3.标定页面需要保存的轮廓数
    int calibProfileNumberToSave = 0;  // 3.标定页面已经保存的轮廓数
    QString calibFilePath;             // 用于标定的点云的存储路径

    static int currentDataMode;  // 当前采图模式

    NormalCentroid normalCentroid;  // 图像中心线提取类
};

Q_DECLARE_METATYPE(QVector<QPointF>)

#endif  // LASERCONTROL_H
