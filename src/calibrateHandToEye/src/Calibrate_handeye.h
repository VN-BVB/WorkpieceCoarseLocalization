#ifndef CALIBRATE_HANDEYE_H
#define CALIBRATE_HANDEYE_H
#include <opencv2/opencv.hpp>   // Include OpenCV API
#include "include/handEyeCalibration.h"
#include "include/calibration.h"
#include "include/others.h"
class HandEyeCalibrationLogic {
public:
    HandEyeCalibrationLogic();
    ~HandEyeCalibrationLogic();

    bool calibrate(const std::string& imagePath, const std::string& posePath, const std::string& resultPath);

private:
    cv::Mat Kc = (cv::Mat_<double>(3, 3) <<
                      1.857894e+03, 0, 7.707130e+02,
                  0, 1.857929e+03, 5.644370e+02,
                  0, 0, 1 );
    vector<double> camera_distortion = { -9.431161e-02, 1.898391e-01, -4.771411e-05, -2.115818e-04, 0 };//k_1, k_2, p_1, p_2, k_3

    int board_width = 11;  //标定板宽    实际标定板参数
    int board_heignt = 9; //标定板高
    double circle_distance = 0.015;  //特征点间距  Aubo机器人(单位是m) 0.015  ;  工厂机器人(单位是mm) 15
    std::string imagePath = "../data/eye_to_hand/Aubo/images/";   //images   images_2   Aubo  second_factory
    std::string posePath = "../data/eye_to_hand/Aubo/pose/"; //对应棋盘格图像的机械臂位姿
    std::string resultPath = "../data/eye_to_hand/Aubo/pose/";//输出手眼标定结果
    cv::Mat calibrate();
};

#endif // HANDEYECALIBRATIONLOGIC_H
