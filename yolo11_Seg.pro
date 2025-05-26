QT     += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

QMAKE_CXXFLAGS_RELEASE = -ZI -MD
QMAKE_LFLAGS_RELEASE = /DEBUG
QMAKE_CXXFLAGS += /MP
SOURCES += \
    WorkpieceCoarseLocalization.cpp \
    main.cpp \
    src/camera_control/basler/BaslerControl.cpp \
    src/fittingWorkpieceCoordinate/Fittingworkpiececoordinate.cpp \
    src/utils/image_widget/QImageWidget.cpp \
    src/utils/image_widget/ScalableGraphicsView.cpp \
    src/yoloInference/Yolo11Inference.cpp \
    src/yoloInference/yolo11-seg.cpp \
    src/yoloInference/yolov11-rect.cpp

HEADERS += \
    WorkpieceCoarseLocalization.h \
    include/MyMatrix.h \
    include/maskImageProcessConfig.hpp \
    src/camera_control/basler/BaslerControl.h \
    src/fittingWorkpieceCoordinate/Fittingworkpiececoordinate.h \
    src/utils/image_widget/QImageWidget.h \
    src/utils/image_widget/ScalableGraphicsView.h \
    src/yoloInference/Yolo11Inference.h \
    src/yoloInference/yolo11-seg.h \
    src/yoloInference/common.hpp \
    src/yoloInference/yolov11-rect.h

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
