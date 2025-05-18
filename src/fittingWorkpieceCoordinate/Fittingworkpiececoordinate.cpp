#include "FittingWorkpieceCoordinate.h"
std::array<cameraConfig, 3> cameraParameters; //相机参数数量
/**
 * @brief 构造函数，初始化时加载校准参数
 */
FittingWorkpieceCoordinate::FittingWorkpieceCoordinate(){
    loadCalibrationParameters("./data/config/workpiece_localization_calib.json");
}

/**
 * @brief 处理工件坐标拟合
 * @param objs 检测到的物体列表
 * @param imgNum 当前图像编号
 */
void FittingWorkpieceCoordinate::whenFittingWorkpieceCoordinate(std::vector<seg::Object> objs, int imgNum)
{
    std::vector<cv::Point2d> Pt2ds;
    std::vector<cv::Point3d> worldPoints;
    std::vector<int> validPixels;

    // 遍历每个 Object
    for (const auto& obj : objs) {

        // 计算矩形的中心坐标
        //cv::Point2d center = cv::Point(obj.rect.x + obj.rect.width / 2, obj.rect.y + obj.rect.height / 2);
        cv::Point2d point = cv::Point(obj.rect.x, obj.rect.y);//左上角坐标
        //std::cout<<"point"<<point<<std::endl;
        // 计算有效像素数量（boxMask 中值为 1 的像素）
        int validPixel = cv::countNonZero(obj.boxMask);  // 统计 boxMask 中值为 1 的像素数
        Pt2ds.push_back(point);
        validPixels.push_back(validPixel);
    }
    if (Pt2ds.empty()) {
        // 如果没有有效的点，输出日志
        emit appendFittingLog(QString("未在图像 %1 中找到有效点").arg(imgNum + 1));
        return;
    }
    worldPoints = pixel2WorldCoordPoint(Pt2ds, imgNum);//计算世界坐标系下的坐标
    for (size_t i = 0; i < objs.size(); i++) {
        ObjectInfo objInfo;
        objInfo.object = objs[i];
        objInfo.pt3d = worldPoints[i];
        objInfo.validPixel = validPixels[i];
        objInfo.cameraIndex = imgNum;
        allObjects.push_back(objInfo);
    }
}
/**
 * @brief 推理完成后的处理函数
 */
void FittingWorkpieceCoordinate::whenFinishInferrence()
{
    categorizedObjects.clear();
    categoryWorldCenters.clear();
    categoryWorldLeftTopCenters.clear();
    filteredWorldCenters.clear();       // 人工筛选后的工件中心点（世界坐标系）
    filteredWorldTopLeftPoints.clear(); // 人工筛选后的工件左上角点（世界坐标系）
    worldMaskImages.clear();
    workpieceROIs.clear();
    selectedWorkpieces.clear();

    categorizedObjects =classifyWorkpieces(allObjects,threshold);
    removeSmallCategories(categorizedObjects); //实时拍摄使用，站点拍摄意义不大

    categoryWorldCenters = calculateCategoryCenters(categorizedObjects);
    //categoryWorldLeftTopCenters在displayDetectedWorkpieces中存储。
    displayDetectedWorkpieces(categorizedObjects, cvImagesInferring, categoryWorldCenters);
    // for (const auto& Center3d : categoryCenters3d) {
    //     std::cout<<"每次结束给的坐标"<<Center3d<<std::endl;
    // }

    allObjects.clear();
}

std::vector<cv::Point3d> FittingWorkpieceCoordinate:: pixel2WorldCoordPoint(std::vector<cv::Point2d> &Pt2ds,
                                                                             int cameraNumber){
    std::vector<cv::Point3d> cameraPointsXYZ;
    Point2dto3d(cameraParameters[cameraNumber].globalPlane,
                cameraParameters[cameraNumber].cameraMatrix,
                cameraParameters[cameraNumber].distCoeffs,
                Pt2ds, cameraPointsXYZ); // 输出相机坐标系下在对应平面上的映射
    //std::cout<<"cameraPointsXYZ"<<cameraPointsXYZ<<std::endl;
    std::vector<cv::Point3d> worldPoints = transformCameraToBase(cameraPointsXYZ,
                                                                 cameraParameters[cameraNumber].extrinsicMatrix);
    //std::cout<<"cameraParameters[cameraNumber].extrinsicMatrix"<<cameraParameters[cameraNumber].extrinsicMatrix<<std::endl;
    return worldPoints;
}
/**
 * @brief 将2D点转换为3D点
 * @param plane 平面方程参数
 * @param cameraMatrix 相机内参矩阵
 * @param distCoeffs 畸变系数
 * @param Pt2ds 输入2D点集
 * @param Pt3ds 输出3D点集
 */
