#include "LaserControl.h"

// 初始化静态变量
int LaserControl::currentDataMode = -1;

LaserControl::LaserControl() {}

LaserControl::~LaserControl() { this->closeCamera(); }

void LaserControl::openCamera() {
    PLOGD << L"正在连接镭烁相机... ...";
    char ptxtTemp[] = "../3rdparty/HD6/SN6-0020WR-1195611";

    // HD系列初始化
    int ret = LSHD6_InitialCameraWithoutUI(LASER_NUMBER, ptxtTemp, exposureTime, 30.0, true, 0, false, false, 1, "192.168.5.11");
    if (ret == 0) {
        int left, top, width, height;
        left = top = width = height = 0;
        LSHD6_GetCameraROI(LASER_NUMBER, left, top, width, height);  // 获取当前ROI
        PLOGD << L"当前ROI：" << left << "," << top << "," << width << "," << height;
        LSHD6_SetSingleCallBack(LASER_NUMBER, singleCallBack, this);  // 设置单轮廓回调函数
        this->laserLaserLineOn();                                     // 开启激光线
        //        this->laserSetProfileMode();                                  // 默认轮廓模式
        this->laserSetOriginImageMode();  // 默认原始图模式
        this->laserStartGetData();        // 开启后直接开始采图
        PLOGD << L"镭烁相机连接成功";

        /*
        //        // 验证哪些点可以计算三维点轮廓坐标（结果：行号 191~718 之间）
        //        cv::Mat image = cv::Mat::zeros(1024, 1280, CV_8UC1);
        //        for (int i = 0; i < 1280; ++i) {
        //            for (int j = 0; j < 1024; ++j) {
        //            double profX, profZ;
        //            LSHD6_GetProfileFromSinglePixel(LASER_NUMBER, i, j, profX, profZ);
        //                if (profX > -90 && profZ > -90) {
        //                    qDebug() << "行号：" << j << "列号：" << i << profX << profZ;
        //                    image.at<uchar>(j, i) = 255;
        //                }
        //            }
        //        }
        //        cv::imwrite("../testImage.bmp", image);
        */

        emit sendCameraState(MY_COLOR::GREEN);
    } else {
        PLOGE << L"镭烁相机连接失败，错误代码: " << ret;
        emit sendCameraState(MY_COLOR::RED);
    }
}

/**
 * @brief MainWindow::singleCallBack 单个轮廓或原始图回调函数
 * @param profileX 单个轮廓的X轴数据指针
 * @param profileZ 单个轮廓的Z轴数据指针
 * @param count 单个轮廓的点数
 * @param pImage 原始图像数据指针（8byte 灰度图像）
 * @param imgWidth 原始图像宽度（单位：像素）
 * @param imgHeight 原始图像高度（单位：像素）
 * @param timeStamp 时间戳（单位毫秒）
 * @param userDefine C++专用，用户自定义数据指针，可传入this方便调用自己class中函数
 */
