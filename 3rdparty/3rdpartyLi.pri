#------------------OpenCV库-------------------
# INCLUDEPATH += D:/ProgramData/opencv/build/include/
# INCLUDEPATH += D:/ProgramData/opencv/build/include/opencv2/
# LIBS += -LD:/ProgramData/opencv/build/x64/vc15/lib/ -lopencv_world440
INCLUDEPATH += D:/qt/opencv/opencv/build/include
CONFIG(debug, debug|release){
    LIBS += D:/qt/opencv/opencv/build/x64/vc15/lib/opencv_world460d.lib
} else {
    LIBS += D:/qt/opencv/opencv/build/x64/vc15/lib/opencv_world460.lib
}

# VTK
# CONFIG(debug, debug|release){
#     INCLUDEPATH += $$quote(D:/qt/vtkmsvc/include/vtk-8.2)
#     LIBS += $$quote(D:/qt/vtkmsvc/lib/vtk*d.lib)
# } else {
#     INCLUDEPATH += $$quote(D:/qt/vtkmsvc/include/vtk-8.2)
#     LIBS += $$quote(D:/qt/vtkmsvc/lib/vtk*.lib)
# }

# 引入Eigen矩阵运算库
INCLUDEPATH += D:\ProgramData\eigen-git-mirror-master

# # 引入PCL点云库
include(D:\ProgramData\PCL1.9.1new\PCL191new.pri)

# 引入Coin3d机器人显示库
INCLUDEPATH += D:/ProgramData/Coin3d/include
LIBS += D:\ProgramData\Coin3d\lib\*.lib
# # 引入cere
INCLUDEPATH += D:\ProgramData\cerea\ceres\include
LIBS += D:\ProgramData\cerea\ceres\lib\*.lib
# LIBS += -lglog

# 引入cereal序列化饭序列化库
INCLUDEPATH += ./3rdparty/cereal/include

# # CUDA合并TensorRT12.6
# INCLUDEPATH += 'D:/YOLO/NVIDIA GPU Computing Toolkit/CUDA/v12.6/include'
# LIBS += 'D:/YOLO/NVIDIA GPU Computing Toolkit/CUDA/v12.6/lib/x64/*.lib'
# CUDA合并TensorRT
INCLUDEPATH += 'D:/YOLO/NVIDIA GPU Computing Toolkit/CUDA/v11.8/include'
LIBS += 'D:/YOLO/NVIDIA GPU Computing Toolkit/CUDA/v11.8/lib/x64/*.lib'
INCLUDEPATH += 'D:/YOLO/NVIDIA GPU Computing Toolkit/TensorRT-8.6.1.6.Windows10.x86_64.cuda-11.8/TensorRT-8.6.1.6/include'
LIBS += 'D:/YOLO/NVIDIA GPU Computing Toolkit/TensorRT-8.6.1.6.Windows10.x86_64.cuda-11.8/TensorRT-8.6.1.6/lib/*.lib'




# 引入Basler相机库
INCLUDEPATH += $$quote(D:/Basler/pylon7/Development/include)
LIBS +=  $$quote(D:/Basler/pylon7/Development/lib/x64/*.lib)

# # 引入AUBO机器人库
# INCLUDEPATH += ./3rdparty/auboi5
# INCLUDEPATH += ./3rdparty/auboi5/inc
# LIBS += ./3rdparty/auboi5/lib/libserviceinterface.lib

# 引入镭烁HD6相机库
INCLUDEPATH += ./3rdparty/HD6
LIBS += ./3rdparty/HD6/LS_HD6.lib