void FittingWorkpieceCoordinate::Point2dto3d(std::vector<double> plane, cv::Mat &cameraMatrix,
                                     cv::Mat &distCoeffs, std::vector<cv::Point2d> &Pt2ds, std::vector<cv::Point3d> &Pt3ds)
{
    //Q_UNUSED(distCoeffs)

    double A = -(plane[0]/plane[3]), B = -(plane[1]/plane[3]), C = -(plane[2]/plane[3]);
    std::vector<cv::Point2d> undistortedPts;
    // 使用cv::undistortPoints进行去畸变


    cv::undistortPoints(Pt2ds, undistortedPts, cameraMatrix, distCoeffs);

    for (int i = 0; i < undistortedPts.size(); ++i) {
        double x1 = undistortedPts[i].x, y1 = undistortedPts[i].y;
        cv::Point3d pt;
        pt.z = (1 / (A * x1 + B * y1 + C));
        pt.x = x1 * pt.z;
        pt.y = y1 * pt.z;
        Pt3ds.push_back(pt);

    }
    // for (const auto &a :Pt3ds ){
    //     std::cout<<"Point2dto3d Camera:"<<a<<std::endl;
    // }
}

std::vector<cv::Point3d> FittingWorkpieceCoordinate::transformCameraToBase(
    const std::vector<cv::Point3d>& cameraPoints,
    const cv::Mat& transformationMatrix)
{
    std::vector<cv::Point3d> basePoints;

    // 确保变换矩阵是 4x4 的矩阵
    if (transformationMatrix.rows != 4 || transformationMatrix.cols != 4) {
        std::cerr << "Error: Transformation matrix must be 4x4!" << std::endl;
        return basePoints;
    }

    // 对每个相机坐标系下的点进行转换
    for (const auto& cameraPoint : cameraPoints) {
        // 将相机点转换为 4x1 向量，使用齐次坐标表示
        cv::Mat cameraPointMat = (cv::Mat_<double>(4, 1) << cameraPoint.x, cameraPoint.y, cameraPoint.z, 1);

        // 使用 4x4 变换矩阵进行转换
        cv::Mat transformedPointMat = transformationMatrix * cameraPointMat;

        // 提取变换后的 3D 点 (x, y, z)，忽略齐次坐标的 w 分量
        cv::Point3d basePoint(
            transformedPointMat.at<double>(0),
            transformedPointMat.at<double>(1),
            transformedPointMat.at<double>(2)
            );

        // 将变换后的点添加到结果中
        basePoints.push_back(basePoint);
    }

    return basePoints;
}
void FittingWorkpieceCoordinate::saveAllObjectsToFile(std::string filePath)
{
    std::ofstream outputFile(filePath, std::ios::trunc);  // 覆盖模式trunc打开文件，追加模式为app
    if (outputFile.is_open()) {
        outputFile << "Classification Results:\n\n";

        for (size_t i = 0; i < categorizedObjects.size(); ++i) {
            outputFile << "Category " << i + 1 << ":\n";
            for (size_t j = 0; j < categorizedObjects[i].size(); ++j) {
                outputFile <<"ID:"<<j<<" x "<<categorizedObjects[i][j].pt3d.x
                           <<"y "<<categorizedObjects[i][j].pt3d.y
                           <<"z "<<categorizedObjects[i][j].pt3d.z
                           <<"Image X:"<<categorizedObjects[i][j].object.rect.x
                           <<"Image y:"<<categorizedObjects[i][j].object.rect.y;

                // 确保 validPixels 长度匹配
                outputFile << "Valid Pixels: " << categorizedObjects[i][j].validPixel << std::endl;
            }
        }
        outputFile.close();
    } else {
        std::cerr << "Failed to open file for writing!" << std::endl;
    }
}
// 计算两点之间的欧几里得距离
double FittingWorkpieceCoordinate::calculateDistance(const cv::Point3d &p1, const cv::Point3d &p2)
{
    return std::sqrt(std::pow(p1.x - p2.x, 2) +
                     std::pow(p1.y - p2.y, 2) +
                     std::pow(p1.z - p2.z, 2));
}
// 计算类别的平均坐标（质心）
cv::Point3d FittingWorkpieceCoordinate::computeCentroid(const std::vector<ObjectInfo> &group)
{
    double sumX = 0, sumY = 0, sumZ = 0;
    for (const auto& obj : group) {
        sumX += obj.pt3d.x;
        sumY += obj.pt3d.y;
        sumZ += obj.pt3d.z;
    }
    int n = group.size();
    return cv::Point3d(sumX / n, sumY / n, sumZ / n);
}
// 进行工件分类
std::vector<std::vector<FittingWorkpieceCoordinate::ObjectInfo> >
FittingWorkpieceCoordinate::classifyWorkpieces(const std::vector<ObjectInfo> &allObjects, double threshold)
{
    std::vector<cv::Point3d> categoryLeftPts; // 记录每个类别的左上角代表点（动态更新）
    for (const auto& obj : allObjects) {
        bool foundGroup = false;

        // 只和每个类别的代表点比较
        for (size_t i = 0; i < categoryLeftPts.size(); ++i) {
            if (calculateDistance(obj.pt3d, categoryLeftPts[i]) < threshold) {
                categorizedObjects[i].push_back(obj);  // 归入该类别
                // 计算新的类别中心点
                categoryLeftPts[i] = computeCentroid(categorizedObjects[i]);
                foundGroup = true;
                break;
            }
        }
        // 如果没有找到合适的类别，创建新类别
        if (!foundGroup) {
            categorizedObjects.push_back({obj});
            categoryLeftPts.push_back(obj.pt3d);
        }
    }
    // std::string filePath ="./data/result/FittingWorkpieceCoordinate/classified_workpieces.txt";
    // saveAllObjectsToFile(filePath);
    return categorizedObjects;
}
void FittingWorkpieceCoordinate::removeSmallCategories(std::vector<std::vector<ObjectInfo>>& categorizedObjects) {
    // 使用 std::remove_if 和 erase 移除大小小于 一定数量的 的类别
    categorizedObjects.erase(
        std::remove_if(categorizedObjects.begin(), categorizedObjects.end(),
                       [](const std::vector<ObjectInfo>& category) {
                           return category.size() < 1; //“1”为一定数量，此处为站点拍摄，取1无实际价值
                       }),
        categorizedObjects.end()
        );
    std::string filePath ="./data/result/FittingWorkpieceCoordinate/classified_workpiecesRemove.txt";
    saveAllObjectsToFile(filePath);
}
std::vector<cv::Point3d> FittingWorkpieceCoordinate::calculateCategoryCenters(std::vector<std::vector<ObjectInfo>>& categorizedObjects) {

    for (auto& category : categorizedObjects) {
        // 按 validPixel 从大到小排序，选出 validPixel 最多的三项
        std::sort(category.begin(), category.end(), [](const ObjectInfo& a, const ObjectInfo& b) {
            return a.validPixel > b.validPixel;  // 降序排列
        });
        // std::string filePath ="./data/result/FittingWorkpieceCoordinate/object_testWorldPoints2.txt";
        // saveAllObjectsToFile(filePath);
        // 选择 validPixel 最多的前三项
        int count = std::min(maxPixelCount, (int)category.size());  // 至少选择 1 项，但不超过 3 项
        cv::Point3d sumCenter(0, 0, 0);  // 用来累加平均中心坐标

        for (int i = 0; i < count; ++i) {
            // 计算每个 Object 对应的矩形框的中心点
            cv::Rect rect = category[i].object.rect;
            //std::cout<<"rect:"<<rect<<std::endl;
            //cv::Point2d center(rect.x , rect.y);
            cv::Point2d center(rect.x + rect.width / 2.0, rect.y + rect.height / 2.0);
            //std::cout<<"beforecenter: "<<rect.x<<", "<<rect.y<<std::endl;
            //std::cout<<"center: "<<center<<std::endl;
            std::vector<cv::Point2d> categoryCenters={};//存储图像坐标系下的中心坐标
            categoryCenters.push_back(center);
            std::vector<cv::Point3d> finalWorldPoints = pixel2WorldCoordPoint(categoryCenters, category[i].cameraIndex);
            // std::cout<<"finalWorldPoints:"<<finalWorldPoints<<std::endl;
            sumCenter += finalWorldPoints[0];  // 累加
        }

        // 计算该类别的平均中心坐标
        cv::Point3d averageCenter = sumCenter / count;
        //std::cout<<"averageCenter:"<<averageCenter<<std::endl;

        categoryWorldCenters.push_back(averageCenter);  // 将结果加入类别中心坐标集合
    }
    //std::cout<<"test size "<<categoryCenters3d.size();

    // std::string filePath = "./data/result/FittingWorkpieceCoordinate/object_finalWorldPointsInAuto.txt";
    // std::ofstream outputFile(filePath, std::ios::trunc);  // 覆盖模式trunc打开文件，追加模式为app
    // if (outputFile.is_open()) {
    //     // 遍历 worldPoints
    //     for (size_t i = 0; i < categoryWorldCenters.size(); i++) {
    //         outputFile << "workpiece " << i + 1 << ": ";  // 工件编号从 1 开始
    //         outputFile << "categoryCenters3d: ("
    //                    << categoryWorldCenters[i].x/*+1300*/  << " , "
    //                    << categoryWorldCenters[i].y << " , "
    //                    << categoryWorldCenters[i].z << "), "
    //                    <<std::endl;
    //     }
    //     outputFile.close();  // 关闭文件
    //     std::cout << "工件信息已保存至 " << filePath << std::endl;
    // } else {
    //     std::cerr << "Failed to open file for writing!" << std::endl;
    // }
    // for (const auto& pt : finalWorldPoints) {
    //     std::cout <<"平面方程拟合"<< "(" << pt.x << ", " << pt.y << ", " << pt.z << ")\n";
    // }
    return categoryWorldCenters;
}
/**
 * @brief 加载校准参数
 * @param filename 参数文件路径
 */
