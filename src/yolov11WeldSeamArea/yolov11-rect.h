#ifndef YOLOV11_H
#define YOLOV11_H
#include <math.h>
#include <fstream>
#include <QString>
#include <QImage>
#include <QVector>
#include <QDebug>
#include "NvInferPlugin.h"
#include <QMessageLogger>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn/dnn.hpp>
#include <src/yolo11SegNormal/common.hpp>
#include <cmath>
#include <algorithm>
//#define obb
// const std::vector<std::string> CLASS_NAMES = {"workpiece"};

// const std::vector<std::vector<unsigned int>> COLORS = {
//     {0, 255, 0},
// };

using namespace det;
class Yolov11_Rect{
public:
    explicit Yolov11_Rect(const std::string& engine_file_path);
    ~Yolov11_Rect();

    void make_pipe(bool warmup = true);
    void copy_from_Mat(const cv::Mat& image);
    void copy_from_Mat(const cv::Mat& image, cv::Size& size);
    void letterbox(const cv::Mat& image, cv::Mat& out, cv::Size& size);
    void infer();
    void postprocess(std::vector<det::Object>& objs, float score_thres = 0.25f, float iou_thres = 0.65f, int topk = 100,
                     int num_labels = 80);
    static void draw_objects(const cv::Mat& image, cv::Mat& res, const std::vector<det::Object>& objs,
                             const std::vector<std::string>& CLASS_NAMES, const std::vector<std::vector<unsigned int>>& COLORS);
    int num_bindings;
    int num_inputs = 0;
    int num_outputs = 0;
    std::vector<void*> host_ptrs;
    std::vector<void*> device_ptrs;
    det::PreParam pparam;
    //double M_PI = 3.14159265358979323846;

private:
    nvinfer1::ICudaEngine* engine = nullptr;
    nvinfer1::IRuntime* runtime = nullptr;
    nvinfer1::IExecutionContext* context = nullptr;
    cudaStream_t stream = nullptr;
    Logger gLogger{nvinfer1::ILogger::Severity::kERROR};
    std::vector<det::Binding> input_bindings;
    std::vector<det::Binding> output_bindings;
};

#endif // YOLOV11_H
