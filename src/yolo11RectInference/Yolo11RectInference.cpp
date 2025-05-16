#include "yolo11RectInference/Yolo11RectInference.h"

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

    // 创建一个向量来保存扩展后的图像
    std::vector<cv::Mat> expandedImages;

    // 更新掩膜画布
    for (size_t i = 0; i < worldMaskImages.size(); ++i) {
        cv::Mat image = worldMaskImages[i];

        // 获取原图的大小
        int imageWidth = image.cols;
        int imageHeight = image.rows;

        // 定义扩展后的画布大小
        int expandedWidth = 1626;
        int expandedHeight = 1236;

        // 创建白色画布
        cv::Mat canvas = cv::Mat(expandedHeight, expandedWidth, CV_8UC3, cv::Scalar(255, 255, 255));  // 白色背景

        // 计算将原图放置到画布中心的起始位置
        int xOffset = (canvas.cols - imageWidth) / 2;
        int yOffset = (canvas.rows - imageHeight) / 2;

        // 将原图粘贴到画布的中心
        image.copyTo(canvas(cv::Rect(xOffset, yOffset, imageWidth, imageHeight)));

        // 保存合成后的图像
        cv::imwrite("./data/test/camera_" + std::to_string(i) + ".bmp", canvas);

        // 将扩展后的图像添加到 expandedImages 向量中
        expandedImages.push_back(canvas);
    }

    // 调用 whenImageNeedToInfer()，传递扩展后的图像
    whenImageNeedToInfer(expandedImages);
}


void Yolo11RectInference::whenImageNeedToInfer(std::vector<cv::Mat> cvImages) {
    imgNum = 0;
    cvImagesInferring_Rect = cvImages;
    for (auto &cvimage : cvImagesInferring_Rect) {
        image = cvimage;
        inferSegAndCalcTime();
        //emit sendInferResultToMainWindow(res);
    }
}

void Yolo11RectInference::inferSegAndCalcTime() {
    objs_Rect.clear();
    yolo11_rect->copy_from_Mat(image, size);
    auto start = std::chrono::system_clock::now();
    yolo11_rect->infer();
    auto end = std::chrono::system_clock::now();
    yolo11_rect->postprocess(objs_Rect, score_thres, iou_thres, topk, num_channels);
    yolo11_rect->draw_objects(image, res, objs_Rect, CLASS_NAMES, COLORS);
    inferTime = (double)std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000.;
    emit sendCoordinateTofit (objs_Rect,imgNum);
}