void LaserControl::singleCallBack(double *profileX, double *profileZ, int count, BYTE *pImage, int imgWidth, int imgHeight,
                                  unsigned int timeStamp, void *userDefine) {
    Q_UNUSED(count)
    Q_UNUSED(timeStamp)
    LaserControl *user = (LaserControl *)userDefine;  // 获取注册函数时的LaserControl对象

    if (currentDataMode == ORIGIN_IMAGE_MODE) {  // 如果获取的是原始图像，自行检测中心线，并转为轮廓数据
        // 自己计算轮廓数据
        cv::Mat cvImage(imgHeight, imgWidth, CV_8UC1, pImage);  // 获取到的原始图像
        cv::Mat cvImageWithCenterLine;                          // 绘制了中心线的图像
        QVector<QPointF> centerLineInImage;                     // 图像条纹中心线
        QVector<QPointF> profileData;                           // 空间轮廓数据
        double profX, profZ;
        user->normalCentroid.getCenterLine(cvImage, cvImageWithCenterLine, centerLineInImage);  // 检测条纹中心线

        // for (int i = 0; i < centerLineInImage.size(); ++i) {
        //     if (i % 5 == 0) {
        //         LSHD6_GetProfileFromSinglePixel(LASER_NUMBER, centerLineInImage[i].x(), centerLineInImage[i].y(), profX,
        //         profZ); if (profX > 0 && profZ > 0) {
        //             profileData.append(QPointF(profX, profZ));
        //         }
        //     }
        // }
        // 保存原始图像（如需）
        if (user->imageNumberToSave > 0) {
            user->saveImage(pImage, imgWidth, imgHeight);
        }
        if (centerLineSegResultInImageWithInliers.size() >= 1) {  // 绘制拟合出的直线
            user->drawFittedLineOnImage(cvImageWithCenterLine);
        }
        emit user->sendCVImageToMainWindow(cvImageWithCenterLine);  // 如果需要显示cv::Mat格式的图像，用这个信号传递
        // emit user->sendProfile(profileData);  // 如果需要显示轮廓，用这个信号
        emit user->sendCenterLine(centerLineInImage);  // 如果需要发送图像条纹中心线，用这个信号
    } else if (currentDataMode == PROFILE_MODE) {      // 如果获取的是点云数据，过滤无效点，并在主窗口显示
        QVector<QPointF> profileData;
        for (int i = 0; i < 200; ++i) {
            if (profileZ[i] > 0 && profileX[i] > 0) {
                profileData.append(QPointF(profileX[i], profileZ[i]));
            }
        }
        if (user->profileNumberToSave > 0) {
            user->saveProfile(profileData);
        } else if (user->calibProfileNumberToSave > 0) {
            user->saveProfileToCalib(profileData);
        }
        emit user->sendProfile(profileData);
    }
}

/**
 * @brief LaserControl::rawFittedLineOnImage 在图像上绘制拟合出来的直线
 * @param image 输入/输出图像
 */
void LaserControl::drawFittedLineOnImage(cv::Mat &image) {
    auto calculateX = [](const QPointF &point, const QPointF &direction, double y) -> double {
        // 根据直线上一点以及直线的方向，以及目标点的y坐标，计算目标点的x坐标
        return point.x() + (y - point.y()) * direction.x() / direction.y();
    };

    if (weldSeamType == WELD_SEAM_TYPE::PLANE_V_SHAPED) {  // 平面V型焊缝
        QPointF p1(centerLineSegResultInImageWithoutInliers[0][0], centerLineSegResultInImageWithoutInliers[0][2]);
        QPointF d1(centerLineSegResultInImageWithoutInliers[0][3], centerLineSegResultInImageWithoutInliers[0][5]);
        QPointF p2(centerLineSegResultInImageWithoutInliers[1][0], centerLineSegResultInImageWithoutInliers[1][2]);
        QPointF d2(centerLineSegResultInImageWithoutInliers[1][3], centerLineSegResultInImageWithoutInliers[1][5]);
        int x1_1 = calculateX(p1, d1, 0);
        int x1_2 = calculateX(p1, d1, image.rows);
        int x2_1 = calculateX(p2, d2, 0);
        int x2_2 = calculateX(p2, d2, image.rows);
        cv::line(image, cv::Point(x1_1, 0), cv::Point(x1_2, image.rows), cv::Scalar(0, 255, 0), 2);  // 画线
        cv::line(image, cv::Point(x2_1, 0), cv::Point(x2_2, image.rows), cv::Scalar(0, 255, 0), 2);
        cv::line(image, cv::Point(featurePointInImage[0] - 20, featurePointInImage[2]),
                 cv::Point(featurePointInImage[0] + 20, featurePointInImage[2]), cv::Scalar(255, 0, 0), 3);  // 标记特征点
        cv::line(image, cv::Point(featurePointInImage[0], featurePointInImage[2] - 20),
                 cv::Point(featurePointInImage[0], featurePointInImage[2] + 20), cv::Scalar(255, 0, 0), 3);

    } else if (weldSeamType == WELD_SEAM_TYPE::PLANE_BUTT_JOINT) {  // 平面对接焊缝
        QPointF p(centerLineSegResultInImageWithInliers[0].first[0], centerLineSegResultInImageWithInliers[0].first[2]);
        QPointF d(centerLineSegResultInImageWithInliers[0].first[3], centerLineSegResultInImageWithInliers[0].first[5]);
        int x1 = calculateX(p, d, 0);
        int x2 = calculateX(p, d, image.rows);
        cv::line(image, cv::Point(x1, 0), cv::Point(x2, image.rows), cv::Scalar(0, 255, 0), 2);  // 画线
        cv::line(image, cv::Point(featurePointInImage[0] - 20, featurePointInImage[2]),
                 cv::Point(featurePointInImage[0] + 20, featurePointInImage[2]), cv::Scalar(255, 0, 0), 3);  // 标记特征点
        cv::line(image, cv::Point(featurePointInImage[0], featurePointInImage[2] - 20),
                 cv::Point(featurePointInImage[0], featurePointInImage[2] + 20), cv::Scalar(255, 0, 0), 3);

    } else if (weldSeamType == WELD_SEAM_TYPE::PLANE_CORNER_JOINT) {
        // 平面角接焊缝

    } else if (weldSeamType == WELD_SEAM_TYPE::PLANE_LAP_JOINT) {
        // 平面搭接焊缝
    }
}

