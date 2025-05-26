QT     += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

QMAKE_CXXFLAGS_RELEASE = -ZI -MD
QMAKE_LFLAGS_RELEASE = /DEBUG

SOURCES += \
    WorkpieceCoarseLocalization.cpp \
    main.cpp \
    src/Calibrate_HandToEye/Calibrate_handeye.cpp \
    src/Calibrate_HandToEye/src/calibration.cpp \
    src/Calibrate_HandToEye/src/handEyeCalibration.cpp \
    src/Calibrate_HandToEye/src/others.cpp \
    src/camera_and_laser_plane_calibration/CalibratateCamera.cpp \
    src/camera_and_laser_plane_calibration/CameraAndLaserPlaneCalibration.cpp \
    src/camera_and_laser_plane_calibration/LibDetect.cpp \
    src/camera_control/basler/BaslerControl.cpp \
    src/fittingWorkpieceCoordinate/Fittingworkpiececoordinate.cpp \
    src/utils/image_widget/QImageWidget.cpp \
    src/utils/image_widget/ScalableGraphicsView.cpp \
    src/yolo11SegInference/Yolo11Inference.cpp \
    src/yolo11SegNormal/yolo11-seg.cpp \
    src/yolov11WeldSeamArea/yolov11-rect.cpp

HEADERS += \
    WorkpieceCoarseLocalization.h \
    maskImageProcessConfig.hpp \
    src/Calibrate_HandToEye/include/Camera_Calibration.h \
    src/Calibrate_HandToEye/include/calibration.h \
    src/Calibrate_HandToEye/include/handEyeCalibration.h \
    src/Calibrate_HandToEye/include/others.h \
    src/Calibrate_HandToEye/Calibrate_handeye.h \
    src/camera_and_laser_plane_calibration/CalibratateCamera.h \
    src/camera_and_laser_plane_calibration/CameraAndLaserPlaneCalibration.h \
    src/camera_and_laser_plane_calibration/LibDetect.h \
    src/camera_and_laser_plane_calibration/MyMatrix.h \
    src/camera_control/basler/BaslerControl.h \
    src/fittingWorkpieceCoordinate/Fittingworkpiececoordinate.h \
    src/utils/image_widget/QImageWidget.h \
    src/utils/image_widget/ScalableGraphicsView.h \
    src/yolo11SegInference/Yolo11Inference.h \
    src/yolo11SegNormal/yolo11-seg.h \
    src/yolo11SegNormal/common.hpp \
    src/yolov11WeldSeamArea/yolov11-rect.h

FORMS += \
    WorkpieceCoarseLocalization.ui

include( ./3rdparty/3rdpartyRom.pri )
INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

INCLUDEPATH += $$PWD/./src
DEPENDPATH += $$PWD/./src
INCLUDEPATH += $$PWD/./3rdparty/plog/include


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
