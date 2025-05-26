#include "test.h"
// #ifdef
// #elif defined()
// #else
// #endif
Test::Test() {}

void Test::testCalibration() {
    // 图片输入
    std::vector<std::string> imagePaths;
    std::string basePath = "D:/qt/qtb2/getImage_basler/getImage_basler/data/image/";
    // std::string basePath = "D:/qt/qtb2/getImage_basler/getImage_basler/data/image/40";
    // std::string basePath = "D:/wechatprofile/WeChat Files/wxid_8ljwsrm5m35k11/FileStorage/File/2025-03/calibration/calibration/"; //
    // 修正相对路径格式
    //  for (int i = 0; i <= 8; ++i) {
    //      std::string path = basePath + std::to_string(i) + ".bmp";
    //      if (std::filesystem::exists(path)) {
    //          imagePaths.push_back(path);
    //      } else {
    //          std::cerr << "Warning: File not found " << path << std::endl;
    //      }
    //  }
    loadImagePaths(basePath, imagePaths);
    // for (const auto& entry : std::filesystem::directory_iterator(basePath)) {
    //     // 只选择 bmp 格式的文件
    //     if (entry.is_regular_file() && entry.path().extension() == ".bmp") {
    //         imagePaths.push_back(entry.path().string());
    //     }else{
    //         std::cerr << "Warning: File not found " << path << std::endl;
    //     }
    // 测试标定功能
    cv::Mat cameraMatrix;
    cv::Mat distCoeffs;
    std::vector<cv::Mat> tvecsMat;
    std::vector<cv::Mat> rvecsMat;

    if (!imagePaths.empty()) {
        int imageCount;
        double totalErr;
        CameraAndLaserPlaneCalibration::cameraCalibration(imagePaths, cameraMatrix, distCoeffs, tvecsMat, rvecsMat, imageCount,
                                                          totalErr);
        // 输出标定结果
        std::cout << "Camera Matrix:\n" << cameraMatrix << std::endl;
        std::cout << "Distortion Coefficients:\n" << distCoeffs << std::endl;
        // for (const auto& pt : tvecsMat) {
        //     std::cout << "tvecsMat: \n" << pt << std::endl;
        // }
        // for (const auto& pt : rvecsMat) {
        //     std::cout << "rvecsMat: \n" << pt << std::endl;
        // }

        std::string configDir = "./data/config";
        createDirectoryIfNotExists(configDir);
        // if (!std::filesystem::exists(configDir)) {
        //     std::filesystem::create_directories(configDir);
        // }
        // 将结果写入data/config/config.txt
        std::ofstream outFile("./data/config/config.txt");
        if (outFile.is_open()) {
            outFile << "Camera Matrix:\n" << cameraMatrix << "\n";
            outFile << "Distortion Coefficients:\n" << distCoeffs << "\n";
            outFile.close();
            std::cout << "Calibration results saved to data/config/config.txt" << std::endl;
        } else {
            std::cerr << "Error: Unable to open data/config/config.txt for writing." << std::endl;
        }
    } else {
        std::cerr << "Error: No valid image files found for calibration." << std::endl;
    }
    return;
}
// #include <opencv2/calib3d.hpp>
// #include <iostream>

// using namespace cv;
// using namespace std;

// struct Easy : public LMSolver::Callback {
//     Easy() = default;

//     virtual bool compute(InputArray f_param, OutputArray f_error, OutputArray f_jacobian) const override
//     {
//         Mat  param = f_param.getMat();

//         if( f_error.empty() ) f_error.create(1, 1, CV_64F);             // dim(error) = 1
//         Mat  error = f_error.getMat();

//         vector<double> x{param.at<double>(0,0), param.at<double>(1,0)}; // dim(param) = 2
//         double         error0 = calc(x);
//         error.at<double>(0,0) = error0;

//         if( ! f_jacobian.needed() ) return true;
//         else if( f_jacobian.empty() ) f_jacobian.create(1, 2, CV_64F);
//         Mat  jacobian = f_jacobian.getMat();

//         double e = 1e-10; // estimate derivatives in epsilon environment
//         jacobian.at<double>(0, 0) = (calc({x[0] + e, x[1]    }) - error0) / e; // d/dx0 (error)
//         jacobian.at<double>(0, 1) = (calc({x[0],     x[1] + e}) - error0) / e; // d/dx1 (error)
//         return true;
//     }

//     double calc(const vector<double> x) const { return x[0]*x[0] + x[1]*x[1]; }
// };