/**
 * @brief LaserControl::saveImage 保存图像
 * @param pImage 图像数据指针
 * @param imgWidth 图像宽度
 * @param imgHeight 图像高度
 */
void LaserControl::saveImage(BYTE *pImage, int imgWidth, int imgHeight) {
    PLOGD << L"正在保存图像... ...";
    cv::Mat cvImage = cv::Mat(imgHeight, imgWidth, CV_8UC1, (uint8_t *)pImage);
    cv::imwrite("../data/image/" + std::to_string(this->savedImages) + ".bmp", cvImage);
    PLOGD << L"保存成功";

    this->savedImages++;
    this->imageNumberToSave--;
}

/**
 * @brief LaserControl::saveProfileToPath 保存轮廓为点云数据
 * @param profileData 轮廓数据
 * @param path        路径以及文件名
 */
void LaserControl::saveProfileToPath(QVector<QPointF> &profileData, QString path) {
    // 打开文件以进行写入
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file for writing:" << path;
        return;
    }
    QTextStream out(&file);

    // 写入PCD文件头部
    out << "# .PCD v0.7 - Point Cloud Data file format\n";
    out << "VERSION 0.7\n";
    out << "FIELDS x y z\n";
    out << "SIZE 4 4 4\n";
    out << "TYPE F F F\n";
    out << "COUNT 1 1 1\n";
    out << "WIDTH " << profileData.size() << "\n";
    out << "HEIGHT 1\n";
    out << "VIEWPOINT 0 0 0 1 0 0 0\n";
    out << "POINTS " << profileData.size() << "\n";
    out << "DATA ascii\n";
    // 写入点数据
    for (const QPointF &point : profileData) {
        out << point.x() << " 0 " << point.y() << "\n";
    }
    file.close();

    if (file.error() == QFile::NoError) {
        PLOGD << "轮廓数据保存成功:" << path.toStdString();
    } else {
        PLOGE << "轮廓数据保存失败:" << file.errorString().toStdString();
    }
}

/**
 * @brief LaserControl::saveProfile 保存轮廓以及逻辑处理
 * @param profileData 轮廓数据
 */
void LaserControl::saveProfile(QVector<QPointF> profileData) {
    PLOGD << L"正在保存轮廓数据... ...";

    saveProfileToPath(profileData, "../data/cloud/" + QString::number(this->savedProfiles) + ".pcd");

    this->savedProfiles++;
    this->profileNumberToSave--;
}

/**
 * @brief LaserControl::saveProfileToCalib 保存用于标定的轮廓数据
 * @param profileData 轮廓数据
 */
void LaserControl::saveProfileToCalib(QVector<QPointF> profileData) {
    PLOGD << L"正在保存用于标定的轮廓数据... ...";

    saveProfileToPath(profileData, calibFilePath);

    this->savedCalibProfiles++;
    this->calibProfileNumberToSave--;
}

