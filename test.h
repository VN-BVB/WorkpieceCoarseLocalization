#ifndef TEST_H
#define TEST_H
#include "WorkpieceCoarseLocalization.h"
#include "opencv2/opencv.hpp"
#include "src/yolo11SegNormal/yolo11-seg.h"
#include <chrono>
#include <QApplication>
#include <cuda_runtime.h>
#include <iostream>
#include <filesystem>
#include <src/camera_and_laser_plane_calibration/CameraAndLaserPlaneCalibration.h>
#include <direct.h>
#include <sys/stat.h>
#include <string>
#include <io.h> // for _access
#include "NvInfer.h"
#include <fstream>
#include <memory>
class ggLogger : public nvinfer1::ILogger {
public:
    void log(Severity severity, const char* msg) noexcept override {
        if (severity <= Severity::kERROR)  // 只打印 Error 及以上
            std::cout << "[TensorRT] " << msg << std::endl;
    }
};
class Test
{
public:
    Test();
    ggLogger gLogger;
    void testYolo();
    void testCalibration();

    bool directoryExists(const std::string &path);
    void createDirectoryIfNotExists(const std::string &path);
    bool fileExists(const std::string &filename);
    void loadImagePaths(const std::string &basePath, std::vector<std::string> &imagePaths);

    bool checkEngineCompatible(const std::string &engine_path);
    bool checkEngineCompatibleTen10(const std::string &engine_path);
    void checkTensorRTEngine();
};

#endif // TEST_H
