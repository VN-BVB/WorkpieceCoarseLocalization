#ifndef CALIBRATATECAMERA_H
#define CALIBRATATECAMERA_H
#include <direct.h>  // 💡 这里包含 _mkdir
#include <sys/stat.h>
#include <windows.h>

#include <QApplication>
#include <QObject>
#include <cereal/archives/json.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/vector.hpp>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <string>

#include "include/MyMatrix.h"
#include "opencv2/opencv.hpp"
#include "src/cameraCalibration/CameraAndLaserPlaneCalibration.h"
#pragma execution_character_set("utf-8")
extern int cameraIndex;
extern std::string configFilePath;
extern std::string trackFilePath;
class CalibratateCamera : public QObject {
    Q_OBJECT
public:
    CalibratateCamera();
    ~CalibratateCamera();
    cv::Mat extrinsicMatrix;

public slots:
    void whenNeedCalibratateCamera();
    void whenSaveHcg(cv::Mat Hcg);
    void whenSaveConfigJson(bool cameraIntrinsicParaEnable = false, bool extrinsicMatrixEnable = false);

private:
    cv::Mat cameraMatrix;
    cv::Mat distCoeffs;
    std::vector<double> globalPlane;

    std::vector<cv::Mat> tvecsMat;
    std::vector<cv::Mat> rvecsMat;
    int imageCount;
    std::vector<std::vector<cv::Point3f>> objectCornerPoints;  // 标定板坐标系中的世界坐标点
    double totalErr;
    std::vector<cv::Mat> extrinsicMatrices;

    bool hasSuffix(const std::string &str, const std::string &suffix);
    bool directoryExists(const std::string &path);
    void createDirectoryIfNotExists(const std::string &path);
    void listImagesInDirectory(const std::string &basePath, std::vector<std::string> &imagePaths);
signals:
    void appendCalibrationLog(QString message);
    void sendSignalToTransmitCalibPara(const std::string &filename);
    void sendCalibrationResultToMainWindow(cv::Mat res);
};

#endif  // CALIBRATATECAMERA_H
