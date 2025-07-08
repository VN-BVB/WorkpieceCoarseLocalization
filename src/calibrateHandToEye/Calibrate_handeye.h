#ifndef CALIBRATE_HANDEYE_H
#define CALIBRATE_HANDEYE_H
#include <QObject>
#include <QThread>
#include <opencv2/opencv.hpp>

#include "include/calibration.h"
#include "include/handEyeCalibration.h"
#include "include/others.h"
#include "src/cameraCalibration/CalibratateCamera.h"
#include "src/fittingWorkpieceCoordinate/Fittingworkpiececoordinate.h"

// #pragma execution_character_set("utf-8")
// #define findChessboardCorner

extern int cameraIndex;
extern std::string configFilePath;
extern std::string trackFilePath;
// extern float currentTrackPostion;
class HandEyeCalibrationLogic : public QObject {
    Q_OBJECT
public:
    HandEyeCalibrationLogic();
    ~HandEyeCalibrationLogic();
    cv::Mat Kc;
    cv::Mat camera_distortion;  // k_1, k_2, p_1, p_2, k_3
    cv::Mat Hcg;                //????????camera???????????λ?????
    // int board_width = 11;  //??????    ??????????
    // int board_heignt = 9; //??????
    // double circle_distance = 0.015;  //??????????  Aubo??????(??λ??m) 0.015  ;  ??????????(??λ??mm) 15
    int board_width = 6;             //??????    ??????????
    int board_heignt = 9;            //??????
    double circle_distance = 0.042;  //??????????  Aubo??????(??λ??m) 0.015  ;  ??????????(??λ??mm) 15
    std::string calibPath = "./data/calib/camera";
    float currentTrackPosToCalib;
    void whenCalibrateEye2Hand();
    void whenGetTrackHcg();
    void getTrackDirectiontest(std::string trackPath);
    void test(std::string trackPath);
    void getTrackDirection(std::string path, cv::Mat& HcgTrackCalib);
    double calculate_Reprojection_Error(const std::vector<cv::Point2f>& imagePoints, const std::vector<cv::Point3f>& objectPoints,
                                        const cv::Mat& TCW,  // 相机到基座的变换矩阵
                                        const cv::Mat& TWE,  // 相机到基座的变换矩阵
                                        const cv::Mat& TEB,  // 标定板到末端执行器的变换矩阵
                                        const cv::Mat& K, const cv::Mat& dist_coeffs);
signals:
    void sendSignalToTransmitCalibPara(const std::string& filename);
    void sendTotalTranslation(cv::Point3d totalTranslation);
    void appendHandEyeLog(QString message);
    void sendSaveHcg(cv::Mat Hcg);
};
class ReprojectionErrorCallback : public cv::LMSolver::Callback {
public:
    ReprojectionErrorCallback(const std::vector<std::vector<cv::Point2f>>& _imagePoints, const std::vector<cv::Point3f>& _objPoints,
                              const std::vector<cv::Mat>& _vecHg, const cv::Mat& _Kc, const cv::Mat& _camera_distortion)
        : imagePoints(_imagePoints), objPoints(_objPoints), vecHg(_vecHg), Kc(_Kc), camera_distortion(_camera_distortion) {}

