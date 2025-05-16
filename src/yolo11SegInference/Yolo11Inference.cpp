#include "yolo11SegInference/Yolo11Inference.h"
std::vector<cv::Mat> cvImagesInferring;
Yolo11SegInference::Yolo11SegInference() {
    cudaSetDevice(0);
    yolo11_seg->make_pipe(true);

}

Yolo11SegInference::~Yolo11SegInference()
{

    if (yolo11_seg) {
        delete yolo11_seg;
        yolo11_seg = nullptr;
    }
}



void Yolo11SegInference::whenPathNeedToInfer(std::string path) {
    imgNum = 0;
    detectedWp = 0;
    std::vector<cv::Mat> cvImages;
    cv::glob(path + "/*.jpg", imagePathList);

    for (auto &path : imagePathList) {
        image = cv::imread(path);
        cvImages.push_back(image);
        inferSegAndCalcTime();  // 推理并计算结果
        emit sendInferResultToMainWindow(res);
        cv::imwrite("./data/result/result" + std::to_string(imgNum++) + ".bmp", res);
    }

    cv::glob(path + "/*.bmp", imagePathList);

    for (auto &path : imagePathList) {
        image = cv::imread(path);
        cvImages.push_back(image);
        inferSegAndCalcTime();  // 推理并计算结果
        emit sendInferResultToMainWindow(res);
        cv::imwrite("./data/result/result" + std::to_string(imgNum++) + ".bmp", res);
    }
    cvImagesInferring = cvImages;
    emit sendAppendInferLog(QString("共检测工件数量:%1").
                            arg(detectedWp));
    emit sendSignalTocalculate();
}

void Yolo11SegInference::whenImageNeedToInfer(std::vector<cv::Mat> cvImages) {
    imgNum = 0;
    detectedWp = 0;
    // 对传入的 cvImages 进行深拷贝，防止后续操作影响到原始图像
    cvImagesInferring.clear();
    for (const auto& cvimage : cvImages) {
        cvImagesInferring.push_back(cvimage.clone());  // 深拷贝
    }

    for (auto &cvimage : cvImagesInferring) {
        image = cvimage;
        inferSegAndCalcTime();
        emit sendInferResultToMainWindow(res);
        whenImageNeedToSave(cvimage,res);
    }
    emit sendAppendInferLog(QString("共检测工件数量:%1").
                            arg(detectedWp));
    emit sendSignalTocalculate();
}
void Yolo11SegInference::whenImageNeedToSave(cv::Mat cvimage, cv::Mat cvImagesInference){
    std::time_t now = std::time(nullptr);
    std::tm *localTime = std::localtime(&now);
    std::ostringstream dateTimeStream;
    dateTimeStream << std::put_time(localTime, "%Y%m%d_%H%M%S");
    // cv::imwrite("./data/workpieceCoaLoc/camera_" +std::to_string(imgNum) +"_"+ dateTimeStream.str() + ".bmp", cvimage);
    cv::imwrite("./data/workpieceCoaLoc/infer/camera_" +std::to_string(imgNum++) +"_"+ dateTimeStream.str() +"_result.bmp", cvImagesInference);
    // cv::imshow("Chessboard Image with Subpixel Corners", cvImagesInference);  // 显示处理后的图像
    // cv::waitKey(0);  // 防止采图卡顿
    std::cout << "camera"+std::to_string(imgNum)+" Save image in workpieceCoaLoc succ." << std::endl;
}

