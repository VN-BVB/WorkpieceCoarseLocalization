#include "CalibratateCamera.h"
/**
 * @brief 默认构造函数
 */
CalibratateCamera::CalibratateCamera() {}
/**
 * @brief 析构函数
 */
CalibratateCamera::~CalibratateCamera() {}
/**
 * @brief 执行相机校准的主函数
 *
 * 该函数会从指定目录读取校准图片，进行相机参数校准，
 * 计算外参矩阵，拟合平面，并评估校准误差。
 */
void CalibratateCamera::whenNeedCalibratateCamera() {
    // 图片输入
    std::vector<std::string> imagePaths;
    std::vector<std::string> imagePlanePaths;
    cv::Mat res;
    if (cameraIndex == 0) {
        QString message = "未选择相机";
        emit appendCalibrationLog(message);
        return;
    }
    // std::vector<cv::Mat> extrinsicMatriceTemp;
    // 标定路径
    std::string basePath = "./data/calib/camera" + std::to_string(cameraIndex) + "/img";
    std::string planePath = "./data/calib/camera" + std::to_string(cameraIndex) + "/plane";
#if _HAS_CXX17
    for (const auto& entry : std::filesystem::directory_iterator(basePath)) {
        if (entry.is_regular_file()) {
            std::string filePath = entry.path().string();
            // 判断文件是否为图片文件（根据文件扩展名）
            if (hasSuffix(filePath, ".bmp") /*|| hasSuffix(filePath, ".jpg") || hasSuffix(filePath, ".png")*/) {
                imagePaths.push_back(filePath);
            }
        }
    }
    for (const auto& entry : std::filesystem::directory_iterator(planePath)) {
        if (entry.is_regular_file()) {
            std::string filePath = entry.path().string();
            // 判断文件是否为图片文件（根据文件扩展名）
            if (hasSuffix(filePath, ".bmp")) {
                imagePlanePaths.push_back(filePath);
            }
        }
    }
#else
    listImagesInDirectory(basePath, imagePaths);
    listImagesInDirectory(planePath, imagePlanePaths);
#endif
    // 输出所有找到的图片文件路径
    if (!imagePaths.empty()) {
        CameraAndLaserPlaneCalibration::cameraCalibration(imagePaths, cameraMatrix, distCoeffs, tvecsMat, rvecsMat, imageCount, totalErr);
        // std::cout << "Camera Matrix:\n" << cameraMatrix << std::endl;
        // std::cout << "Distortion Coefficients:\n" << distCoeffs << std::endl;
        // //求外参集合
        // CameraAndLaserPlaneCalibration::calculateExtrinsicMatrices(imageCount,tvecsMat, rvecsMat, extrinsicMatriceTemp);

        // 拟合平面
        CameraAndLaserPlaneCalibration::ErrorMetrics errorMetrics = {0.0, 0.0, 0.0, 0.0};
        CameraAndLaserPlaneCalibration::planeCalibration(imagePlanePaths, cameraMatrix, distCoeffs, globalPlane, errorMetrics);
        QString errorLog = QString(
                               "相机标定误差: %1 mm\n"
                               "平面拟合误差:\nX: %2 mm\nY: %3 mm\nZ: %4 mm")
                               .arg(totalErr, 0, 'f', 6)
                               .arg(errorMetrics.meanErrorX, 0, 'f', 6)
                               .arg(errorMetrics.meanErrorY, 0, 'f', 6)
                               .arg(errorMetrics.meanErrorZ, 0, 'f', 6);

        emit appendCalibrationLog(errorLog);
    } else {
        std::cerr << "Error: No valid image files found for calibration." << std::endl;
    }
    whenSaveConfigJson(true);  // 保存
    return;
}
/**
 * @brief 检查字符串是否以指定后缀结尾
 * @param str 要检查的字符串
 * @param suffix 后缀字符串
 * @return 如果str以suffix结尾返回true，否则返回false
 */
bool CalibratateCamera::hasSuffix(const std::string& str, const std::string& suffix) {
    if (str.length() < suffix.length()) {
        return false;
    }
    return str.substr(str.length() - suffix.length()) == suffix;
}
void CalibratateCamera::whenSaveHcg(cv::Mat Hcg) {
    extrinsicMatrix = Hcg;

    whenSaveConfigJson(false, true);
}

/**
 * @brief 保存标定结果到配置文件
 *
 * 将相机矩阵、畸变系数和全局平面参数保存到config.txt文件中，
 * 并发送信号通知其他组件。
 */
