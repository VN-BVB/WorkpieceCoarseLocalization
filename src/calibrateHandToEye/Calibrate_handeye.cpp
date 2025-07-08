#include "Calibrate_handeye.h"
HandEyeCalibrationLogic::HandEyeCalibrationLogic() {}

HandEyeCalibrationLogic::~HandEyeCalibrationLogic() {}

void HandEyeCalibrationLogic::whenCalibrateEye2Hand() {
    if (cameraIndex == 0) {
        emit appendHandEyeLog(QString("未选择相机"));
        return;
    }
    std::string imagePathEyeToHand = calibPath + std::to_string(cameraIndex) + "/img";  // images   images_2   Aubo  second_factory
    std::string posePath = calibPath + std::to_string(cameraIndex) + "/pos";            // 对应棋盘格图像的机械臂位姿
    Kc = cameraParameters[cameraIndex - 1].cameraMatrix;
    camera_distortion = cameraParameters[cameraIndex - 1].distCoeffs;
    cv::Size boardSize = {board_width, board_heignt};  // 标定板规格
    vector<cv::Point3f> objPoints;

    // 输出标定板对应的世界坐标点集
    calculate_Object_Points(board_width, board_heignt, circle_distance, objPoints);
    // 输出标定板的图像坐标点集
    std::vector<std::vector<cv::Point2f>> imagePoints;
#ifdef findChessboardCorner
    calculate_Image_Points_ChessboardCorners(imagePathEyeToHand, boardSize, imagePoints);
#else
    calculate_Image_Points(imagePathEyeToHand, boardSize, imagePoints);
#endif
    // 已知内参标定外参  vecHc为n组棋盘到相机坐标系的Rt(4*4)矩阵
    std::vector<cv::Mat> vecHc;
    Calibration_Solve_Extrinsics(Kc, camera_distortion, objPoints, imagePoints, vecHc);  // 已知内参求外参

    std::vector<cv::Mat> R_gripper2base;
    std::vector<cv::Mat> t_gripper2base;
    std::vector<cv::Mat> R_target2cam;
    std::vector<cv::Mat> t_target2cam;

    cv::Mat R_cam2gripper = (cv::Mat_<double>(3, 3));
    cv::Mat t_cam2gripper = (cv::Mat_<double>(3, 1));

    // 读取相机标定信息文件和机器人位姿信息文件
    // vecHg为n组基坐标系到末端姿态的Rt(4*4)矩阵
    std::vector<cv::Mat> vecHg;
    bool ret = HandEyeCalibration::readRobotDatasFromFile(posePath, vecHg, false);  // aubo-false

    // 方法1，Tsai两步法手眼标定，基于AX=XB，先求An,Bn,再求X，即Hcg
    if (0) {
        // std::string resultPath = "./data/calib/camera1/eye_to_hand/result";//输出手眼标定结果
        // HandEyeCalibration::calibrateEyeInHand(Hcg, vecHg, vecHc, HandEyeCalibration::HAND_EYE_TSAI, resultPath +
        // "EyeInHandMatrix.yml");//5.6ms
    }
    // 方法2，opencv自带函数，CALIB_HAND_EYE_TSAI比较快，精度也还不错。两方法二选一
    else if (1) {
        cv::Mat tempR, tempT;
        for (size_t i = 0; i < vecHg.size(); i++) {
            RT2R_T(vecHg[i], tempR, tempT);  // 位姿矩阵分解

            R_gripper2base.push_back(tempR);
            t_gripper2base.push_back(tempT);
        }
        for (size_t i = 0; i < vecHg.size(); i++) {
            RT2R_T(vecHc[i], tempR, tempT);

            R_target2cam.push_back(tempR);
            t_target2cam.push_back(tempT);
        }
        // 该函数应该是仅在opencv4.1.2以上才有
        cv::calibrateHandEye(R_gripper2base, t_gripper2base, R_target2cam, t_target2cam, R_cam2gripper, t_cam2gripper,
                             cv::CALIB_HAND_EYE_PARK);  // CALIB_HAND_EYE_TSAI  CALIB_HAND_EYE_HORAUD  CALIB_HAND_EYE_PARK
        Hcg = R_T2RT(R_cam2gripper, t_cam2gripper);
        // cv::Mat Hcginv  = (cv::Mat_<double>(4, 4) <<
        //                    0.9984397834079223, -0.01412124659420459, -0.05402396970759443, 81.43850783523575,
        //                0.05112694691904834, -0.1578020064128438, 0.986146318793929, 76.20937287027097,
        //                0.02245070615990125, 0.9873697975767268, 0.1568338248791225, 79.21900104146491,
        //                0, 0, 0, 1);
        // bool success = cv::invert(Hcginv, Hcg, cv::DECOMP_SVD);  // 使用SVD方法求逆
        // std::cout<<"Hcg"<<Hcg;
        // cv::Mat Hcginv = Hcg.inv();
        // std::cout<<"Hcginv"<<Hcginv;
    }
    std::cout << "Hcg 矩阵为： " << std::endl;
    std::cout << Hcg << std::endl << HandEyeCalibration::isRotationMatrix(Hcg) << std::endl << std::endl;  // 判断是否为旋转矩阵

    // N张图片下的 机械臂基座下标定板XYZ为：
    std::cout << "机械臂下标定板XYZ为：" << std::endl;
    vecWorldX.clear();
    vecWorldY.clear();
    vecWorldZ.clear();
    for (int i = 0; i < vecHc.size(); ++i) {
        cv::Mat cheesePos{0.0, 0.0, 0.0, 1.0};  // 4*1矩阵，单独求机械臂下，标定板的xyz
        cv::Mat worldPos = vecHg[i] * Hcg * vecHc[i] * cheesePos;
        // std::cout << i << ": " << worldPos.t() << std::endl;
        vecWorldX.push_back(worldPos.at<double>(0, 0));  // 标定板在机械臂基坐标系的x坐标
        vecWorldY.push_back(worldPos.at<double>(1, 0));  // 标定板在机械臂基坐标系的y坐标
        vecWorldZ.push_back(worldPos.at<double>(2, 0));  // 标定板在机械臂基坐标系的z坐标
    }

    // 求取各个维度的均值和标准差
    double stdevX, stdevY, stdevZ, meanX, meanY, meanZ;
    calc_stdev(vecWorldX, stdevX, meanX);
    calc_stdev(vecWorldY, stdevY, meanY);
    calc_stdev(vecWorldZ, stdevZ, meanZ);

    // Aubo机器人结果输出
    emit appendHandEyeLog(QString("X方向均标准差为: %1 mm").arg(stdevX * 1000.0));
    emit appendHandEyeLog(QString("Y方向均标准差为: %1 mm").arg(stdevY * 1000.0));
    emit appendHandEyeLog(QString("Z方向均标准差为: %1 mm").arg(stdevZ * 1000.0));
    // 保存json
    cv::Mat HcgSave = Hcg.clone();  // 创建一个新的Mat来保存修改后的Hcg矩阵
    // 修改 Hcg 的前三行第四列的值（乘以1000）
    for (int i = 0; i < 3; ++i) {
        HcgSave.at<double>(i, 3) = Hcg.at<double>(i, 3) * 1000;  // 修改对应值
    }
    std::cout << "HcgSave" << HcgSave << std::endl;
    emit sendSaveHcg(HcgSave);

    // 数据可视化分析
    /*可将各标定板在基座标系下的x,y,z的分布折线图画出，明显偏离的予以剔除（例如Z方向，可先求平均，然后求各个Z值与平均的偏差，偏差大的剔除）
    plot的横轴和纵轴定义*/
    vector<double> plot_horizontal_coordinates(vecWorldX.size());  // 横轴
    vector<double> plot_verticalX_coordinates(vecWorldX.size());   // X方向纵轴
    vector<double> plot_verticalY_coordinates(vecWorldX.size());   // Y方向纵轴
    vector<double> plot_verticalZ_coordinates(vecWorldX.size());   // Z方向纵轴
    for (int i = 0; i < plot_horizontal_coordinates.size(); i++) {
        plot_horizontal_coordinates[i] = i;
        plot_verticalX_coordinates[i] = (vecWorldX[i] - meanX) * 1000;
        plot_verticalY_coordinates[i] = (vecWorldY[i] - meanY) * 1000;
        plot_verticalZ_coordinates[i] = (vecWorldZ[i] - meanZ) * 1000;
    }
    ////三个方向的折线图
    draw_line_chart(plot_horizontal_coordinates, plot_verticalX_coordinates, "X");
    draw_line_chart(plot_horizontal_coordinates, plot_verticalY_coordinates, "Y");
    draw_line_chart(plot_horizontal_coordinates, plot_verticalZ_coordinates, "Z");

    //*********************************************结束****************************************//
    return;
}
void HandEyeCalibrationLogic::whenGetTrackHcg() {
    if (Hcg.empty()) {
        emit appendHandEyeLog(QString("未进行手眼标定"));
        return;
    }
    if (cameraIndex == 0) {
        emit appendHandEyeLog(QString("未选择相机"));
        return;
    }
    std::string trackPath = calibPath + std::to_string(cameraIndex) + "/compare";
    cv::Mat HcgTrack;
    getTrackDirection(trackPath, HcgTrack);
    emit sendSaveHcg(HcgTrack);
    // getTrackDirectiontest(trackPath);
    // test(trackPath);
}
void HandEyeCalibrationLogic::getTrackDirection(std::string path, cv::Mat& HcgTrackCalib) {  // 指向原点移动
    std::vector<cv::Mat> cvImages;
    std::vector<std::string> imagePathList;
    cv::glob(path + "/*.bmp", imagePathList);
    for (auto& path : imagePathList) {
        cv::Mat image = cv::imread(path);
        cvImages.push_back(image);
    }
    // 标定板规格
    cv::Size boardSize = {board_width, board_heignt};
    std::vector<std::vector<cv::Point2f>> imagePoints;
    vector<cv::Point3f> objPoints;
    calculate_Object_Points(board_width, board_heignt, circle_distance, objPoints);
#ifdef findChessboardCorner
    calculate_Image_Points_ChessboardCorners(path, boardSize, imagePoints);
#else
    calculate_Image_Points_V(cvImages, boardSize, imagePoints);
#endif

    // 已知内参标定外参, vecHc 为每个图像的外参矩阵
    std::vector<cv::Mat> vecHc;
    Calibration_Solve_Extrinsics(Kc, camera_distortion, objPoints, imagePoints, vecHc);

    // 存储转换后的世界坐标系中的点
    std::vector<std::vector<cv::Point3d>> worldPointsVec;

    // 将每张图像的物体点转换到机器人基座标系
    for (int i = 0; i < vecHc.size(); ++i) {
        std::vector<cv::Point3d> worldPoints;

        for (int j = 0; j < objPoints.size(); ++j) {
            cv::Mat RT_Mat = vecHc[i];  // 当前图像的外参矩阵（旋转+平移）
            cv::Mat point3d = (cv::Mat_<double>(4, 1) << objPoints[j].x, objPoints[j].y, objPoints[j].z, 1);

            // 将物体点从相机坐标系转换到机器人基坐标系
            cv::Mat robotBasePoint = Hcg * RT_Mat * point3d;

            // 提取机器人基坐标系下的 3D 坐标
            worldPoints.push_back(cv::Point3d(robotBasePoint.at<double>(0), robotBasePoint.at<double>(1), robotBasePoint.at<double>(2)));
        }

        worldPointsVec.push_back(worldPoints);
    }
    // 用于累积所有相对平移向量
    cv::Point3d totalTranslation(0.0f, 0.0f, 0.0f);
    int totalPairs = 0;                          // 用于记录总的平移向量对数
    std::vector<cv::Point3d> translationErrors;  // 用于存储每对平移向量的误差

    // 遍历所有图像对，计算平均平移向量
    for (int i = 0; i < worldPointsVec.size() - 1; ++i) {
        const auto& worldPoints1 = worldPointsVec[i];
        const auto& worldPoints2 = worldPointsVec[i + 1];

        if (worldPoints1.size() == worldPoints2.size()) {
            for (int j = 0; j < worldPoints1.size(); ++j) {
                cv::Point3d diff = worldPoints2[j] - worldPoints1[j];
                totalTranslation += diff;
                totalPairs++;

                // 计算该对平移向量与总平移向量平均值的误差
                cv::Point3d error = diff - totalTranslation / totalPairs;  // 误差向量
                translationErrors.push_back(error);                        // 存储误差
            }
        }
    }
    // 计算所有平移向量的平均值
    if (totalPairs > 0) {
        totalTranslation /= totalPairs;
        // std::cout << "totalTranslation: " << totalTranslation << std::endl;
    } else {
        std::cout << "没有有效的图像对来计算平移。" << std::endl;
    }
    emit sendTotalTranslation(totalTranslation);
    // 计算各轴方向的误差
    std::vector<double> xErrors, yErrors, zErrors;

    // 计算每个方向的误差
    for (const auto& error : translationErrors) {
        xErrors.push_back(error.x);
        yErrors.push_back(error.y);
        zErrors.push_back(error.z);
    }

    // 计算均方根误差（RMSE）和平均绝对误差（MAE）
    // 计算 x 轴方向的误差
    double mae_x = std::accumulate(xErrors.begin(), xErrors.end(), 0.0) / xErrors.size();
    emit appendHandEyeLog(QString("X轴误差的平均大小MAE: %1 mm").arg(mae_x * 1000));

    // 计算 y 轴方向的误差
    double mae_y = std::accumulate(yErrors.begin(), yErrors.end(), 0.0) / yErrors.size();
    emit appendHandEyeLog(QString("Y轴误差的平均大小MAE: %1 mm").arg(mae_y * 1000));

    // 计算 z 轴方向的误差
    double mae_z = std::accumulate(zErrors.begin(), zErrors.end(), 0.0) / zErrors.size();
    emit appendHandEyeLog(QString("Z轴误差的平均大小MAE: %1 mm").arg(mae_z * 1000));

    // 现在需要更新 Hcg 矩阵，减去计算得到的平移向量
    cv::Mat R = Hcg(cv::Rect(0, 0, 3, 3));  // 旋转矩阵
    cv::Mat t = Hcg(cv::Rect(3, 0, 1, 3));  // 原始平移向量

    // 将计算得到的平移向量转换为 Mat 类型
    double magnitude =
        sqrt(totalTranslation.x * totalTranslation.x + totalTranslation.y * totalTranslation.y + totalTranslation.z * totalTranslation.z);
    // currentTrackPosToCalib = magnitude; // 假设地轨编码器读取到的位置，实际在手眼标定函数中获取。
    currentTrackPosToCalib = 2200;  // 假设地轨编码器读取到的位置
    cv::Point3d unitDirection(totalTranslation.x / magnitude, totalTranslation.y / magnitude, totalTranslation.z / magnitude);

    // 将 unitDirection 转换为 cv::Mat
    cv::Mat unitDirectionMat = (cv::Mat_<double>(3, 1) << unitDirection.x, unitDirection.y, unitDirection.z);
    MyMatrixTrackDirection track(unitDirectionMat);
    track.appendToJsonFile(trackFilePath);
    // 更新平移向量
    std::cout << "unitDirectionMat: " << unitDirectionMat << std::endl;
    // std::cout << "t: " << t << std::endl;
    cv::Mat t_updated = t * 1000 - unitDirectionMat * currentTrackPosToCalib;
    // std::cout << "unitDirectionMat * currentTrackPosToCalib;: " << unitDirectionMat * currentTrackPosToCalib << std::endl;
    // std::cout << "t_updated: " << t_updated << std::endl;

    // 构建新的地轨矩阵
    cv::Mat Hc_updated = cv::Mat::eye(4, 4, CV_64F);
    R.copyTo(Hc_updated(cv::Rect(0, 0, 3, 3)));          // 将旋转矩阵复制到新的地轨矩阵
    t_updated.copyTo(Hc_updated(cv::Rect(3, 0, 1, 3)));  // 将更新后的平移向量复制到新的地轨矩阵

    std::cout << "Updated hand-eye matrix (ground track matrix):" << std::endl;
    std::cout << Hc_updated << std::endl;

    // 更新 Hcg 为新的地轨矩阵
    HcgTrackCalib = Hc_updated;
}
void HandEyeCalibrationLogic::test(std::string trackPath) {
    std::vector<cv::Mat> dirImages;
    std::vector<std::string> imagePathList;
    cv::glob(trackPath + "/*.bmp", imagePathList);
    for (auto& path : imagePathList) {
        cv::Mat image = cv::imread(path);
        dirImages.push_back(image);
    }
    // 手动给定的平移向量
    cv::Mat Hc1_manual1 = (cv::Mat_<double>(4, 4) << 0.9995132115610255, -0.02019904319366579, -0.02377684943435908, -0.8605622719022955,
                           -0.02025012754446433, -0.9997931213772339, -0.001909654733043817, -0.1559250071294697, -0.02373335731405568,
                           0.002390209368848928, -0.9997154668453262, 1.06690330933994, 0, 0, 0, 1);

    cv::Mat Hc2_manual1 = (cv::Mat_<double>(4, 4) << 0.9998209637165499, -0.001445359319344509, -0.01886667563052896, -0.7248343721691441,
                           -0.001446082067449214, -0.999998954118583, -2.466563952358003e-05, -0.0560321003531572, -0.01886662024751207,
                           5.194398478103324e-05, -0.9998220081305766, 1.064627607740112, 0, 0, 0, 1);
    cv::Size boardSize = {board_width, board_heignt};  // 标定板规格

    // 获取每个标定图像的图像坐标点集
    std::vector<std::vector<cv::Point2f>> imagePoints;
    vector<cv::Point3f> objPoints;

    // 计算世界坐标点集
    calculate_Object_Points(board_width, board_heignt, circle_distance, objPoints);
    calculate_Image_Points_V(dirImages, boardSize, imagePoints);

    // 已知内参标定外参, vecHc 为每个图像的外参矩阵
    std::vector<cv::Mat> vecHc;
    Calibration_Solve_Extrinsics(Kc, camera_distortion, objPoints, imagePoints, vecHc);

    // 检查 dirImages 是否有两个图像
    if (dirImages.size() != 2) {
        std::cerr << "错误：必须有两个图像来计算轨迹方向！" << std::endl;
        return;
    }

    // 获取两个标定板的外参矩阵
    cv::Mat Hc1 = vecHc[0];  // 第一个标定板的外参矩阵
    cv::Mat Hc2 = vecHc[1];  // 第二个标定板的外参矩阵

    // 计算两个标定板之间的相对平移向量
    std::vector<cv::Point3f> worldPoints1, worldPoints2;

    // 计算每个图像的物体点从标定板坐标系转换到相机坐标系
    for (int i = 0; i < objPoints.size(); ++i) {
        // 对于每个物体点，应用外参矩阵 Hc1 和 Hc2 进行转换
        cv::Mat point3d = (cv::Mat_<double>(4, 1) << objPoints[i].x, objPoints[i].y, objPoints[i].z, 1);

        // 将物体点从标定板坐标系转换到相机坐标系（使用 Hc1 和 Hc2）
        cv::Mat cameraPoint1 = Hc1 * point3d;
        cv::Mat cameraPoint2 = Hc2 * point3d;

        // 将转换后的物体点添加到对应的向量中
        worldPoints1.push_back(cv::Point3f(cameraPoint1.at<double>(0), cameraPoint1.at<double>(1), cameraPoint1.at<double>(2)));
        worldPoints2.push_back(cv::Point3f(cameraPoint2.at<double>(0), cameraPoint2.at<double>(1), cameraPoint2.at<double>(2)));
    }

    // 用于累积所有相对平移向量
    cv::Point3f totalTranslation(0.0f, 0.0f, 0.0f);
    int totalPairs = 0;  // 用于记录总的平移向量对数

    // 计算所有对应点的相对平移向量并求取平均
    for (int i = 0; i < worldPoints1.size(); ++i) {
        cv::Point3f diff = worldPoints2[i] - worldPoints1[i];
        // std::cout<<"cameradiff"<<diff<<std::endl;
        totalTranslation += diff;
        totalPairs++;  // 每次都记录一个平移向量对
    }
    cv::Mat totalTranslationMat;
    // 计算所有平移向量的平均值
    if (totalPairs > 0) {
        totalTranslation = totalTranslation / totalPairs;
        totalTranslationMat = (cv::Mat_<double>(3, 1) << totalTranslation.x, totalTranslation.y, totalTranslation.z);
        std::cout << "平均相对平移向量（基于棋盘格点）: " << totalTranslation << std::endl;
    } else {
        std::cout << "没有有效的图像点对用于计算平移。" << std::endl;
        return;
    }

    cv::Mat Hc1_manual = Hc1_manual1;
    cv::Mat Hc2_manual = Hc2_manual1;
    // 提取手动平移向量
    cv::Mat T1_manual = Hc1_manual(cv::Rect(3, 0, 1, 3));  // 获取 Hc1 的平移向量 (3x1)
    cv::Mat T2_manual = Hc2_manual(cv::Rect(3, 0, 1, 3));  // 获取 Hc2 的平移向量 (3x1)

    // 计算手动给定的平移向量之间的相对平移向量
    cv::Mat deltaT_manual = T1_manual - T2_manual;
    // 打印矩阵中的平移向量和相对平移向量
    // std::cout << "手动平移向量：" << std::endl;
    // std::cout <<"Hc1"<<Hc1_manual<<std::endl;
    // std::cout <<"Hc2"<<Hc2_manual<<std::endl;
    // std::cout << "T1（矩阵 1）: " << T1_manual.t() << std::endl;
    // std::cout << "T2（矩阵 2）: " << T2_manual.t() << std::endl;
    // std::cout << "两个轨迹方向之间的相对平移向量（手动）: " << deltaT_manual.t() << std::endl;

    // 计算误差向量（差异）
    cv::Mat errorVector = totalTranslationMat - deltaT_manual;

    // 如果需要逐个分量打印误差
    std::cout << "误差分量（单位：米）:" << std::endl;
    for (int i = 0; i < errorVector.rows; ++i) {
        std::cout << "分量 " << i << ": " << errorVector.at<double>(i, 0) * 1000 << " 毫米" << std::endl;
    }
}