// int main(int argc, char** argv)
// {
//     Ptr<Easy>      callback   = makePtr<Easy>();
//     Ptr<LMSolver>  solver     = LMSolver::create(callback, 100000, 1e-37);
//     Mat            parameters = (Mat_<double>(2,1) << 5, 100);
//     solver->run(parameters);
//     cout << parameters << endl;
// }
bool Test::directoryExists(const std::string& path) {
    struct _stat info;
    if (_stat(path.c_str(), &info) != 0) return false;  // cannot access
    return (info.st_mode & _S_IFDIR) != 0;
}

void Test::createDirectoryIfNotExists(const std::string& path) {
    if (!directoryExists(path)) {
        _mkdir(path.c_str());  // Windows专用
    }
}
bool Test::fileExists(const std::string& filename) { return _access(filename.c_str(), 0) == 0; }

void Test::loadImagePaths(const std::string& basePath, std::vector<std::string>& imagePaths) {
    for (int i = 0; i <= 8; ++i) {
        std::string path = basePath + std::to_string(i) + ".bmp";
        if (fileExists(path)) {
            imagePaths.push_back(path);
        } else {
            std::cerr << "Warning: File not found " << path << std::endl;
        }
    }
}
bool Test::checkEngineCompatible(const std::string& engine_path) {
    // 读取 engine 文件
    std::ifstream engine_file(engine_path, std::ios::binary);
    if (!engine_file) {
        std::cerr << "Engine file not found: " << engine_path << std::endl;
        return false;
    }
    engine_file.seekg(0, std::ios::end);
    size_t size = engine_file.tellg();
    engine_file.seekg(0, std::ios::beg);

    std::vector<char> engine_data(size);
    engine_file.read(engine_data.data(), size);

    // 创建 TensorRT 运行时
    auto runtime = nvinfer1::createInferRuntime(gLogger);  // 需要你实现一个 gLogger
    if (!runtime) {
        std::cerr << "Failed to create TensorRT runtime." << std::endl;
        return false;
    }

    // 尝试反序列化 engine
    nvinfer1::ICudaEngine* engine = nullptr;
    try {
        engine = runtime->deserializeCudaEngine(engine_data.data(), size, nullptr);
    } catch (...) {
        std::cerr << "Exception thrown during engine deserialization." << std::endl;
        return false;
    }

    if (!engine) {
        std::cerr << "Failed to deserialize engine: version mismatch or corrupt file." << std::endl;
        return false;
    }

    engine->destroy();
    runtime->destroy();
    return true;
}
bool Test::checkEngineCompatibleTen10(const std::string& engine_path) {
    // 打开文件
    std::ifstream engine_file(engine_path, std::ios::binary);
    if (!engine_file) {
        std::cerr << "Engine file not found: " << engine_path << std::endl;
        return false;
    }

    // 读取全部内容
    engine_file.seekg(0, std::ios::end);
    size_t size = engine_file.tellg();
    engine_file.seekg(0, std::ios::beg);

    std::vector<char> engine_data(size);
    engine_file.read(engine_data.data(), size);

    // 创建 runtime（TensorRT 10+ 使用 unique_ptr 和 createInferRuntime）
    auto runtime = std::unique_ptr<nvinfer1::IRuntime>(nvinfer1::createInferRuntime(gLogger));
    if (!runtime) {
        std::cerr << "Failed to create TensorRT runtime." << std::endl;
        return false;
    }

    // 尝试反序列化 engine
    nvinfer1::ICudaEngine* engine = nullptr;
    try {
        engine = runtime->deserializeCudaEngine(engine_data.data(), size);
    } catch (...) {
        std::cerr << "Exception thrown during engine deserialization." << std::endl;
        return false;
    }

    if (!engine) {
        std::cerr << "Failed to deserialize engine. Version mismatch or corrupt engine." << std::endl;
        return false;
    }

    // 清理资源
    // engine->destroy();  // 或用 smart pointer
    return true;
}
void Test::checkTensorRTEngine() {
    std::cout << "TensorRT version: " << NV_TENSORRT_MAJOR << "." << NV_TENSORRT_MINOR << "." << NV_TENSORRT_PATCH << std::endl;

    const std::string engine_file_path_seg_workpiece = "./data/YoloModel/degreesMask118.engine";
    const std::string engine_file_path_rect = "./data/YoloModel/degreesRoughweldseaminspection118.engine";

    if (!checkEngineCompatible(engine_file_path_seg_workpiece)) {
        std::cerr << "Engine (seg_workpiece) is incompatible or corrupted!" << std::endl;
    } else {
        std::cerr << "yes" << std::endl;
    }

    if (!checkEngineCompatible(engine_file_path_rect)) {
        std::cerr << "Engine (rect) is incompatible or corrupted!" << std::endl;
    } else {
        std::cerr << "yes" << std::endl;
    }
}  // createExecutionContext