void FittingWorkpieceCoordinate::loadCalibrationParameters(const std::string& filename) {
    cv::Mat cameraMatrixRead = cv::Mat::zeros(3, 3, CV_64F);  // 清零相机矩阵
    cv::Mat distCoeffsRead = cv::Mat::zeros(1, 5, CV_64F);    // 清零畸变系数
    std::vector<double> planeRead;                        // 清空全局平面参数
    cv::Mat extrinsicMatrixRead;
    int cameraNumber = 0;
    // 打开 JSON 文件
    std::ifstream file(filename);
    if (!file.is_open() || file.peek() == std::ifstream::traits_type::eof()) {
        // 文件为空或无法打开，初始化一个包含多个相机的默认值
        std::cerr << "File is empty or failed to open. Initializing default values." << std::endl;

        // 创建多个默认的相机配置
        std::map<std::string, MyMatrix> cameraConfigMap;

        // 假设有多个相机配置，名称为 Camera1, Camera2, Camera3
        for (int i = 1; i <= 3; ++i) {
            cv::Mat defaultCameraMatrix = cv::Mat::eye(3, 3, CV_64F);  // 默认相机矩阵
            cv::Mat defaultDistCoeffs = cv::Mat::zeros(1, 5, CV_64F);  // 默认畸变系数
            std::vector<double> defaultPlane = { 0.0, 0.0, 0.0 };       // 默认平面参数
            cv::Mat defaultExtrinsicMatrix = cv::Mat::eye(4, 4, CV_64F); // 默认外参矩阵

            MyMatrix defaultMatrix(defaultCameraMatrix, defaultDistCoeffs, defaultPlane, defaultExtrinsicMatrix);
            cameraConfigMap["Camera" + std::to_string(i)] = defaultMatrix;  // 使用 Camera1, Camera2, Camera3 为键
        }

        // 保存默认相机配置到文件
        std::ofstream outFile(filename);
        cereal::JSONOutputArchive outputArchive(outFile);
        outputArchive(cereal::make_nvp("Cameras", cameraConfigMap));

        std::cerr << "Default values written to file." << std::endl;
        return;
    }

    // 创建 Cereal JSON 输入归档
    cereal::JSONInputArchive inputArchive(file);

    try {
        // 反序列化所有相机的数据，假设每个相机的名称是 Camera1, Camera2, 等等
        std::map<std::string, MyMatrix> cameraConfigMap;
        inputArchive(cereal::make_nvp("Cameras", cameraConfigMap));

        // 遍历所有相机并输出其参数
        for (const auto& cameraConfigPara : cameraConfigMap) {
            std::string cameraName = cameraConfigPara.first;
            MyMatrix cameraData = cameraConfigPara.second;

            // 将反序列化后的数据传递到对应的变量
            cameraData.transferData(cameraMatrixRead, distCoeffsRead, planeRead, extrinsicMatrixRead);
            cameraParameters[cameraNumber].cameraMatrix = cameraMatrixRead;
            cameraParameters[cameraNumber].distCoeffs = distCoeffsRead;
            cameraParameters[cameraNumber].globalPlane = planeRead;
            cameraParameters[cameraNumber].extrinsicMatrix = extrinsicMatrixRead;
            cameraNumber++;
        }
        // cameraMatrix = cameraParameters[0].cameraMatrix;
        // distCoeffs = cameraParameters[0].distCoeffs;
        // plane = cameraParameters[0].globalPlane;
        // extrinsicMatrix = cameraParameters[0].extrinsicMatrix;
        // 打印相机的参数
        // std::cout << "Calibration for cameraName" << 0 << ":\n";
        // std::cout << "Camera Matrix:\n" << cameraMatrix << std::endl;
        // std::cout << "Distortion Coefficients:\n" << distCoeffs << std::endl;
        // std::cout << "ExtrinsicMatrix:\n" << extrinsicMatrix << std::endl;
        // std::cout << "Global Plane Parameters: ";
        // for (const auto& p : plane) {
        //     std::cout << p << " ";
        // }
        // std::cout << std::endl;

    } catch (const cereal::Exception& e) {
        std::cerr << "Error reading calibration parameters: " << e.what() << std::endl;
        return;
    }

    // 可以记录调试信息
    QString message = "Complete to load data/config/workpiece_localization_calib.json";
    emit appendFittingLog(message);
}
void FittingWorkpieceCoordinate::drawGridAndAxes(cv::Mat& railMap)
{
    const int pixelRow = railMap.rows;
    const int pixelCol = railMap.cols;

    // 设置 canvasMat 中定义的原点位置
    const int originX = pixelCol - 100;
    const int originY = 500;

    const int gridSpacingX = 100;//间距
    const int gridSpacingY = 100;

    const cv::Scalar axisColor(0, 0, 0);
    const int axisThickness = 2;

    // 绘制 X 轴（水平线）
    cv::line(railMap, cv::Point(0, originY), cv::Point(pixelCol, originY), axisColor, axisThickness);
    // 绘制 Y 轴（竖直线，注意 y 轴反向）
    cv::line(railMap, cv::Point(originX, 0), cv::Point(originX, pixelRow), axisColor, axisThickness);

    // 绘制竖直网格线（x方向）
    for (int x = originX % gridSpacingX; x < pixelCol; x += gridSpacingX) {
        cv::line(railMap, cv::Point(x, 0), cv::Point(x, pixelRow), cv::Scalar(200, 200, 200), 1);
    }

    // 绘制水平网格线（y方向反转）
    for (int y = originY % gridSpacingY; y < pixelRow; y += gridSpacingY) {
        cv::line(railMap, cv::Point(0, y), cv::Point(pixelCol, y), cv::Scalar(200, 200, 200), 1);
    }

    // 添加坐标标签
    for (int x = originX % gridSpacingX; x < pixelCol; x += gridSpacingX) {
        int label = x - originX;
        cv::putText(railMap, std::to_string(label), cv::Point(x + 2, originY - 5),
                    cv::FONT_HERSHEY_SIMPLEX, 0.4, axisColor, 1);
    }

    for (int y = originY % gridSpacingY; y < pixelRow; y += gridSpacingY) {
        int label = originY - y;  // 注意 y 轴是反的
        cv::putText(railMap, std::to_string(label), cv::Point(originX + 5, y),
                    cv::FONT_HERSHEY_SIMPLEX, 0.4, axisColor, 1);
    }
}
// 绘制工件图像
void FittingWorkpieceCoordinate::drawDetectedWorkpieces(cv::Mat& railMap, const cv::Mat& resizedImage,
                                                        cv::Point3d& worldCenter,
                                                        int categoryIdx)
{

    const cv::Scalar correctColor(0, 255, 0);
    const cv::Scalar warningColor(255, 255, 125); // 警告颜色
    const cv::Scalar deleteColor(255, 0, 0); // 删除颜色
    const int correctLineThickness = 3; // 正常线厚度
    const int warningLineThickness = 3; // 警告线厚度
    const int deleteLineThickness = 3; // 删除线厚度
    //1300
    cv::Mat centerPoint = (cv::Mat_<double>(3, 1) << worldCenter.x, worldCenter.y, 1);
    cv::Mat centerResult = canvasMat * centerPoint;
    double railMapX = centerResult.at<double>(0, 0);
    double railMapY = centerResult.at<double>(1, 0);
    // 确保抠出来的图像中心与 worldCenter 对齐
    int offsetX = static_cast<int>(railMapX - resizedImage.cols / 2);
    int offsetY = static_cast<int>(railMapY - resizedImage.rows / 2);
    cv::Rect roi;  // 定义一个空的矩形

    // 适配到画布
    if (offsetX >= 0 && offsetX + resizedImage.cols < railMap.cols &&
        offsetY >= 0 && offsetY + resizedImage.rows < railMap.rows) {
        roi = cv::Rect(offsetX, offsetY, resizedImage.cols, resizedImage.rows); // 正确设置 roi
        cv::rectangle(railMap, roi, correctColor, correctLineThickness);
        resizedImage.copyTo(railMap(roi));  // 将图像绘制到画布中
    } else {
        // 如果超出范围，调整到边界
        offsetX = std::max(0, std::min(offsetX, railMap.cols - resizedImage.cols));
        offsetY = std::max(0, std::min(offsetY, railMap.rows - resizedImage.rows));
        roi = cv::Rect(offsetX, offsetY, resizedImage.cols, resizedImage.rows); // 正确设置 roi
        resizedImage.copyTo(railMap(roi));

        // 绘制包裹工件的警告线
        cv::rectangle(railMap, roi, warningColor, warningLineThickness);
    }
    // 将 roi 和类别索引存入全局变量
    workpieceROIs.push_back(std::make_pair(roi, categoryIdx));
    // 标注工件中心点
    cv::circle(railMap, cv::Point(static_cast<int>(railMapX), static_cast<int>(railMapY)), 4, cv::Scalar(255, 0, 0), -1);
    std::string text = "(" + std::to_string(worldCenter.x) + ", " + std::to_string(worldCenter.y) + ", " + std::to_string(worldCenter.z) + ")";
    cv::Point textPosition = cv::Point(static_cast<int>(railMapX) - resizedImage.cols / 2, static_cast<int>(railMapY) + resizedImage.rows / 2 + 30);
    cv::putText(railMap, text, textPosition, cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 0, 0), 1);
    if (std::find(selectedWorkpieces.begin(), selectedWorkpieces.end(), categoryIdx) != selectedWorkpieces.end()) {
        cv::rectangle(railMap, roi, deleteColor, deleteLineThickness); // 红色边框
    }
}