void HandEyeCalibrationLogic::getTrackDirectiontest(std::string trackPath) {
    std::vector<cv::Mat> dirImages;
    std::vector<std::string> imagePathList;
    cv::glob(trackPath + "/*.bmp", imagePathList);
    for (auto& path : imagePathList) {
        cv::Mat image = cv::imread(path);
        dirImages.push_back(image);
    }
    cv::Mat Hc2_manual1 = (cv::Mat_<double>(4, 4) << 0.9995132115610255, -0.02019904319366579, -0.02377684943435908, -0.8605622719022955,
                           -0.02025012754446433, -0.9997931213772339, -0.001909654733043817, -0.1559250071294697, -0.02373335731405568,
                           0.002390209368848928, -0.9997154668453262, 1.06690330933994, 0, 0, 0, 1);
    cv::Mat Hc1 = (cv::Mat_<double>(4, 4) << 0.9998209637165499, -0.001445359319344509, -0.01886667563052896, -0.7248343721691441,
                   -0.001446082067449214, -0.999998954118583, -2.466563952358003e-05, -0.0560321003531572, -0.01886662024751207,
                   5.194398478103324e-05, -0.9998220081305766, 1.064627607740112, 0, 0, 0, 1);

    // 标定板规格
    cv::Size boardSize = {board_width, board_heignt};

    // 获取每个标定图像的图像坐标点集
    std::vector<std::vector<cv::Point2f>> imagePoints;
    vector<cv::Point3f> objPoints;

    // 计算世界坐标点集
    calculate_Object_Points(board_width, board_heignt, circle_distance, objPoints);
    calculate_Image_Points_V(dirImages, boardSize, imagePoints);

    // 已知内参标定外参, vecHc 为每个图像的外参矩阵
    std::vector<cv::Mat> vecHc;
    Calibration_Solve_Extrinsics(Kc, camera_distortion, objPoints, imagePoints, vecHc);

    // 存储转换后的世界坐标系中的点
    std::vector<std::vector<cv::Point3d>> worldPointsVec;

    // 将每张图像的物体点转换到机器人基座标系
    for (int i = 0; i < vecHc.size(); ++i) {
        std::vector<cv::Point3d> worldPoints;

        for (int j = 0; j < objPoints.size(); ++j) {
            cv::Mat RT_Mat = vecHc[i];  // 当前图像的外参矩阵（旋转+平移）
            cv::Mat point3d = (cv::Mat_<double>(4, 1) << objPoints[j].x, objPoints[j].y, objPoints[j].z, 1);

            // 将物体点从相机坐标系转换到机器人基坐标系
            cv::Mat robotBasePoint = Hc1 * RT_Mat * point3d;

            // 提取机器人基坐标系下的 3D 坐标
            worldPoints.push_back(cv::Point3d(robotBasePoint.at<double>(0), robotBasePoint.at<double>(1), robotBasePoint.at<double>(2)));
        }

        worldPointsVec.push_back(worldPoints);
    }
    // 用于累积所有相对平移向量
    cv::Point3d totalTranslation(0.0f, 0.0f, 0.0f);
    int totalPairs = 0;  // 用于记录总的平移向量对数

    // 遍历所有图像对
    for (int i = 0; i < worldPointsVec.size() - 1; ++i) {
        const std::vector<cv::Point3d>& worldPoints1 = worldPointsVec[i];
        const std::vector<cv::Point3d>& worldPoints2 = worldPointsVec[i + 1];

        if (worldPoints1.size() == worldPoints2.size()) {
            // 计算两张图像之间所有对应点的平移向量并累加
            for (int j = 0; j < worldPoints1.size(); ++j) {
                cv::Point3d diff = worldPoints2[j] - worldPoints1[j];
                // std::cout<<"robotdiff"<<diff<<std::endl;
                totalTranslation += diff;
                totalPairs++;  // 每次都记录一个平移向量对
            }
        }
    }

    // 计算所有平移向量的平均值
    if (totalPairs > 0) {
        totalTranslation = totalTranslation / totalPairs;
        std::cout << "平均相对平移向量（基于棋盘格点）2: " << totalTranslation << std::endl;
    } else {
        std::cout << "No valid pairs of images to compute translation." << std::endl;
    }
    // 现在需要更新 Hc1 矩阵，减去计算得到的平移向量
    cv::Mat R = Hc1(cv::Rect(0, 0, 3, 3));  // 旋转矩阵
    cv::Mat t = Hc1(cv::Rect(3, 0, 1, 3));  // 原始平移向量

    // 将计算得到的平移向量转换为 Mat 类型
    cv::Mat translationMat = (cv::Mat_<double>(3, 1) << totalTranslation.x, totalTranslation.y, totalTranslation.z);

    // 更新平移向量：用原平移向量减去相对平移向量
    cv::Mat t_updated = t + translationMat;

    // 构建新的地轨矩阵
    cv::Mat Hc_updated = cv::Mat::eye(4, 4, CV_64F);
    R.copyTo(Hc_updated(cv::Rect(0, 0, 3, 3)));          // 将旋转矩阵复制到新的地轨矩阵
    t_updated.copyTo(Hc_updated(cv::Rect(3, 0, 1, 3)));  // 将更新后的平移向量复制到新的地轨矩阵

    std::cout << "Updated hand-eye matrix (ground track matrix):" << std::endl;
    std::cout << Hc_updated << std::endl;

    // // 更新 Hc1 为新的地轨矩阵
    // Hc1 = Hc_updated;

    // 提取地轨矩阵 Hc1 和 Hc2_manual1 的平移向量
    cv::Mat t_Hc1 = Hc_updated(cv::Rect(3, 0, 1, 3));           // Hc_updated 的平移向量
    cv::Mat t_Hc2_manual1 = Hc2_manual1(cv::Rect(3, 0, 1, 3));  // Hc2_manual1 的平移向量

    // 计算平移向量的误差
    cv::Mat translation_error = t_Hc1 - t_Hc2_manual1;

    // 输出误差结果
    std::cout << "Translation error between Hc1 and Hc2_manual1:" << std::endl;
    std::cout << "X error: " << translation_error.at<double>(0) * 1000 << std::endl;
    std::cout << "Y error: " << translation_error.at<double>(1) * 1000 << std::endl;
    std::cout << "Z error: " << translation_error.at<double>(2) * 1000 << std::endl;
    // 单位化 totalTranslation 向量
    double magnitude =
        sqrt(totalTranslation.x * totalTranslation.x + totalTranslation.y * totalTranslation.y + totalTranslation.z * totalTranslation.z);
    cv::Point3d unitDirection(totalTranslation.x / magnitude, totalTranslation.y / magnitude, totalTranslation.z / magnitude);

    // 计算点到原点的向量
    cv::Point3d vectorToPoint = cv::Point3d(2.0, 3.0, 4.0);

    // 计算投影距离
    double translationAxis = vectorToPoint.x / unitDirection.x;  //
    double relativeYAxis = vectorToPoint.y - (translationAxis * unitDirection.y);

    // std::cout << "Point to origin distance along direction: " << distance << std::endl;
}
// 重投影误差计算
double ReprojectionErrorCallback::calculate_Reprojection_Error(const std::vector<cv::Point2f>& imagePoints,
                                                               const std::vector<cv::Point3f>& objectPoints,
                                                               const cv::Mat& TCW,  // 相机到基座的变换矩阵
                                                               const cv::Mat& TWE,  // 相机到基座的变换矩阵
                                                               const cv::Mat& TEB,  // 标定板到末端执行器的变换矩阵
                                                               const cv::Mat& K, const cv::Mat& dist_coeffs) const {
    double error = 0.0;
    std::vector<cv::Point2f> projected_points;
    // 判断并转换 TCW 和 TEB 为 4x4 矩阵
    // std::cout << "TWE: " << TWE << std::endl;
    cv::Mat TCW_4x4, TEB_4x4;
    if (TCW.rows * TCW.cols >= 16) {
        // 如果 TCW 有至少16个元素， reshape 为 4x4 矩阵
        TCW_4x4 = TCW.clone().reshape(1, 4);  // 重塑为 4x4 矩阵
        // std::cout<<"TCW > 16"<<TCW.size()<<std::endl;
        if (TCW_4x4.cols != 4) {
            // 如果 reshape 后的列数不是 4，补充为单位矩阵
            std::cout << "TCW_4x4 cols != 4" << std::endl;
            // TCW.rowRange(0, 4).copyTo(TCW_4x4.rowRange(0, 4));  // 复制前 16 个元素到 4x4 矩阵
        }
    } else {
        std::cout << "TCW no 16" << std::endl;
    }

    if (TEB.rows * TEB.cols >= 16) {
        // 如果 TEB 有至少16个元素， reshape 为 4x4 矩阵
        TEB_4x4 = TEB.clone().reshape(1, 4);  // 重塑为 4x4 矩阵
        if (TEB_4x4.cols != 4) {
            // 如果 reshape 后的列数不是 4，补充为单位矩阵
            // TEB_4x4 = cv::Mat::eye(4, 4, CV_64F);
            std::cout << "TEB_4x4.cols != 4" << std::endl;
            // TEB.rowRange(0, 4).copyTo(TEB_4x4.rowRange(0, 4));  // 复制前 16 个元素到 4x4 矩阵
        }
    } else {
        std::cout << "TEB no 16" << std::endl;
    }

    // 打印 TCW 和 TEB 矩阵
    // qDebug()<<"qqqqqq";
    // std::cout << "TCW_4x4: " << TCW_4x4 << std::endl;
    // std::cout << "TEB_4x4: " << TEB_4x4 << std::endl;
    for (size_t i = 0; i < objectPoints.size(); ++i) {
        // 3D点
        if (std::isnan(objectPoints[i].x) || std::isnan(objectPoints[i].y) || std::isnan(objectPoints[i].z) || std::isinf(objectPoints[i].x) ||
            std::isinf(objectPoints[i].y) || std::isinf(objectPoints[i].z)) {
            std::cerr << "Warning: Invalid 3D point at index " << i << ": " << "x = " << objectPoints[i].x << ", y = " << objectPoints[i].y
                      << ", z = " << objectPoints[i].z << std::endl;
            // 可以选择抛出异常或者返回某种错误值
            continue;  // 或者其他错误处理方式
        }

        cv::Mat point3D = (cv::Mat_<double>(4, 1) << objectPoints[i].x, objectPoints[i].y, objectPoints[i].z, 1.0);
        // std::cout<<"point3D"<<point3D<<std::endl;
        //  直接左乘矩阵：从标定板坐标系到末端执行器坐标系
        cv::Mat point_gripper = TEB_4x4 * point3D;
        // 直接左乘矩阵：从末端执行器坐标系到基座坐标系
        if (TWE.empty()) {
            // 如果 TWE_4x4 为空，打印警告
            std::cerr << "Warning: TWE_4x4 is empty!" << std::endl;
        } else {
            // 如果 TWE_4x4 不为空，打印矩阵
            // std::cout << "TWE: " << TWE << std::endl;
        }
        cv::Mat point_base = TWE * point_gripper;

        // 直接左乘矩阵：从基座坐标系到相机坐标系
        cv::Mat point_camera = TCW_4x4 * point_base;
        // std::cout<<"point_camera"<<point_camera<<std::endl;
        // std::cout<<"point_camerarowRange"<<point_camera.rowRange(0, 3)<<std::endl;
        // 将3D点从基座坐标系转换到相机坐标系，并投影到2D图像平面
        cv::Mat projected_point;
        // 检查 `rowRange(0, 3)` 是否有效
        if (point_camera.rowRange(0, 3).rows == 3 && point_camera.rowRange(0, 3).cols == 1) {
            // std::cout << "point_camera rowRange(0, 3) is valid." << std::endl;
        } else {
            std::cerr << "point_camera rowRange(0, 3) is invalid!" << std::endl;
        }

        cv::projectPoints(point_camera.rowRange(0, 3), cv::Mat::zeros(3, 1, CV_64F), cv::Mat::zeros(3, 1, CV_64F), K, dist_coeffs, projected_point);

        projected_points.push_back(cv::Point2f(projected_point.at<double>(0, 0), projected_point.at<double>(0, 1)));

        // 计算重投影误差
        double dx = imagePoints[i].x - projected_points[i].x;
        double dy = imagePoints[i].y - projected_points[i].y;
        error += dx * dx + dy * dy;
    }

    return error;
}
void ReprojectionErrorCallback::enforceOrthogonality(cv::Mat& matrix) const {
    // 提取矩阵的前 3x3 部分
    cv::Mat R = matrix(cv::Rect(0, 0, 3, 3));

    // 使用奇异值分解(SVD)来强制矩阵正交
    cv::Mat U, S, Vt;
    cv::SVD::compute(R, S, U, Vt);  // SVD分解 R = U * S * Vt
    R = U * Vt;                     // 重构正交矩阵

    // 将修改后的 3x3 部分更新回 TCW 或 TEB
    R.copyTo(matrix(cv::Rect(0, 0, 3, 3)));
}