void LaserControl::laserStartGetData() {
    bool ret = LSHD6_LowSpeedGrab(LASER_NUMBER, 30);
    if (ret == true) {
        PLOGD << L"镭烁相机开始采图成功";
    } else {
        PLOGE << L"镭烁相机开始采图失败";
    }
}

void LaserControl::laserStopGetData() {
    bool ret = LSHD6_Stop(LASER_NUMBER);
    if (ret == true) {
        PLOGD << L"镭烁相机停止采图成功";
    } else {
        PLOGE << L"镭烁相机停止采图失败";
    }
}

void LaserControl::laserLaserLineOn() {
    bool ret = LSHD6_SetLaserOn(LASER_NUMBER);
    if (ret == true) {
        PLOGD << L"镭烁相机激光器打开成功";
    } else {
        PLOGE << L"镭烁相机激光器打开失败";
    }
}

void LaserControl::laserLaserLineOff() {
    bool ret = LSHD6_SetLaserOff(LASER_NUMBER);
    if (ret == true) {
        PLOGD << L"镭烁相机激光器关闭成功";
    } else {
        PLOGE << L"镭烁相机激光器关闭失败";
    }
}

void LaserControl::laserSetOriginImageMode() {
    this->laserStopGetData();

    bool ret = LSHD6_SetSingleCallBackMode(LASER_NUMBER, ORIGIN_IMAGE_MODE);
    if (ret == true) {
        PLOGD << L"镭烁相机设置原始图模式成功";
        currentDataMode = ORIGIN_IMAGE_MODE;
    } else {
        PLOGE << L"镭烁相机设置原始图模式失败";
    }

    this->laserStartGetData();
}

void LaserControl::laserSetProfileMode() {
    this->laserStopGetData();

    bool ret = LSHD6_SetSingleCallBackMode(LASER_NUMBER, PROFILE_MODE);
    if (ret == true) {
        PLOGD << L"镭烁相机设置轮廓模式成功";
        currentDataMode = PROFILE_MODE;
    } else {
        PLOGE << L"镭烁相机设置轮廓模式失败";
    }

    this->laserStartGetData();
}

void LaserControl::laserSetExposureTime() {
    this->laserStopGetData();

    bool ret = LSHD6_SetExposureTime(LASER_NUMBER, exposureTime);
    if (ret == true) {
        PLOGD << L"镭烁相机设置曝光时间成功";
    } else {
        PLOGE << L"镭烁相机设置曝光时间失败";
    }

    this->laserStartGetData();
}

void LaserControl::closeCamera() {
    this->laserLaserLineOff();

    bool ret = LSHD6_DestroyCamera(LASER_NUMBER);
    if (ret == true) {
        PLOGD << L"镭烁相机销毁成功";
        emit sendCameraState(MY_COLOR::RED);
    } else {
        PLOGE << L"镭烁相机销毁失败";
        emit sendCameraState(MY_COLOR::GRAY);
    }
}

void LaserControl::whenExposureTimeRenew(double exposureTime) {
    this->exposureTime = exposureTime;
    this->laserSetExposureTime();
}

void LaserControl::whenNeedToSaveImage(int number) {
    PLOGD << L"收到需要保存的图像数量: " << number;

    if (currentDataMode == ORIGIN_IMAGE_MODE) {
        this->imageNumberToSave = number;
    } else if (currentDataMode == PROFILE_MODE) {
        this->profileNumberToSave = number;
    }
}

void LaserControl::whenNeedToSaveCalibData(int calibDataType, QString filePath) {
    if (calibDataType == MY_CALIB_DATA_TYPE::POSITION) {
        PLOGD << L"相机收到位置保存指令，无需处理";
        return;
    } else if (calibDataType == MY_CALIB_DATA_TYPE::POSITION_POSTURE) {
        PLOGD << L"相机收到位姿保存指令，开始保存TCP在当前位姿下的传感器点云... ...";

        this->calibFilePath = filePath;
        this->calibProfileNumberToSave = 1;
    } else {
        PLOGE << L"相机收到未知标定数据保存指令，不予处理";
        return;
    }
}
