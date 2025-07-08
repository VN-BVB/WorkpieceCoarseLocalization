# TensorRT
INCLUDEPATH += 'E:/Install package/cuda117/TensorRT-8.4.1.5.Windows10.x86_64.cuda-11.6.cudnn8.4/TensorRT-8.4.1.5/include'
LIBS += 'E:/Install package/cuda117/TensorRT-8.4.1.5.Windows10.x86_64.cuda-11.6.cudnn8.4/TensorRT-8.4.1.5/lib/*.lib'

# CUDA
INCLUDEPATH += 'C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v11.7/include'
LIBS += 'C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v11.7/lib/x64/*.lib'

# VTK
INCLUDEPATH += E:/ProgramData/VTK/include/vtk-8.0
include(E:/ProgramData/VTK/VTK800.pri)

# PCL
INCLUDEPATH += E:/ProgramData/PCL1.8.1new/PCL1.8.1/include/pcl-1.8/
INCLUDEPATH += E:/ProgramData/PCL1.8.1new/PCL1.8.1/include/pcl-1.8/pcl/
INCLUDEPATH += E:/ProgramData/PCL1.8.1new/PCL1.8.1/3rdParty/Boost/include/boost-1_64/
INCLUDEPATH += E:/ProgramData/PCL1.8.1new/PCL1.8.1/3rdParty/Boost/include/boost-1_64/boost/
INCLUDEPATH += E:/ProgramData/PCL1.8.1new/PCL1.8.1/3rdParty/Eigen/eigen3/
INCLUDEPATH += E:/ProgramData/PCL1.8.1new/PCL1.8.1/3rdParty/FLANN/include/
INCLUDEPATH += E:/ProgramData/PCL1.8.1new/PCL1.8.1/3rdParty/FLANN/include/flann/
INCLUDEPATH += E:/ProgramData/PCL1.8.1new/PCL1.8.1/3rdParty/OpenNI2/Include/
INCLUDEPATH += E:/ProgramData/PCL1.8.1new/PCL1.8.1/3rdParty/Qhull/include/
INCLUDEPATH += E:/ProgramData/PCL1.8.1new/PCL1.8.1/3rdParty/VTK/include/vtk-8.0
include(E:/ProgramData/PCL1.8.1new/PCL181.pri)

# OpenCV
INCLUDEPATH += E:/ProgramData/opencv/build/include/
INCLUDEPATH += E:/ProgramData/opencv/build/include/opencv2/
LIBS += -LE:/ProgramData/opencv/build/x64/vc15/lib/ -lopencv_world440

# Basler
INCLUDEPATH += E:/ProgramData/Basler/pylon6/Development/include
INCLUDEPATH += E:/ProgramData/Basler/pylon6/Development/include/Base
INCLUDEPATH += E:/ProgramData/Basler/pylon6/Development/include/GenApi
INCLUDEPATH += E:/ProgramData/Basler/pylon6/Development/include/genapic
INCLUDEPATH += E:/ProgramData/Basler/pylon6/Development/include/pylon
INCLUDEPATH += E:/ProgramData/Basler/pylon6/Development/include/pylonc
LIBS += E:/ProgramData/Basler/pylon6/Development/lib/x64/GCBase_MD_VC141_v3_1_Basler_pylon.lib
LIBS += E:/ProgramData/Basler/pylon6/Development/lib/x64/GenApi_MD_VC141_v3_1_Basler_pylon.lib
LIBS += E:/ProgramData/Basler/pylon6/Development/lib/x64/PylonBase_v6_3.lib
LIBS += E:/ProgramData/Basler/pylon6/Development/lib/x64/PylonC.lib
LIBS += E:/ProgramData/Basler/pylon6/Development/lib/x64/PylonGUI_v6_3.lib
LIBS += E:/ProgramData/Basler/pylon6/Development/lib/x64/PylonUtility_v6_3.lib

# TJ_TCP
INCLUDEPATH += E:/ProgramData/TJ_Projector/TCP
LIBS += E:/ProgramData/TJ_Projector/TCP/TJSTProjectorApi.lib

# 引入plog日志库
INCLUDEPATH += ./3rdparty/plog/include

# 引入cereal序列化饭序列化库
INCLUDEPATH += ./3rdparty/cereal/include

# 宝元机器人
INCLUDEPATH += ./3rdParty/BaoYuanRobot
LIBS += ./3rdParty/BaoYuanRobot/sc2_vc_x64.lib
