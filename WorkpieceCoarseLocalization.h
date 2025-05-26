#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMetaType>
#include <QMouseEvent>
#include <QThread>

#include "src/camera_control/basler/BaslerControl.h"
#include "src/fittingWorkpieceCoordinate/Fittingworkpiececoordinate.h"
#include "src/utils/image_widget/ScalableGraphicsView.h"
#include "src/yoloInference/Yolo11Inference.h"
#pragma execution_character_set("utf-8")
QT_BEGIN_NAMESPACE
namespace Ui {
class WorkpieceCoarseLocalization;
}
QT_END_NAMESPACE

class WorkpieceCoarseLocalization : public QWidget {
    Q_OBJECT
public:
    WorkpieceCoarseLocalization(QWidget *parent = nullptr);
    ~WorkpieceCoarseLocalization();

private:
    Ui::WorkpieceCoarseLocalization *ui;

    Yolo11SegInference *yolo11SegInference = new Yolo11SegInference;                          // 分割类
    Yolo11RectInference *yolo11RectInference = new Yolo11RectInference;                       // 目标检测类
    FittingWorkpieceCoordinate *fittingWorkpieceCoordinate = new FittingWorkpieceCoordinate;  // 工件拟合
    BaslerControl *baslerControl = new BaslerControl;                                         // 相机类

    QThread *inferenceSubThread = new QThread;         // 深度学习推理线程
    QThread *cameraControlSubThread = new QThread;     // 相机线程
    QThread *fittingWorkpieceSubThread = new QThread;  // 坐标拟合线程

    QGraphicsScene *scene = new QGraphicsScene;  // 创建一个 QGraphicsScene
    bool detectionEnabled;
    std::shared_ptr<workpieceBoxInWorld> resultPtr;

private slots:
    void on_btnConnectCamera_clicked();

    void on_btnSaveImage_clicked();

    void on_btnInferPath_clicked();

    void on_btnStartInfer_clicked();

    void on_btn_VerifyCoordinates_clicked();

    void on_comboCameras_currentTextChanged(const QString &currentCamera);

    void on_btnDisconnectCamera_clicked();

    void on_btnGetWorkpieceInfo_clicked();

    void on_comboWorkpieceNum_currentIndexChanged(int index);

public slots:
    void startCoarseLocalization();
    void whenGetInferResult(cv::Mat res);
    void whenAppendLog(const QString message);
    void whenGetImage(cv::Mat res);
    void whenGetWorkpieceResult(cv::Mat res);
    void whenGetWorkpieceRailMap(cv::Mat res);
    void whenUpdateComboBox(const std::vector<std::string> &SerialNumbers);
    void whenUpdateComboWp(const int size);
    void whenViewWorldCoordinateLabel(int x, int y);
    void getLocalizationResult(const workpieceBoxInWorld &workpieceBoxInfoInWorld);
    void printWorkpieceBoxInfo(const workpieceBoxInWorld *info, int workpieceIndex);
<<<<<<< HEAD

=======
    void debugProjectPointOnlyY(const cv::Mat &trackDirection, const cv::Point3d &pt);
>>>>>>> 14f67258beab6f9128e2af30f1270ba52e11fde4
signals:
    void sendCommandToInferPath(std::string path);
    void sendDisconnectCamera();
    void sendOpenCamera();
    void sendSignalToCalibratate();
    void sendSignalToSaveCalibPara();
    void sendVerifyCoordinatesInManual();
    void sendRobotConnect();
    void sendRobotDisconnect();
};
#endif  // MAINWINDOW_H
