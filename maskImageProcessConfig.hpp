#ifndef MASKIMAGEPROCESSCONFIG_H
#define MASKIMAGEPROCESSCONFIG_H
#include <opencv2/opencv.hpp>
struct workpieceBoxInWorld{
    std::vector<std::pair<cv::Point3d, cv::Point3d>> workpieceAreaRect; // <center, topleft>
    std::vector<std::vector<cv::Rect_<double>>> weldAreaRect;//工件焊缝区域信息
    cv::Mat TrackDirection; // 地轨方向向量
};
//------------------------------画布绘制参数---------------------------------------
namespace CanvasDrawingConfig {
//长画布参数
const int pixelRow = 6000;  // 原始画布高度
const int pixelCol =1800; // 原始画布宽度
const cv::Scalar correctColor(0, 255, 0); //正确颜色
const cv::Scalar warningColor(255, 255, 125); // 警告颜色
const cv::Scalar deleteColor(255, 0, 0); // 删除颜色
const int correctLineThickness = 3; // 正常线厚度
const int warningLineThickness = 3; // 警告线厚度
const int deleteLineThickness = 3; // 删除线厚度
const int gridSpacingX = 100;//坐标轴X间距
const int gridSpacingY = 100;//坐标轴Y间距
const cv::Scalar axisColor(0, 0, 0);//坐标轴颜色
const int axisThickness = 2;//坐标轴厚度
const cv::Mat canvasMat = (cv::Mat_<double>(3, 3) <<
                                                 1, 0, pixelCol-100,
                                                 0, -1, 500,
                                                 0, 0, 1); //绘制坐标系偏移
const int railMapRotationAngle = 0;// 0 90 180 270  画布最后可视化的角度
}

//------------------------------推理掩膜变换---------------------------------------
namespace MaskTransformConfig {
// 定义扩展后的画布大小(粗定位焊缝推理)
const int expandedWidth = 1024;
const int expandedHeight = 1024;
const int AdjustWorkpieceResolution = 1;//调整工件掩膜分辨率倍数
const int rectRotationAngle =90;//90 180 270 //旋转工件提高召回率
}
class CoordinateMapper {
public:
    // 坐标映射类型枚举
    enum class CoordMappingType {
        XY,            // x->x, y->y
        NegX_Y,        // x->-x, y->y
        X_NegY,        // x->x, y->-y
        NegX_NegY,     // x->-x, y->-y
        YX,            // x->y, y->x
        NegY_X,        // x->-y, y->x
        Y_NegX,        // x->y, y->-x
        NegY_NegX      // x->-y, y->-x
    };

    // 映射函数：将 rel 从像素坐标变换到世界坐标系
    static cv::Point2d mapToCoord(
        const cv::Point2d& rel,
        const cv::Point2d& worldCenter,
        CoordMappingType type
        ) {
        double dx = rel.x;
        double dy = rel.y;
        double x = worldCenter.x;
        double y = worldCenter.y;

        switch (type) {
        case CoordMappingType::XY:
            return {x + dx, y + dy};
        case CoordMappingType::NegX_Y:
            return {x - dx, y + dy};
        case CoordMappingType::X_NegY:
            return {x + dx, y - dy};
        case CoordMappingType::NegX_NegY:
            return {x - dx, y - dy};
        case CoordMappingType::YX:
            return {x + dy, y + dx};
        case CoordMappingType::NegY_X:
            return {x - dy, y + dx};
        case CoordMappingType::Y_NegX:
            return {x + dy, y - dx};
        case CoordMappingType::NegY_NegX:
            return {x - dy, y - dx};
        default:
            return {x + dx, y + dy};  // 默认情况：不做变换
        }
    }
};
#endif // MASKIMAGEPROCESSCONFIG_H
