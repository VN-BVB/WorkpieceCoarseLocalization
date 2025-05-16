#pragma once

#include <QObject>
#include <QLineEdit>
#include "stdafx.h"
#include "rsdef.h"
#include "aubo_example.h"
#include "aubo_myMove.h"

//#define pi 3.14159265358979323846;
//机械臂服务器IP地址 即控制器的IP （通过示教器settings里的network中点击ifconfig可以获得）

//#define ROBOT_ADDR "192.168.0.10"//真实地址
//#define ROBOT_PORT 8899 //默认为8899

#define ROBOT_ADDR "192.168.100.1"//真实地址
#define ROBOT_PORT 8899 //默认为8899

#define M_PI 3.14159265358979323846
//机械臂控制上下文句柄
extern RSHD g_rshd;

// //在QT界面上显示机器人的路点实时信息，全局声明
// extern QLineEdit *aubo_lineEdit_Joint1;
// extern QLineEdit *aubo_lineEdit_Joint2;
// extern QLineEdit *aubo_lineEdit_Joint3;
// extern QLineEdit *aubo_lineEdit_Joint4;
// extern QLineEdit *aubo_lineEdit_Joint5;
// extern QLineEdit *aubo_lineEdit_Joint6;
// extern QLineEdit *aubo_lineEdit_Flange_x;
// extern QLineEdit *aubo_lineEdit_Flange_y;
// extern QLineEdit *aubo_lineEdit_Flange_z;
// extern QLineEdit *aubo_lineEdit_Flange_rx;
// extern QLineEdit *aubo_lineEdit_Flange_ry;
// extern QLineEdit *aubo_lineEdit_Flange_rz;



class Aubo_control : public QObject{
	Q_OBJECT
public:
	Aubo_control(void);


	bool aubo_connect(RSHD &rshd, const char * addr, int port);  //aubo机器人TCP连接
	bool aubo_disconnect(RSHD &rshd);

	void aubo_get_RealTime_waypoint(RSHD rshd);  //获取机器人的实时路点

	void aubo_get_current_waypoint(RSHD rshd, wayPoint_S *waypoint);  //获取当前机器人的位置信息
};
