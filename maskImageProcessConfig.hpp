#ifndef MASKIMAGEPROCESSCONFIG_H
#define MASKIMAGEPROCESSCONFIG_H
#include <opencv2/opencv.hpp>
//------------------------------画布绘制参数---------------------------------------
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
const cv::Mat canvasMat = (cv::Mat_<double>(3, 3) <<   1, 0, pixelCol-100,
                                                 0, -1, 500,
                                                 0, 0, 1); //绘制坐标系偏移
const int railMapRotationAngle = 180;// 0 90 180 270  画布最后可视化的角度
//------------------------------推理掩膜变换---------------------------------------
// 定义扩展后的画布大小(粗定位焊缝推理)
const int expandedWidth = 1024;
const int expandedHeight = 1024;
const int AdjustWorkpieceResolution = 1;//调整工件掩膜分辨率倍数
const int rectRotationAngle =90;//90 180 270 //旋转工件提高召回率
#endif // MASKIMAGEPROCESSCONFIG_H
