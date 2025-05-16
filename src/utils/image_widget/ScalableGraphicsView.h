#ifndef SCALABLEGRAPHICSVIEW_H
#define SCALABLEGRAPHICSVIEW_H
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QLabel>
#include <QGraphicsPixmapItem>
#include <QScrollBar>
class ScalableGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    // 定义构造函数，接受 QGraphicsScene 和 QWidget* 作为参数
    explicit ScalableGraphicsView(QWidget* parent = nullptr); // 父类为 QWidget*


    void wheelEvent(QWheelEvent *event) override;  // 处理鼠标滚轮事件
    void mousePressEvent(QMouseEvent *event) override;  // 处理鼠标按下事件
signals:
    void senderSignalPixelCoordinates(const int x,const int  y);
private:
    bool isDragging = false;  // 是否正在拖拽
    QPoint lastMousePos;  // 上一次鼠标位置
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
};

#endif // SCALABLEGRAPHICSVIEW_H
