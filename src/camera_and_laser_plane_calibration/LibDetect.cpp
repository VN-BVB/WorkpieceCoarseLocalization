#include "LibDetect.h"

#include <iostream>

int xInLabel, yInLabel, xInUi, yInUi, xInImg, yInImg, imageWidth, imageHeight;

LibDetect::LibDetect(QWidget* parent) : QLabel(parent) {}

LibDetect::~LibDetect() {}

void LibDetect::mousePressEvent(QMouseEvent* e) {
    xInLabel = e->x();
    yInLabel = e->y();
    xInUi = geometry().x() + xInLabel;
    yInUi = geometry().y() + yInLabel;

    // 这里需要知道输入图像的大小，然后根据缩放关系求出相应位置
    xInImg = xInLabel * imageWidth / width();
    yInImg = yInLabel * imageHeight / height();
}
