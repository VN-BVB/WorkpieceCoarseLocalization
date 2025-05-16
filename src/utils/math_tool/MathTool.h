#ifndef MATHTOOL_H
#define MATHTOOL_H

#pragma execution_character_set("utf-8")

class MathTool {
public:
    MathTool();
    static double twoPointsDistance(const Eigen::Vector4d& point1, const Eigen::Vector4d& point2);
};

#endif  // MATHTOOL_H