void Yolo11SegInference::inferSegAndCalcTime() {
    objs.clear();
    yolo11_seg->copy_from_Mat(image, size);
    auto start = std::chrono::system_clock::now();
    yolo11_seg->infer();
    auto end = std::chrono::system_clock::now();
    yolo11_seg->postprocess(objs, score_thres, iou_thres, topk, num_channels, seg_h, seg_w);
    yolo11_seg->draw_objects(image, res, objs, CLASS_NAMES, COLORS, MASK_COLORS);
    inferTime = (double)std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000.;
    emit sendAppendInferLog(QString("相机%1检测工件数量:%2").arg(imgNum+1).
                            arg(objs.size()));
    detectedWp += objs.size();
    this->mask = yolo11_seg->maskOnly.clone();
    for (auto &obj : objs) {
        colorizeAndDisplayConnectedComponents(obj.boxMask);
    }
    emit sendCoordinateTofit (objs,imgNum);
}
void Yolo11SegInference::colorizeAndDisplayConnectedComponents(cv::Mat& mask) {
    // 确保掩膜是二值图像（0 或 255）
    cv::Mat binarizedMask;
    if (mask.channels() > 1) {
        cv::cvtColor(mask, binarizedMask, cv::COLOR_BGR2GRAY);  // 转为灰度图
    } else {
        binarizedMask = mask;
    }

    // 二值化掩膜（如果需要）
    cv::threshold(binarizedMask, binarizedMask, 127, 255, cv::THRESH_BINARY);

    // 存储所有连通域的轮廓
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;

    // 查找所有连通域（包括孔洞）
    cv::findContours(binarizedMask, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

    // 存储每个外部连通域的像素数（去除孔洞）
    std::vector<int> externalArea(contours.size(), 0);

    // 计算每个外部连通域的像素数（排除内部孔洞）
    for (int i = 0; i < contours.size(); ++i) {
        // 如果这是外部连通域
        if (hierarchy[i][3] == -1) {  // 如果父轮廓为 -1，表示这是一个外部轮廓
            externalArea[i] = cv::contourArea(contours[i]);  // 计算外部连通域的像素数

            // 查找该外部轮廓内部的所有孔洞，并减去这些孔洞的面积
            for (int j = 0; j < contours.size(); ++j) {
                if (hierarchy[j][3] == i) {  // 如果是外部连通域的子轮廓，即孔洞
                    externalArea[i] -= cv::contourArea(contours[j]);  // 减去孔洞的像素数
                }
            }
        }
    }

    // 找到面积最大的连通域的索引
    int maxAreaIndex = -1;
    int maxArea = 0;
    for (int i = 0; i < externalArea.size(); ++i) {
        if (externalArea[i] > maxArea) {
            maxArea = externalArea[i];
            maxAreaIndex = i;
        }
    }

    // 创建一个空的二值掩膜图像（全为0）
    cv::Mat filteredMask = mask.clone();  // 先克隆原始掩膜图像

    // 删除面积较小的连通域，只保留最大的外部连通域
    for (int i = 0; i < contours.size(); ++i) {
        if (i != maxAreaIndex && hierarchy[i][3] == -1) {  // 如果不是最大面积的外部连通域
            cv::drawContours(filteredMask, contours, i, cv::Scalar(0), cv::FILLED);  // 删除该连通域
        }
    }
    mask = filteredMask;
    // cv::imshow("Filtered Connected Components", filteredMask);
    // cv::imshow("Filtered Mask", mask);
    // cv::waitKey(0);  // 等待按键
}
//--------------------------------------------目标检测-------------------------------------------
Yolo11RectInference::Yolo11RectInference() {
    cudaSetDevice(0);
    yolo11_rect->make_pipe(true);

}

Yolo11RectInference::~Yolo11RectInference()
{

    if (yolo11_rect) {
        delete yolo11_rect;
        yolo11_rect = nullptr;
    }
}
void Yolo11RectInference::whenRecieveWpMaskInWorld(std::vector<cv::Point3d> worldCenters,
                                                   std::vector<cv::Mat> worldMaskImages){

    // 检查两个向量的大小是否一致
    if (worldCenters.size() != worldMaskImages.size()) {
        std::cerr << "Error: worldCenters and worldMaskImages sizes do not match!" << std::endl;
        return;
    }
    maskWorldCenters = worldCenters;

    // 创建一个向量来保存扩展后的图像
    std::vector<cv::Mat> expandedImages;

    // 更新掩膜画布
    for (size_t i = 0; i < worldMaskImages.size(); ++i) {
        cv::Mat image = worldMaskImages[i];

        // // 获取原图的大小
        int imageWidth = image.cols;
        int imageHeight = image.rows;
        cv::Mat resizedImage;
        // cv::resize(image, resizedImage, cv::Size(imageWidth * AdjustWorkpieceResolution,
        //                                          imageHeight * AdjustWorkpieceResolution),
        //                                          0, 0, cv::INTER_LANCZOS4);
        cv::resize(image, resizedImage, cv::Size(imageWidth * AdjustWorkpieceResolution,
                                                 imageHeight * AdjustWorkpieceResolution),
                   0, 0, cv::INTER_CUBIC);

        // 获取放大后的图像大小
        imageWidth = resizedImage.cols;
        imageHeight = resizedImage.rows;

        // 创建白色画布
        cv::Mat canvas = cv::Mat(expandedHeight, expandedWidth, CV_8UC3, cv::Scalar(255, 255, 255));  // 白色背景

        // 计算将原图放置到画布中心的起始位置
        int xOffset = (canvas.cols - imageWidth) / 2;
        int yOffset = (canvas.rows - imageHeight) / 2;

        // 将原图粘贴到画布的中心
        resizedImage.copyTo(canvas(cv::Rect(xOffset, yOffset, imageWidth, imageHeight)));
        // 顺时针旋转90°
        switch (rotationAngle) {
        case 0: // 不旋转
            break;
        case 90:
            cv::rotate(canvas, canvas, cv::ROTATE_90_CLOCKWISE);
            break;
        case 180:
            cv::rotate(canvas, canvas, cv::ROTATE_180);
            break;
        case 270:
            cv::rotate(canvas, canvas, cv::ROTATE_90_COUNTERCLOCKWISE);
            break;
        default:
            std::cerr << "Unsupported rotation angle: " << rotationAngle << ". Must be 0, 90, 180 or 270." << std::endl;
            break;
        }
        // 保存合成后的图像
        //cv::imwrite("./data/test/camera_" + std::to_string(i) + ".bmp", canvas);

        // 将扩展后的图像添加到 expandedImages 向量中
        expandedImages.push_back(canvas);
    }

    // 调用 whenImageNeedToInfer()，传递扩展后的图像
    whenImageNeedToInfer(expandedImages);

    //emit sendInferResultToMainWindow(res);
}


void Yolo11RectInference::whenImageNeedToInfer(std::vector<cv::Mat> cvImages) {
    imgNum = 0;
    cvImagesInferring_Rect = cvImages;
    //std::cout<<"cvImagesInferring_Rect.size()"<<cvImagesInferring_Rect.size()<<std::endl;
    // emit sendAppendInferLog(QString("已检测工件数量:%1").arg(cvImagesInferring_Rect.size()));
    std::vector<std::vector< cv::Rect_<float>>> rect_Dets;
    int i = 0;
    for (auto &cvimage : cvImagesInferring_Rect) {
        image = cvimage;
        std::vector<cv::Rect_<float>> rect_Det;
        inferSegAndCalcTime();
        for (auto& obj : objs_det) {
            rect_Det.push_back(obj.rect);
        }
        rect_Dets.push_back(rect_Det);
        inferedImages.push_back(res);
        // 在每次迭代时显示掩模图像
        // cv::imshow("Mask Image ", res);  // 显示掩模图像
        cv::imwrite("./data/roughWeldArea/" + std::to_string(i++) + ".bmp", res);
        // cv::waitKey(50);
        emit sendInferResultToMainWindow(res);
    }
    whenCoordinatesNeedToProceed(rect_Dets, maskWorldCenters);
}
//推理画图，结果为画出来的res和objs_det。
void Yolo11RectInference::inferSegAndCalcTime() {
    objs_det.clear();

    yolo11_rect->copy_from_Mat(image, size);
    auto start = std::chrono::system_clock::now();
    yolo11_rect->infer();
    auto end = std::chrono::system_clock::now();
    yolo11_rect->postprocess(objs_det, score_thres, iou_thres, topk, num_channels);

    yolo11_rect->draw_objects(image, res, objs_det, CLASS_NAMES, COLORS);
    inferTime = (double)std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000.;
    //emit sendCoordinateTofit (objs_Rect,imgNum);
    //cv::imshow("result", res);
    //cv::imwrite("D:/YOLO/yolo11_Seg_C++/data/roughWeldArea/camera_" +std::to_string(imgNum++)+"_result" + ".bmp", res);
    //cv::waitKey(0);
}
void Yolo11RectInference::whenCoordinatesNeedToProceed(std::vector<std::vector<cv::Rect_<float>>> rect_Dets,
                                                        std::vector<cv::Point3d> worldCenters) {

        // 计算画布的中心，使用 double 类型
    const double canvasW = static_cast<double>(expandedWidth);
    const double canvasH = static_cast<double>(expandedHeight);
    const cv::Point2d canvasCenter(canvasW / 2.0, canvasH / 2.0);

    std::vector<std::vector<std::array<double, 4>>> boxInfos;

    for (size_t i = 0; i < rect_Dets.size(); ++i) {
        const auto& rects = rect_Dets[i];
        std::vector<std::array<double, 4>> infos;

        for (const auto& rect : rects) {
            // 检测框中心点（旋转后图像）
            double cx = static_cast<double>(rect.x) + rect.width * 0.5;
            double cy = static_cast<double>(rect.y) + rect.height * 0.5;

            double origX = 0.0, origY = 0.0;
            double w = 0.0, h = 0.0;

            switch (rotationAngle) {
            case 0:
                origX = cx;
                origY = cy;
                w = rect.width;
                h = rect.height;
                break;
            case 90:
                origX = cy;
                origY = canvasW - cx;
                w = rect.height;
                h = rect.width;
                break;
            case 180:
                origX = canvasW - cx;
                origY = canvasH - cy;
                w = rect.width;
                h = rect.height;
                break;
            case 270:
                origX = canvasH - cy;
                origY = cx;
                w = rect.height;
                h = rect.width;
                break;
            default:
                std::cerr << "Unsupported rotation angle: " << rotationAngle << std::endl;
                continue;
            }

            // 计算相对位置（除以分辨率）
            cv::Point2d rel((origX - canvasCenter.x) / AdjustWorkpieceResolution,
                            (origY - canvasCenter.y) / AdjustWorkpieceResolution);

            double offsetX = rel.x + worldCenters[i].x;
            double offsetY = rel.y + worldCenters[i].y;
            double normW = w / AdjustWorkpieceResolution;
            double normH = h / AdjustWorkpieceResolution;

            infos.push_back({ offsetX, offsetY, normW, normH });
        }

        boxInfos.push_back(std::move(infos));
    }

    emit sendBoxInfoToDisplay(boxInfos);
}