void FittingWorkpieceCoordinate::displayDetectedWorkpieces(const std::vector<std::vector<ObjectInfo>>& categorizedObjects,
                                                           const std::vector<cv::Mat>& cvImagesToDisplay,
                                                           const std::vector<cv::Point3d>& categoryCenters3d)
{

    railMap = cv::Mat(pixelRow, pixelCol, CV_8UC3, cv::Scalar(255, 255, 255)); // 确保白色背景
    //1300

    // cv::imshow("Initial White Canvas", railMap); // 显示初始白色画布
    // cv::waitKey(0);

    // 获取图像路径列表
    // std::vector<std::string> imagePathList;
    // cv::glob(imagePath + "/*.bmp", imagePathList);  // 获取所有图像路径
    // 遍历所有类别
    for (size_t categoryIdx = 0; categoryIdx < categorizedObjects.size(); ++categoryIdx) {
        const auto& category = categorizedObjects[categoryIdx];
        if (category.empty()) continue;

        // 选取前个 pt3d 计算均值作为左上角坐标
        cv::Point3d worldTopLeft(0, 0, 0);
        int count = std::min(maxPixelCount, static_cast<int>(category.size()));
        for (int i = 0; i < count; ++i) {
            worldTopLeft += category[i].pt3d;
        }
        worldTopLeft *= (1.0 / count);

        //将左上角坐标存入类变量中
        categoryWorldLeftTopCenters.push_back(worldTopLeft);

        // 使用 categoryCenters3d 作为中心坐标
        cv::Point3d worldCenter = categoryCenters3d[categoryIdx];

        // 选取 validPixel 最大的对象
        const ObjectInfo& bestObject = category.front();
        int imagNum = bestObject.cameraIndex;

        if (imagNum >= 0 && imagNum < cvImagesToDisplay.size()) {
            cv::Mat image = cvImagesToDisplay[imagNum];

            cv::Rect rect = bestObject.object.rect;
            cv::Mat boxMask = bestObject.object.boxMask;//掩膜矩阵
            // 生成白色背景的掩膜图像
            cv::Mat maskRegion = cv::Mat(rect.size(), CV_8UC3, cv::Scalar(255, 255, 255)) ;//白色画布

            cv::Mat objectRegion = image(rect).clone();//原图rect区域
            objectRegion.copyTo(maskRegion, boxMask);//将原图rect区域中的掩膜部分复制给maskRegion
            // cv::imshow("Extracted Workpiece", maskRegion); // 显示掩膜下的工件图像（像素坐标系）
            // cv::waitKey(0);

            // // 计算缩放比例
            // double worldWidth = 2 * std::abs(worldCenter.x - worldTopLeft.x);
            // double worldHeight = 2 * std::abs(worldCenter.y - worldTopLeft.y);

            // 计算缩放后的大小
            // double scaleX = worldWidth / rect.width;
            // double scaleY = worldHeight / rect.height;
            // 计算世界坐标系中的对角线长度
            double worldDiagonal = std::sqrt(std::pow(worldCenter.x - worldTopLeft.x, 2) +
                                             std::pow(worldCenter.y - worldTopLeft.y, 2));
            // // 计算图像的对角线长度
            double imageDiagonal = std::sqrt(std::pow(rect.width, 2) + std::pow(rect.height, 2)) / 2.0;

            // 计算缩放比例
            double scaleX = worldDiagonal / imageDiagonal;
            double scaleY = worldDiagonal / imageDiagonal;
            cv::Size newSize(maskRegion.cols * scaleX, maskRegion.rows * scaleY);
            // 缩放图像
            cv::Mat resizedImage;
            //插值放缩掩膜
            //cv::resize(rotatedImage, resizedImage, newSize);//使用双线性插值
            //cv::resize(rotatedImage, resizedImage, newSize, 0, 0, cv::INTER_CUBIC);  // 使用立方插值
            cv::resize(maskRegion, resizedImage, newSize, 0, 0, cv::INTER_LANCZOS4);  // Lanczos


            // 计算原图 rect 的角度
            cv::Mat R = cameraParameters[imagNum].extrinsicMatrix(cv::Range(0, 3), cv::Range(0, 3));
            double angleDifference;
            // 计算偏航角（yaw）
            angleDifference = atan2(R.at<double>(1, 0), R.at<double>(0, 0));
            angleDifference = angleDifference * 180.0 / CV_PI;
            // angleDifference = 225;
            // 获取旋转矩阵
            cv::Point2f center(resizedImage.cols/2 , resizedImage.rows/2 ); // 图像的中心
            cv::Mat rotationMatrix = cv::getRotationMatrix2D(center, angleDifference, 1.0); // 获取旋转矩阵
            // 计算旋转后的图像尺寸
            cv::Rect bbox = cv::RotatedRect(center, resizedImage.size(), angleDifference).boundingRect();

            // 创建一个足够大的白色背景，以容纳旋转后的图像
            cv::Mat rotatedImage = cv::Mat(bbox.size(), CV_8UC3, cv::Scalar(255, 255, 255)); // 新背景
            rotationMatrix.at<double>(0, 2) += (bbox.width - resizedImage.cols) / 2;  // 水平平移
            rotationMatrix.at<double>(1, 2) += (bbox.height - resizedImage.rows) / 2;  // 垂直平移
            //https://blog.csdn.net/u013105205/article/details/78826789

            // 将旋转后的图像放入新的背景中，确保图像居中
            cv::warpAffine(resizedImage, rotatedImage, rotationMatrix, bbox.size(), cv::INTER_CUBIC,
                           cv::BORDER_CONSTANT, cv::Scalar(255, 255, 255)); // 旋转图像

            // // 显示旋转后的图像
            // cv::imshow("maskRegion Image", maskRegion);
            // cv::waitKey(0);
            // cv::imshow("resizedImage Image", resizedImage);
            // cv::waitKey(0);
            // cv::imshow("Rotated Image", rotatedImage);
            // cv::waitKey(0);

            worldMaskImages.push_back(rotatedImage);//世界坐标系下的掩模图下
            drawDetectedWorkpieces(railMap, rotatedImage, worldCenter, categoryIdx);
        }
    }
    /* for (size_t categoryIdx = 0; categoryIdx < workpieceROIs.size(); ++categoryIdx) {
    //     const auto& [rect, category] = workpieceROIs[categoryIdx];

    //     // 计算当前物体的中心坐标
    //     cv::Point2f center = (rect.br() + rect.tl()) * 0.5; // 中心点：矩形的左上角和右下角的平均

    //     // 获取当前物体的 worldCenter
    //     cv::Point3d worldCenter = categoryCenters3d[categoryIdx]; // 假设 categoryCenters3d 存储了每个类别的世界坐标

    //     // 存储前后类别的中心点
    //     cv::Point2f avgCenter = center; // 默认当前物体的中心点为平均中心点

    //     // 计算与前一个类别的物体的 IoU
    //     if (categoryIdx > 0) { // 如果当前类别不是第一个类别
    //         const auto& [prevRect, prevCategory] = workpieceROIs[categoryIdx - 1];
    //         float iou = computeIoU(rect, prevRect); // 计算当前物体与前一个物体的 IoU

    //         if (iou > 0.1) {
    //             // 如果 IoU 大于 0.1，则计算前后物体的中心点平均
    //             const cv::Point2f prevCenter = (prevRect.br() + prevRect.tl()) * 0.5;
    //             avgCenter = (center + prevCenter) * 0.5; // 计算平均中心点
    //             worldCenter = (categoryCenters3d[categoryIdx] + categoryCenters3d[categoryIdx - 1]) * 0.5; // 更新 worldCenter
    //         }
    //     }

    //     // 计算与后一个类别的物体的 IoU
    //     if (categoryIdx < workpieceROIs.size() - 1) { // 如果当前类别不是最后一个类别
    //         const auto& [nextRect, nextCategory] = workpieceROIs[categoryIdx + 1];
    //         float iou = computeIoU(rect, nextRect); // 计算当前物体与后一个物体的 IoU

    //         if (iou > 0.1) {
    //             // 如果 IoU 大于 0.1，则计算前后物体的中心点平均
    //             const cv::Point2f nextCenter = (nextRect.br() + nextRect.tl()) * 0.5;
    //             avgCenter = (avgCenter + nextCenter) * 0.5; // 再计算新的平均中心点
    //             worldCenter = (categoryCenters3d[categoryIdx] + categoryCenters3d[categoryIdx + 1]) * 0.5; // 更新 worldCenter
    //         }
    //     }

    //     // 绘制平均的中心点
    //     cv::circle(railMap, avgCenter, 4, cv::Scalar(255, 0, 0), -1); // 绘制红色圆点表示中心位置

    //     // 构建文本
    //     std::string text = "(" + std::to_string(worldCenter.x) + ", " + std::to_string(worldCenter.y) + ", " + std::to_string(worldCenter.z) + ")";
    //     cv::Point textPosition = cv::Point(static_cast<int>(avgCenter.x) - 30, static_cast<int>(avgCenter.y) + 30);
    //     cv::putText(railMap, text, textPosition, cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 0, 0), 1);
    // }*/
    drawGridAndAxes(railMap);
    emit sendWorkpieceResultToMainWindow(railMap);
    //emit sendWorkpieceMaskImageInWorld(categoryCenters3d,worldMaskImages); //发送为筛选前的工件在世界坐标系下的中心点和工件掩膜图像
    cv::Mat railMapRGB;
    cv::cvtColor(railMap, railMapRGB, cv::COLOR_BGR2RGB);
    cv::imwrite("./data/workpieceCoaLoc/FinalRailMap/detected_workpieces.jpg", railMapRGB);

    // 显示最终的画布
    //cv::imshow("Final Workpiece Map", railMap);
    //cv::waitKey(0);
}

