#ifndef LIBDETECT_H
#define LIBDETECT_H

#include <QLabel>
#include <QMouseEvent>

extern int xInLabel, yInLabel, xInUi, yInUi, xInImg, yInImg, imageWidth, imageHeight;

class LibDetect : public QLabel {
    Q_OBJECT

public:
    LibDetect(QWidget *parent = 0);
    ~LibDetect();

public:
    void mousePressEvent(QMouseEvent *e);  // 按下
};

#endif  // LIBDETECT_H
