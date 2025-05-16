#pragma once
#include"./include/aubo_control.h"
#include <QThread>
#include <opencv2/core.hpp>
#pragma execution_character_set("utf-8")
//机械臂控制上下文句柄
extern int cameraIndex;
extern RSHD g_rshd;
class RobotController : public QObject{
	Q_OBJECT
public:
    RobotController(void);

    void whenRobotConnect();  //aubo机器人TCP连接
    void whenRobotDisconnect();
    void whenGetCurrentWaypoint();

    std::string num2fixed_str(int i);
signals:
    void appendMessageLog(QString message);
private:
    Aubo_control *aubo = new Aubo_control;
};
