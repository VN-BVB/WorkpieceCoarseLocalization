#ifndef FITTINGWORKPIECECOORDINATE_H
#define FITTINGWORKPIECECOORDINATE_H
#include <vector>
#include <QObject>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include "maskImageProcessConfig.hpp"
#include "src/yolo11SegNormal/yolo11-seg.h"
#include "src/camera_and_laser_plane_calibration/CalibratateCamera.h"
// #pragma execution_character_set("utf-8")
extern std::string inferencePath ;//推理路径
extern std::vector<cv::Mat> cvImagesInferring;
struct cameraConfig{
    cv::Mat cameraMatrix;
    cv::Mat distCoeffs;
    std::vector<double> globalPlane;
    cv::Mat extrinsicMatrix;
};
struct ObjectInfo {
    Object object;       ///< 检测到的物体
    cv::Point3d pt3d;    ///< 物体左上角的3D坐标
    int validPixel;      ///< 物体的有效像素数量
    int cameraIndex;     ///< 相机序号
};
extern std::array<cameraConfig, 3> cameraParameters; //相机参数数量
using namespace CanvasDrawingConfig;


/**
 * @brief 工件坐标拟合类
 * @details 该类负责处理工件坐标的拟合计算和分类
 */
class FittingWorkpieceCoordinate : public QObject
{
    Q_OBJECT
public slots:
    void whenFittingWorkpieceCoordinate(std::vector<seg::Object> objs, int imgNum);
    void whenFinishInferrence();
    void loadCalibrationParameters(const std::string &filename);
    void handleClickEvent(int x, int y);
    void whenDisplayWeldSeamArea(const std::vector<std::vector<std::array<double, 4>>> &boxInfos);
public:

    FittingWorkpieceCoordinate();
    std::vector<ObjectInfo> allObjects;


    void Point2dto3d(std::vector<double> plane,
                     cv::Mat& cameraMatrix,
                     cv::Mat& distCoeffs,
                     std::vector<cv::Point2d>& Pt2ds,
                     std::vector<cv::Point3d>& Pt3ds);
    std::vector<cv::Point3d> transformCameraToBase(
        const std::vector<cv::Point3d>& cameraPoints,
        const cv::Mat& extrinsicMatrix);
    void saveAllObjectsToFile(std::string filePath);
    double calculateDistance(const cv::Point3d& p1, const cv::Point3d& p2);
    cv::Point3d computeCentroid(const std::vector<ObjectInfo>& group);
    std::vector<std::vector<ObjectInfo>> classifyWorkpieces(const std::vector<ObjectInfo>& allObjects,
                                                            double threshold);
    void removeSmallCategories(std::vector<std::vector<ObjectInfo> > &categorizedObjects);
    std::vector<cv::Point3d> calculateCategoryCenters(std::vector<std::vector<ObjectInfo> > &categorizedObjects);
    void displayDetectedWorkpieces(const std::vector<std::vector<ObjectInfo> > &categorizedObjects,
                                   const std::vector<cv::Mat>& cvImagesToDisplay,
                                   const std::vector<cv::Point3d>& categoryCenters3d);

    void whenVerifyWorkpieceCoordinates();
    std::vector<cv::Point3d> pixel2WorldCoordPoint(std::vector<cv::Point2d> &Pt2ds, int cameraNumber);
    void drawGridAndAxes(cv::Mat &railMap);
    void drawDetectedWorkpieces(cv::Mat &railMap, const cv::Mat &resizedImage,
                                cv::Point3d &worldCenter, int categoryIdx);
private:
    cv::Mat cameraMatrix;
    cv::Mat distCoeffs;
    std::vector<double> plane;
    cv::Mat extrinsicMatrix;

    cv::Mat railMap;//长画布
    // cv::Mat canvasMat = (cv::Mat_<double>(3, 3) <<  1, 0, pixelCol / 2,
    //                      0, -1, pixelRow / 2,
    //                      0, 0, 1); //画布坐标系偏移
    std::vector<std::vector<ObjectInfo>> categorizedObjects; // 存储分类结果
    std::vector<cv::Point3d> categoryWorldCenters; //存储人工筛选前世界坐标下的中心点
    std::vector<cv::Point3d> categoryWorldLeftTopCenters; //存储人工筛选前世界坐标下的左上角点
    std::vector<cv::Point3d> filteredWorldCenters;       // 人工筛选后的工件中心点（世界坐标系）
    std::vector<cv::Point3d> filteredWorldTopLeftPoints; // 人工筛选后的工件左上角点（世界坐标系）
    int distance = 100; // 移动距离
    double threshold = 10.0; //工件坐标分类距离阈值
    int maxPixelCount = 3;  //最多像素掩膜索取数（与类别尺寸取min）
    std::vector<int> selectedWorkpieces;  // 存储被删除的工件索引
    std::vector<cv::Mat>worldMaskImages; //存储世界坐标系下的工作掩膜图像
    std::vector<std::pair<cv::Rect, int>> workpieceROIs; // 存储每个roi和它对应的类别索引
    void railMapRotated(cv::Mat &image,  int angle);
signals:
    /**
     * @brief 日志信号
     * @param message 要记录的日志信息
     */
    void appendFittingLog(QString message);
    void sendWorkpieceResultToMainWindow(cv::Mat res);
    void sendFinalInfoToMain(std::vector<cv::Point3d> resultCenters,
                             std::vector<cv::Point3d> resultLeftTop);
    void sendWorkpieceMaskImageInWorld(std::vector<cv::Point3d> worldCenters,
                                       std::vector<cv::Mat> worldMaskImages);
};

#endif // FITTINGWORKPIECECOORDINATE_H
