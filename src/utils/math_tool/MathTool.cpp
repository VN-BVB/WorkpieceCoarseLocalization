#include "MathTool.h"

MathTool::MathTool() {}

/**
 * @brief MathTool::distanceBetweenTwoPoints
 * @param point1
 * @param point2
 * @return
 */
double MathTool::twoPointsDistance(const Eigen::Vector4d &point1, const Eigen::Vector4d &point2) {
    double twoPowerOfX = pow(point1[0] - point2[0], 2);
    double twoPowerOfY = pow(point1[1] - point2[1], 2);
    double twoPowerOfZ = pow(point1[2] - point2[2], 2);
    return sqrt(twoPowerOfX + twoPowerOfY + twoPowerOfZ);
}
