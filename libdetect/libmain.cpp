#include <QDir>
#include <chrono>
#include <opencv2/opencv.hpp>
#include <vector>

#include "libdetect/libcbdetect/boards_from_corners.h"
#include "libdetect/libcbdetect/config.h"
#include "libdetect/libcbdetect/find_corners.h"
#include "libdetect/libcbdetect/plot_boards.h"
#include "libdetect/libcbdetect/plot_corners.h"

using namespace std::chrono;

void detect(const char* str, cbdetect::CornerType corner_type) {
    cbdetect::Corner corners;
    std::vector<cbdetect::Board> boards;
    cbdetect::Params params;
    params.corner_type = corner_type;

    cv::Mat img = cv::imread(str, cv::IMREAD_COLOR);

    auto t1 = high_resolution_clock::now();
    cbdetect::find_corners(img, corners, params);
    auto t2 = high_resolution_clock::now();
    // cbdetect::plot_corners(img, corners);
    // std::cout<<"plot corners:"<<corners<<std::endl;
    auto t3 = high_resolution_clock::now();
    cbdetect::boards_from_corners(img, corners, boards, params);
    auto t4 = high_resolution_clock::now();
    printf("Find corners took: %.3f ms\n", duration_cast<microseconds>(t2 - t1).count() / 1000.0);
    printf("Find boards took: %.3f ms\n", duration_cast<microseconds>(t4 - t3).count() / 1000.0);
    printf("Total took: %.3f ms\n", duration_cast<microseconds>(t2 - t1).count() / 1000.0 + duration_cast<microseconds>(t4 - t3).count() / 1000.0);
    // cbdetect::plot_boards(img, corners, boards, params);
    std::vector<std::vector<cv::Point2d>> board_points_sorted;
    cbdetect::plot_board_points(img, corners, boards, board_points_sorted);
    for (size_t board_idx = 0; board_idx < board_points_sorted.size(); ++board_idx) {
        std::cout << "Board " << board_idx << ":\n";
        const auto& points = board_points_sorted[board_idx];
        for (size_t pt_idx = 0; pt_idx < points.size(); ++pt_idx) {
            const auto& pt = points[pt_idx];
            std::cout << "  Point " << pt_idx << ": (" << pt.x << ", " << pt.y << ")\n";
        }
    }
}
void detect(int i, const cv::Mat& imageInput, std::vector<std::vector<cv::Point2f>>& imagePointsSeq, cbdetect::CornerType corner_type) {
    cbdetect::Corner corners;
    std::vector<cbdetect::Board> boards;
    cbdetect::Params params;
    params.corner_type = corner_type;

    auto t1 = high_resolution_clock::now();
    cbdetect::find_corners(imageInput, corners, params);
    auto t2 = high_resolution_clock::now();
    auto t3 = high_resolution_clock::now();
    cbdetect::boards_from_corners(imageInput, corners, boards, params);
    auto t4 = high_resolution_clock::now();

    printf("Image %d:\n", i);
    printf("  Find corners took: %.3f ms\n", duration_cast<microseconds>(t2 - t1).count() / 1000.0);
    printf("  Find boards took: %.3f ms\n", duration_cast<microseconds>(t4 - t3).count() / 1000.0);
    printf("  Total took: %.3f ms\n", duration_cast<microseconds>(t2 - t1).count() / 1000.0 + duration_cast<microseconds>(t4 - t3).count() / 1000.0);

    std::vector<std::vector<cv::Point2d>> board_points_sorted;
    cbdetect::plot_board_points(imageInput, corners, boards, board_points_sorted);

    for (size_t board_idx = 0; board_idx < board_points_sorted.size(); ++board_idx) {
        std::vector<cv::Point2f> boardPointsF;
        for (const auto& pt : board_points_sorted[board_idx]) {
            boardPointsF.emplace_back(static_cast<float>(pt.x), static_cast<float>(pt.y));
        }
        imagePointsSeq.push_back(boardPointsF);
    }
}
void detectImageInPath(int argc, char* argv[]) {
    printf("chessboards...");
    QDir dir("./calib/camera1/normal");
    QStringList filters;
    QFileInfoList files = dir.entryInfoList(filters, QDir::Files);
    for (const QFileInfo& fileInfo : files) {
        QString filePath = fileInfo.absoluteFilePath();
        std::string stdFilePath = filePath.toStdString();
        detect(stdFilePath.c_str(), cbdetect::SaddlePoint);
    }
}
// printf("deltilles...");
// detect("../../example_data/e6.png", cbdetect::MonkeySaddlePoint);
// QDir dir("./example_data");
// QStringList filters;
// filters << "*.bmp" << "*.png" << "*.jpg";

// QFileInfoList files = dir.entryInfoList(filters, QDir::Files);
// for (const QFileInfo &fileInfo : files) {
//     QString filePath = fileInfo.absoluteFilePath();
//     qDebug() << "Detecting:" << filePath;

//     detect(filePath.toStdString().c_str(), cbdetect::SaddlePoint);
// }