void CalibratateCamera::whenSaveConfigJson(bool cameraIntrinsicParaEnable, bool extrinsicMatrixEnable) {
    std::string configDir = "./data/config";
    createDirectoryIfNotExists(configDir);
    // std::cout << "Calibration for cameraName" << 0 << ":\n";
    // std::cout << "Camera Matrix:\n" << cameraMatrix << std::endl;
    // std::cout << "Distortion Coefficients:\n" << distCoeffs << std::endl;
    // std::cout << "ExtrinsicMatrix:\n" << extrinsicMatrix << std::endl;
    // std::cout << "Global Plane Parameters: ";
    // for (const auto& p : plane) {
    //     std::cout << p << " ";
    // }
    // std::cout << std::endl;
    // 打开并读取现有的JSON文件
    std::ifstream inFile(configFilePath);
    cereal::JSONInputArchive inputArchive(inFile);
    std::map<std::string, MyMatrix> cameraConfigMap;

    try {
        inputArchive(cereal::make_nvp("Cameras", cameraConfigMap));  // 读取已存在的相机配置
    } catch (...) {
        std::cerr << "No existing camera config found or error reading the file." << std::endl;
    }
    std::string cameraName = "Camera" + std::to_string(cameraIndex);

    if (cameraConfigMap.find(cameraName) != cameraConfigMap.end()) {
        MyMatrix& cameraConfig = cameraConfigMap[cameraName];  // 获取相机配置
        if (cameraIntrinsicParaEnable) {
            cameraConfig.cameraMatrixData = std::vector<double>(cameraMatrix.begin<double>(), cameraMatrix.end<double>());
            cameraConfig.distCoeffsData = std::vector<double>(distCoeffs.begin<double>(), distCoeffs.end<double>());
            cameraConfig.globalPlaneData = globalPlane;  // 更新平面数据

        } else if (extrinsicMatrixEnable) {
            cameraConfig.extrinsicMatrixData = std::vector<double>(extrinsicMatrix.begin<double>(), extrinsicMatrix.end<double>());
        }
    } else {
        // 添加
        MyMatrix cameraConfigAdd(cameraMatrix, distCoeffs, globalPlane, extrinsicMatrix);
        cameraConfigMap[cameraName] = cameraConfigAdd;
    }

    // 打开JSON文件以进行写入
    std::ofstream outFile(configFilePath);
    if (outFile.is_open()) {
        // 创建一个Cereal JSON序列化归档对象
        cereal::JSONOutputArchive outputArchive(outFile);

        // 序列化所有相机数据
        outputArchive(cereal::make_nvp("Cameras", cameraConfigMap));

        std::cout << "Calibration results saved to data/config/workpiece_localization_calib.json" << std::endl;
    } else {
        std::cerr << "Error: Unable to open data/config/workpiece_localization_calib.json for writing." << std::endl;
    }

    // 发射信号传输标定参数
    emit sendSignalToTransmitCalibPara(configFilePath);

    // 记录成功信息
    QString message = "Calibration results saved to data/config/workpiece_localization_calib.json";
    emit appendCalibrationLog(message);
}
bool CalibratateCamera::directoryExists(const std::string& path) {
    struct _stat info;
    if (_stat(path.c_str(), &info) != 0) return false;  // cannot access
    return (info.st_mode & _S_IFDIR) != 0;
}

void CalibratateCamera::createDirectoryIfNotExists(const std::string& path) {
    if (!directoryExists(path)) {
        _mkdir(path.c_str());  // Windows专用
    }
}
void CalibratateCamera::listImagesInDirectory(const std::string& basePath, std::vector<std::string>& imagePaths) {
    std::string searchPath = basePath + "\\*";  // 搜索所有文件
    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);

    if (hFind == INVALID_HANDLE_VALUE) {
        std::cerr << "Warning: Cannot open directory " << basePath << std::endl;
        return;
    }

    do {
        if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {  // 排除子目录
            std::string filename = findData.cFileName;
            std::string fullPath = basePath + "\\" + filename;
            if (hasSuffix(filename, ".bmp") /*|| hasSuffix(filename, ".jpg") || hasSuffix(filename, ".png")*/) {
                imagePaths.push_back(fullPath);
            }
        }
    } while (FindNextFileA(hFind, &findData) != 0);

    FindClose(hFind);
}