void FittingWorkpieceCoordinate::handleClickEvent(int x, int y){
    // qDebug()<<"点击动作";
    for (const auto& roiPair : workpieceROIs) {
        const cv::Rect& roi = roiPair.first;
        int categoryIdx = roiPair.second;
        // // 打印当前 ROI 的信息
        // qDebug() << "ROI:"
        //          << "x =" << roi.x
        //          << ", y =" << roi.y
        //          << ", width =" << roi.width
        //          << ", height =" << roi.height
        //          << "-> categoryIdx =" << categoryIdx;

        // 判断点击坐标是否在工件的矩形区域内
        if (roi.contains(cv::Point(x, y))) {
            // qDebug() << "点击在该 ROI 内";
            // 如果点击了工件，标记它
            if (std::find(selectedWorkpieces.begin(), selectedWorkpieces.end(), categoryIdx) == selectedWorkpieces.end()) {
                selectedWorkpieces.push_back(categoryIdx);
            } else {
                selectedWorkpieces.erase(std::remove(selectedWorkpieces.begin(), selectedWorkpieces.end(), categoryIdx), selectedWorkpieces.end());
            }
            displayDetectedWorkpieces(categorizedObjects, cvImagesInferring, categoryWorldCenters);
            break;
        }
    }
}
void FittingWorkpieceCoordinate::whenVerifyWorkpieceCoordinates()
{
    filteredWorldCenters.clear();
    filteredWorldTopLeftPoints.clear();
    std::vector<cv::Mat>resultWorkpieceMasks;
    for (size_t i = 0; i < categorizedObjects.size(); ++i) {
        if (std::find(selectedWorkpieces.begin(), selectedWorkpieces.end(), i) == selectedWorkpieces.end()) {
            filteredWorldCenters.push_back(categoryWorldCenters[i]);
            filteredWorldTopLeftPoints.push_back(categoryWorldLeftTopCenters[i]);
            resultWorkpieceMasks.push_back(worldMaskImages[i]);
        }
    }
    emit sendFinalInfoToMain(filteredWorldCenters,filteredWorldTopLeftPoints);
    emit sendWorkpieceMaskImageInWorld(filteredWorldCenters,resultWorkpieceMasks);//发送筛选后的

    // std::string filePath = "./data/result/FittingWorkpieceCoordinate/object_finalWorldPointsInManual.txt";
    // std::ofstream outputFile(filePath, std::ios::trunc);  // 覆盖模式trunc打开文件，追加模式为app
    // if (outputFile.is_open()) {
    //     // 遍历 worldPoints
    //     for (size_t i = 0; i < resultCenters.size(); i++) {
    //         outputFile << "workpiece " << i + 1 << ": ";  // 工件编号从 1 开始
    //         outputFile << "resultCenters: ("
    //                    << resultCenters[i].x  << " , "
    //                    << resultCenters[i].y << " , "
    //                    << resultCenters[i].z << "), "
    //                    <<std::endl;
    //     }
    //     outputFile.close();  // 关闭文件
    //     std::cout << "saved to " << filePath << std::endl;
    // } else {
    //     std::cerr << "Failed to open file for writing!" << std::endl;
    // }
}
void FittingWorkpieceCoordinate::whenDisplayWeldSeamArea(const std::vector<std::vector<std::array<double, 4>>> &boxInfos){

    for (size_t i = 0; i < boxInfos.size(); ++i) {

        cv::Mat centerPointOrigin = (cv::Mat_<double>(3, 1) << filteredWorldCenters[i].x, filteredWorldCenters[i].y, 1);
        cv::Mat resultCenter = canvasMat * centerPointOrigin;

        double centerPointX = resultCenter.at<double>(0, 0);
        double centerPointY = resultCenter.at<double>(1, 0);
        for (size_t j = 0; j < boxInfos[i].size(); ++j) {
            // 获取每个方框的信息
            const std::array<double, 4>& box = boxInfos[i][j];//中心和宽高
            double offsetX = box[0]; // 焊缝区域中心点
            double offsetY = box[1];
            double width = box[2];
            double height = box[3];

            // offsetX +=1300;
            double relativeX = offsetX - filteredWorldCenters[i].x;
            double relativeY = offsetY - filteredWorldCenters[i].y;
            double railMapX = relativeX+ centerPointX;
            double railMapY = relativeY+ centerPointY;


            // 计算矩形框的左上角和右下角
            cv::Point topLeft(static_cast<int>(railMapX - width / 2), static_cast<int>(railMapY - height / 2));
            cv::Point bottomRight(static_cast<int>(railMapX + width / 2), static_cast<int>(railMapY + height / 2));

            // 绘制矩形框到 railMap 上
            cv::rectangle(railMap, topLeft, bottomRight, cv::Scalar(0, 255, 0), 2);  // 绿色框，厚度为2

            // 可以添加额外的处理，比如绘制中心点
            cv::Point center(static_cast<int>(railMapX), static_cast<int>(railMapY));
            cv::circle(railMap, center, 5, cv::Scalar(0, 0, 255), -1);  // 绘制中心点，蓝色圆圈
        }
    }
    // 发送更新后的 railMap 到主窗口
    emit sendWorkpieceResultToMainWindow(railMap);
    cv::Mat railMapRGB;
    cv::cvtColor(railMap, railMapRGB, cv::COLOR_BGR2RGB);
    cv::imwrite("./data/workpieceCoaLoc/FinalRailMap/detected_weldSeam.jpg", railMapRGB);
}
float FittingWorkpieceCoordinate::computeIoU(const cv::Rect_<float>& rect1, const cv::Rect_<float>& rect2) {
    // 计算交集
    cv::Rect_<float> intersection = rect1 & rect2;
    // 计算并集
    float unionArea = rect1.area() + rect2.area() - intersection.area();

    if (unionArea == 0) return 0.0f; // 防止除零错误
    return intersection.area() / unionArea;
}
