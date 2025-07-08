#ifndef YOLO11INFERENCE_H
#define YOLO11INFERENCE_H

#include <fittingWorkpieceCoordinate/FittingWorkpieceCoordinate.h>
#include <plog/Init.h>
#include <plog/Initializers/ConsoleInitializer.h>
#include <plog/Initializers/RollingFileInitializer.h>
#include <plog/Log.h>

#include <QDebug>
#include <QObject>

#include "include/maskImageProcessConfig.hpp"
#include "src/yoloInference/yolo11-seg.h"
#include "src/yoloInference/yolov11-rect.h"
using namespace MaskTransformConfig;
const std::vector<std::string> CLASS_NAMES = {"back_corner", "front_corner", "back_beam", "front_beam", "other_type"};
const std::vector<std::vector<unsigned int>> COLORS = {
    {0,   128, 255},
    {128, 255, 0  },
    {255, 0,   128},
    {0,   255, 128},
    {128, 0,   255}
};
const std::vector<std::vector<unsigned int>> MASK_COLORS = {
    {0,   128, 255},
    {128, 255, 0  },
    {255, 0,   128},
    {0,   255, 128},
    {128, 0,   255}
};

extern std::vector<cv::Mat> cvImagesInferring;
// const std::string engine_file_path_seg_workpiece = "./data/YoloModel/degreesMask118.engine";  // degreesMask118
// const std::string engine_file_path_rect = "./data/YoloModel/degreesRoughweldseaminspection118-3.engine";
const std::string engine_file_path_seg_workpiece = "./data/YoloModel/workpieceSegRom.engine";  // degreesMask118
const std::string engine_file_path_rect = "./data/YoloModel/weldAreaDetRom.engine";
// const std::string engine_file_path_seg_workpiece = "./data/YoloModel/degreesMask118.engine";  // degreesMask118
// const std::string engine_file_path_rect = "./data/YoloModel/degreesRoughweldseaminspection118-3.engine";

class Yolo11SegInference : public QObject {
    Q_OBJECT
public:
    Yolo11SegInference();
    ~Yolo11SegInference();

    YOLO11_segCoarse *yolo11_seg = new YOLO11_segCoarse(engine_file_path_seg_workpiece);

    std::vector<std::string> imagePathList;
    cv::Mat res, image;
    //
    int seg_h = 256;
    int seg_w = 256;
    cv::Size size = cv::Size{1024, 1024};
    int seg_channels = 32;     // 分割通道
    int topk = 100;            // 输出结果时的最多目标数量
    float score_thres = 0.5f;  // 置信度
    float iou_thres = 0.1f;    // 交并比
    double inferTime;          // 推理用时
    int imgNum;
    int detectedWp = 0;
    std::vector<seg::Object> objs;
    cv::Mat mask;  // 推理得到的掩膜

    struct ObjectInfo {
        seg::Object object;
        cv::Point3d pt3d;
        int validPixel;  // 物体的有效像素数量
        int imagNum;     // 图片序号
    };

    void whenImageNeedToSave(cv::Mat cvimage, cv::Mat cvImagesInference);

private:
    void inferSegAndCalcTime();
    void colorizeAndDisplayConnectedComponents(cv::Mat &mask);
    void sortSegObjects(std::vector<seg::Object> &objs, const std::string &axis, const std::string &order);
signals:
    void sendInferResultToMainWindow(cv::Mat res);
    void sendCoordinateTofit(std::vector<seg::Object> objs, int imgNum);
    void sendSignalTocalculate();
    void sendAppendInferLog(QString message);

public slots:
    void whenPathNeedToInfer(std::string path = "./data/toInfer");
    void whenImageNeedToInfer(std::vector<cv::Mat> cvImages);
};

// 目标检测
class Yolo11RectInference : public QObject {
    Q_OBJECT
public:
    Yolo11RectInference();
    ~Yolo11RectInference();

    // const std::string engine_file_path_rect = "./data/YoloModel/degreesRoughweldseaminspection118.engine";
    Yolov11_Rect *yolo11_rect = new Yolov11_Rect(engine_file_path_rect);

    std::vector<cv::Mat> cvImagesInferring_Rect;
    std::vector<std::string> imagePathList;
    cv::Mat res, image;
    std::vector<cv::Mat> inferedImages;
    //
    cv::Size size = cv::Size{1024, 1024};
    int num_channels = 5;      // 标签数量
    int topk = 100;            // 输出结果时的最多目标数量
    float score_thres = 0.2f;  // 置信度
    float iou_thres = 0.45f;   // 交并比
    double inferTime;          // 推理用时
    int imgNum;                // 图像索引
    int workpieceNum;          // 存储用工件索引
    std::vector<det::Object> objs_det;
    std::vector<cv::Point3d> maskWorldCenters;

    void whenRecieveWpMaskInWorld(std::vector<cv::Point3d> worldCenters, std::vector<cv::Mat> worldMaskImages);

private:
    void inferSegAndCalcTime();
    void whenCoordinatesNeedToProceed(std::vector<std::vector<cv::Rect_<float>>> rect_Dets, std::vector<cv::Point3d> worldCenters);
signals:
    void sendInferResultToMainWindow(cv::Mat res);
    void sendBoxInfoToDisplay(const std::vector<std::vector<std::array<double, 4>>> &boxInfos);
public slots:
    void whenImageNeedToInfer(std::vector<cv::Mat> cvImages);
};

#endif  // YOLO11INFERENCE_H