    bool compute(cv::InputArray param, cv::OutputArray err, cv::OutputArray J) const override {
        static int test = 1;
        static double study = 0.5;
        qDebug() << "conpute test" << test++;
        ;
        const cv::Mat& params = param.getMat();
        cv::Mat TCW = cv::Mat::eye(4, 4, CV_64F);
        cv::Mat TEB = cv::Mat::eye(4, 4, CV_64F);
        cv::Mat TCWEpsilon = cv::Mat::eye(4, 4, CV_64F);
        cv::Mat TEBEpsilon = cv::Mat::eye(4, 4, CV_64F);
        if (err.empty()) err.create(1, 1, CV_64F);  // dim(error) = 1
        cv::Mat errorout = err.getMat();
        // 从 params 中提取前 12 个元素并 reshape 成 3x4 矩阵
        int rows = params.rows;
        if (0 <= 0 && 12 <= rows) {
            // std::cout << "Row range (0, 12) is valid!" << std::endl;
            params.rowRange(0, 12).clone().reshape(1, 3).copyTo(TCW.rowRange(0, 3).colRange(0, 4));
        } else {
            std::cerr << "Error: Row range (0, 12) is out of bounds!" << std::endl;
        }

        // 从 params 中提取后 12 个元素并 reshape 成 3x4 矩阵
        if (12 <= rows && 24 <= rows) {
            // std::cout << "Row range (12, 24) is valid!" << std::endl;
            params.rowRange(12, 24).clone().reshape(1, 3).copyTo(TEB.rowRange(0, 3).colRange(0, 4));
        } else {
            std::cerr << "Error: Row range (12, 24) is out of bounds!" << std::endl;
        }
        // // 对 TCW 的前 12 个元素进行变化限制
        // for (int i = 0; i < 12; ++i) {
        //     double referenceValue = refValues.at<double>(i);  // 参考值
        //     double currentValue = TCW.at<double>(i / 4, i % 4);  // 获取 TCW 中的当前值

        //     // 设定允许变化的最大范围，例如前 12 个值变化幅度限制为 ±0.2
        //     double maxChange = 5;

        //     // 如果当前值与参考值的差异超过最大变化范围，则将其限制在允许范围内
        //     if (std::abs(currentValue - referenceValue) > maxChange) {
        //         TCW.at<double>(i / 4, i % 4) = referenceValue + maxChange * ((currentValue - referenceValue) > 0 ? 1 : -1);
        //     }
        // }

        // 确保最后一行是 [0, 0, 0, 1]
        TCW.row(3) = cv::Mat::zeros(1, 4, CV_64F);  // 设置最后一行 [0, 0, 0, 1]
        TCW.at<double>(3, 3) = 1.0;                 // 修正为 [0, 0, 0, 1]

        TEB.row(3) = cv::Mat::zeros(1, 4, CV_64F);  // 设置最后一行 [0, 0, 0, 1]
        TEB.at<double>(3, 3) = 1.0;                 // 修正为 [0, 0, 0, 1]
        // 约束 TCW 和 TEB 的前 3x3 为单位正交矩阵
        enforceOrthogonality(TCW);  // 强制 TCW 的前 3x3 为单位正交矩阵
        enforceOrthogonality(TEB);  // 强制 TEB 的前 3x3 为单位正交矩阵

        double totalError = 0.0;  // 用于保存所有图像的误差总和
        for (size_t i = 0; i < imagePoints.size(); i++) {
            totalError += calculate_Reprojection_Error(imagePoints[i], objPoints, TCW, vecHg[i], TEB, Kc, camera_distortion);
            // totalError += error;  // 将当前图像的误差加到总误差中
        }
        // 将总误差转换为 cv::Mat（目标函数输出）
        errorout.at<double>(0, 0) = totalError;  // 总误差作为目标函数值
        std::cout << "totalError" << totalError << std::endl;
        // 计算雅可比矩阵
        if (!J.needed())
            return true;
        else if (J.empty())
            J.create(/*imagePoints.size()*/ 1, 24, CV_64F);
        cv::Mat JMat = J.getMat();
        const double epsilon = 20 * study / test;
        cv::Mat perturbedParams = params.clone();
        // 检查 rowRange 是否有效

        for (int i = 0; i < 24; ++i) {  // 修改这里的循环范围
            // std::cout<<"perturbedParams"<<perturbedParams.at<double>(i)<<std::endl;
            perturbedParams.at<double>(i) += epsilon;
            // std::cout<<"epsilonperturbedParams"<<perturbedParams.at<double>(i)<<std::endl;
            double newError = 0.0;
            perturbedParams.rowRange(0, 12).clone().reshape(1, 3).copyTo(TCWEpsilon.rowRange(0, 3).colRange(0, 4));
            perturbedParams.rowRange(12, 24).clone().reshape(1, 3).copyTo(TEBEpsilon.rowRange(0, 3).colRange(0, 4));
            TCWEpsilon.row(3) = cv::Mat::zeros(1, 4, CV_64F);
            TCWEpsilon.at<double>(3, 3) = 1.0;
            TEBEpsilon.row(3) = cv::Mat::zeros(1, 4, CV_64F);
            TEBEpsilon.at<double>(3, 3) = 1.0;
            // 约束 TCW 和 TEB 的前 3x3 为单位正交矩阵
            enforceOrthogonality(TCWEpsilon);  // 强制 TCW 的前 3x3 为单位正交矩阵
            enforceOrthogonality(TEBEpsilon);  // 强制 TEB 的前 3x3 为单位正交矩阵
            std::cout << "tkb-------------------" << i << std::endl;
            for (size_t j = 0; j < imagePoints.size(); j++) {
                newError += calculate_Reprojection_Error(imagePoints[j], objPoints, TCWEpsilon, vecHg[j], TEBEpsilon, Kc, camera_distortion);
            }
            // qDebug()<<"newError"<<newError;
            JMat.at<double>(0, i) = (newError - totalError) / epsilon;  // 计算雅可比矩阵
        }
        // std::cout<<"size1"<<params.size()<<std::endl;
        // std::cout<<"size"<<JMat.size()<<std::endl;
        return true;
    }

    void enforceOrthogonality(cv::Mat& matrix) const;
    double calculate_Reprojection_Error(const std::vector<cv::Point2f>& imagePoints, const std::vector<cv::Point3f>& objectPoints, const cv::Mat& TCW,
                                        const cv::Mat& TWE, const cv::Mat& TEB, const cv::Mat& K, const cv::Mat& dist_coeffs) const;

private:
    const std::vector<std::vector<cv::Point2f>>& imagePoints;
    const std::vector<cv::Point3f>& objPoints;
    const std::vector<cv::Mat>& vecHg;
    const cv::Mat& Kc;
    const cv::Mat& camera_distortion;
    cv::Mat refValues = (cv::Mat_<double>(12, 1) << -0.001720457820086829, -0.9999436738515231, -0.01047325876737607, 0.2442762011199959,
                         -0.9998644588949632, 0.001548644461831869, 0.01639102007239512, -0.7893348578291108, -0.01637387747517729,
                         0.01050003926897141, -0.9998108047584747, 1.843818786835483);
};
#endif  // HANDEYECALIBRATIONLOGIC_H
