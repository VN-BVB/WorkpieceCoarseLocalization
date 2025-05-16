#ifndef YOLO11RECTINFERENCE_H
#define YOLO11RECTINFERENCE_H

#include <plog/Init.h>
#include <plog/Initializers/ConsoleInitializer.h>
#include <plog/Initializers/RollingFileInitializer.h>
#include <plog/Log.h>
#include <QDebug>
#include <QObject>
#include "src/yolov11WeldSeamArea/yolov11-rect.h"
const std::vector<std::string> CLASS_NAMES = {"BackType1", "FrontType1", "FrontType3", "BackType2", "FrontType2"};

const std::vector<std::vector<unsigned int>> COLORS = {
    {0,   114, 189},
    {217, 83,  25 },
    {237, 177, 32 },
    {126, 47,  142},
    {119, 172, 48 }
};

class Yolo11RectInference : public QObject {
    Q_OBJECT
public:
    Yolo11RectInference();
    ~Yolo11RectInference();
    const std::string engine_file_path_rect="./data/YoloModel/Roughweldseaminspection.engine";
    Yolov11_Rect* yolo11_rect = new Yolov11_Rect(engine_file_path_rect);

    std::vector<cv::Mat> cvImagesInferring_Rect;
    // const std::string engine_file_path = "./data/best1202.engine";
    std::vector<std::string> imagePathList;

    cv::Mat res, image;

    // cv::Size size = cv::Size{640, 640};  // best1101.engine适用
    // int seg_h = 160;
    // int seg_w = 160;

    cv::Size size = cv::Size{1024, 1024};
    int seg_h = 256;
    int seg_w = 256;

    int num_channels = 32;//标签数量
    int topk = 100;  // 输出结果时的最多目标数量
    float score_thres = 0.25f;//置信度
    float iou_thres = 0.65f;//交并比
    double inferTime;  // 推理用时

    int imgNum;
    std::vector<det::Object> objs_Rect;

    void whenRecieveWpMaskInWorld(std::vector<cv::Point3d> worldCenters, std::vector<cv::Mat> worldMaskImages);
private:
    void inferSegAndCalcTime();
signals:
    void sendInferResultToMainWindow(cv::Mat res);
    void sendCoordinateTofit (std::vector<det::Object> objs,int imgNum);
    void sendSignalTocalculate();

public slots:
    void whenImageNeedToInfer(std::vector<cv::Mat> cvImages);

};

#endif  // YOLO11SEGINFERENCE_H
