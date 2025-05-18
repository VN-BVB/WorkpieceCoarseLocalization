#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QMetaType>
#include <QMouseEvent>
#include <src/utils/image_widget/ScalableGraphicsView.h>
#include <src/camera_and_laser_plane_calibration/CalibratateCamera.h>
#include "src/camera_control/basler/BaslerControl.h"
#include "src/yolo11SegInference/Yolo11Inference.h"
#include "src/fittingWorkpieceCoordinate/Fittingworkpiececoordinate.h"
#include "src/Calibrate_HandToEye/Calibrate_handeye.h"
#include "src/Aubo/robot_control.h"
#pragma execution_character_set("utf-8")
QT_BEGIN_NAMESPACE
namespace Ui {
class WorkpieceCoarseLocalization;
}
QT_END_NAMESPACE
extern std::string inferencePath ;//推理路径
extern std::string calibCameraNum;
extern std::string configFilePath;//配置保存路径
extern std::string trackFilePath;
struct workpieceBoxInWorld{
    std::vector<std::pair<cv::Point3d, cv::Point3d>> workpieceAreaRect; // <center, topleft>
    std::vector<std::vector<cv::Rect_<double>>> weldAreaRect;//工件焊缝区域信息
    cv::Mat TrackDirection; // 地轨方向向量
};
class WorkpieceCoarseLocalization : public QWidget  {
    Q_OBJECT
public:
    WorkpieceCoarseLocalization(QWidget *parent = nullptr);
    ~WorkpieceCoarseLocalization();

private:
    Ui::WorkpieceCoarseLocalization *ui;

    //世界坐标系下工件信息

    workpieceBoxInWorld workpieceBoxInfoInWorld;

    Yolo11SegInference *yolo11SegInference = new Yolo11SegInference;  // 分割类
    Yolo11RectInference *yolo11RectInference = new Yolo11RectInference;// 目标检测类
    FittingWorkpieceCoordinate *fittingWorkpieceCoordinate = new FittingWorkpieceCoordinate;//工件拟合
    BaslerControl *baslerControl = new BaslerControl;  // 相机类
    CalibratateCamera *calibratateCamera = new CalibratateCamera; //相机标定
    HandEyeCalibrationLogic *eyeToHandCalibration = new HandEyeCalibrationLogic;//手眼标定类
    RobotController *robot = new RobotController;

    QThread *inferenceSubThread = new QThread;              // 深度学习推理线程
    QThread *cameraCalibrationSubThread = new QThread;      // 相机标定线程
    QThread *eyeToHandCalibrationSubThread = new QThread;      // 手眼标定线程
    QThread *cameraControlSubThread = new QThread;          // 相机线程
    QThread *fittingWorkpieceSubThread = new QThread;       // 坐标拟合线程
    QThread *robotControlSubThread = new QThread;            // 机器人线程

    QGraphicsScene* scene = new QGraphicsScene;  // 创建一个 QGraphicsScene
    bool detectionEnabled;
    void sortWorkpieceBoxInfoByY(workpieceBoxInWorld &boxInfo);
private slots:
    void on_btnConnectCamera_clicked();

    void on_btnSaveImage_clicked();

    void on_btnInferPath_clicked();

    void on_btnCalibratateCamera_clicked();

    void on_btnStartInfer_clicked();

    void on_btn_VerifyCoordinates_clicked();

    void on_comboCameras_currentTextChanged(const QString &currentCamera);

    void on_btnDisconnectCamera_clicked();

    void on_btnRobotConnect_clicked();

    void on_btnRobotDisConnect_clicked();

    void on_btn_calibEyetoHand_clicked();

    void on_btn_calibTrack_clicked();

public slots:
    void startCoarseLocalization();
    void whenGetInferResult(cv::Mat res);
    void whenAppendLog(const QString message);
    void whenGetImage(cv::Mat res);
    void whenGetWorkpieceResult(cv::Mat res);
    void whenGetWorkpieceRailMap(cv::Mat res);
    void whenUpdateComboBox(const std::vector<std::string> &SerialNumbers);
    void whenViewWorldCoordinateLabel(int x, int y);
    void whenGetWeldBoxInfo(const std::vector<std::vector<std::array<double, 4>>> &boxInfos);
    void whenGetResultInfo(const std::vector<cv::Point3d> resultCenters, const std::vector<cv::Point3d> resultLeftTop);
    std::shared_ptr<workpieceBoxInWorld> getLocalizationResult();

signals:
    void sendCommandToInferPath(std::string path);
    void sendDisconnectCamera();
    void initCameraThread();
    void sendSignalToCalibratate();
    void sendSignalToSaveCalibPara();
    void sendVerifyCoordinatesInManual();
    void sendEyeToHandCalib();
    void sendGetTrackHcg();
    void sendRobotConnect();
    void sendRobotDisconnect();
    void sendGetCurrentWaypoint();

};
#endif  // MAINWINDOW_H
