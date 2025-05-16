#ifndef CALIBRATATECAMERA_H
#define CALIBRATATECAMERA_H
#include <QObject>
#include "opencv2/opencv.hpp"
#include <chrono>
#include <QApplication>
#include <iostream>
#include <filesystem>
#include <src/camera_and_laser_plane_calibration/CameraAndLaserPlaneCalibration.h>
#include <cereal/archives/json.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/map.hpp>
#include <src/camera_and_laser_plane_calibration/MyMatrix.h>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <direct.h>  // 💡 这里包含 _mkdir
#include <windows.h>
#pragma execution_character_set("utf-8")
extern int cameraIndex;
extern std::string configFilePath;
extern std::string trackFilePath;
class CalibratateCamera: public QObject
{
    Q_OBJECT
public:
    CalibratateCamera();
    ~CalibratateCamera();
    cv::Mat extrinsicMatrix;

public slots:
    void whenNeedCalibratateCamera();
    void whenSaveHcg(cv::Mat Hcg);
    void whenSaveConfigJson(bool cameraIntrinsicParaEnable = false,
                            bool extrinsicMatrixEnable = false);
private:
    cv::Mat cameraMatrix;
    cv::Mat distCoeffs;
    std::vector<double> globalPlane;

    std::vector<cv::Mat> tvecsMat;
    std::vector<cv::Mat> rvecsMat;
    int imageCount;
    std::vector<std::vector<cv::Point3f>> objectCornerPoints;//标定板坐标系中的世界坐标点
    double totalErr;
    std::vector<cv::Mat> extrinsicMatrices;

    bool hasSuffix(const std::string& str, const std::string& suffix);
    bool directoryExists(const std::string &path);
    void createDirectoryIfNotExists(const std::string &path);
    void listImagesInDirectory(const std::string &basePath, std::vector<std::string> &imagePaths);
signals:
    void appendCalibrationLog(QString message);
    void sendSignalToTransmitCalibPara(const std::string& filename);
    void sendCalibrationResultToMainWindow(cv::Mat res);
};

#endif // CALIBRATATECAMERA_H
