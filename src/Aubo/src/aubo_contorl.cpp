#include "../include/aubo_control.h"


//机械臂控制上下文句柄
RSHD g_rshd = -1;
//在QT界面上显示机器人的路点实时信息，定义
QLineEdit *aubo_lineEdit_Joint1 = nullptr;
QLineEdit *aubo_lineEdit_Joint2 = nullptr;
QLineEdit *aubo_lineEdit_Joint3 = nullptr;
QLineEdit *aubo_lineEdit_Joint4 = nullptr;
QLineEdit *aubo_lineEdit_Joint5 = nullptr;
QLineEdit *aubo_lineEdit_Joint6 = nullptr;
QLineEdit *aubo_lineEdit_Flange_x = nullptr;
QLineEdit *aubo_lineEdit_Flange_y = nullptr;
QLineEdit *aubo_lineEdit_Flange_z = nullptr;
QLineEdit *aubo_lineEdit_Flange_rx = nullptr;
QLineEdit *aubo_lineEdit_Flange_ry = nullptr;
QLineEdit *aubo_lineEdit_Flange_rz = nullptr;




//构造函数
Aubo_control::Aubo_control()
{

}

//aubo机器人TCP连接
bool Aubo_control::aubo_connect(RSHD &rshd, const char * addr, int port)
{
	bool result = false;

	rshd = RS_FAILED;//在rsdef.h里define RS_FAILED=-1 

	//初始化接口库
	if (rs_initialize() == RS_SUCC)
	{
		//创建上下文
		if (rs_create_context(&rshd) == RS_SUCC)
		{
			//登陆机械臂服务器
			if (rs_login(rshd, addr, port) == RS_SUCC)
			{
				result = true;
				//登陆成功
				std::cout << "login succ" << std::endl;

				////启动机械臂(必须连接真实机械臂）
				//example_robotStartup(g_rshd);
			}
			else
			{
				//登陆失败
				std::cerr << "login failed" << std::endl;
			}
		}
		else
		{
			//创建上下文失败
			std::cerr << "rs_create_context error" << std::endl;
		}
	}
	else
	{
		//初始化接口库失败
		std::cerr << "rs_initialize error" << std::endl;
	}

	return result;
}

//aubo机器人断开连接
bool Aubo_control::aubo_disconnect(RSHD &rshd)
{
	////关闭机械臂（必须连接真实机械臂，相当于给机器人关机）
	//example_robotShutdown(rshd);

	if (rs_logout(rshd) == RS_SUCC ? true : false)  //退出登录
	{
		//反初始化，释放内存
		if (RS_SUCC == rs_uninitialize())
			return 1;
		else
			return 0;
	}
	else
		return 0;
}

//在QT界面上显示机器人的路点实时信息
void QT_callback_RealTimeRoadPoint(const aubo_robot_namespace::wayPoint_S  *wayPoint, void *arg)
{	
	//输出6关节角(deg)
	aubo_lineEdit_Joint1->setText(QString(QStringLiteral("%1")).arg(QString::number(wayPoint->jointpos[0] * 180.0 / M_PI, 'f', 6)));
	aubo_lineEdit_Joint2->setText(QString(QStringLiteral("%1")).arg(QString::number(wayPoint->jointpos[1] * 180.0 / M_PI, 'f', 6)));
	aubo_lineEdit_Joint3->setText(QString(QStringLiteral("%1")).arg(QString::number(wayPoint->jointpos[2] * 180.0 / M_PI, 'f', 6)));
	aubo_lineEdit_Joint4->setText(QString(QStringLiteral("%1")).arg(QString::number(wayPoint->jointpos[3] * 180.0 / M_PI, 'f', 6)));
	aubo_lineEdit_Joint5->setText(QString(QStringLiteral("%1")).arg(QString::number(wayPoint->jointpos[4] * 180.0 / M_PI, 'f', 6)));
	aubo_lineEdit_Joint6->setText(QString(QStringLiteral("%1")).arg(QString::number(wayPoint->jointpos[5] * 180.0 / M_PI, 'f', 6)));
	
	//输出末端兰x,y,z,rx,ry,rz
	aubo_lineEdit_Flange_x->setText(QString(QStringLiteral("%1")).arg(QString::number(wayPoint->cartPos.position.x, 'f', 6)));
	aubo_lineEdit_Flange_y->setText(QString(QStringLiteral("%1")).arg(QString::number(wayPoint->cartPos.position.y, 'f', 6)));
	aubo_lineEdit_Flange_z->setText(QString(QStringLiteral("%1")).arg(QString::number(wayPoint->cartPos.position.z, 'f', 6)));
	double roll, pitch, yaw;  
	//四元数转rpy
	toEulerAngle(wayPoint->orientation.x, wayPoint->orientation.y, wayPoint->orientation.z, wayPoint->orientation.w, roll, pitch, yaw);
	aubo_lineEdit_Flange_rx->setText(QString(QStringLiteral("%1")).arg(QString::number(roll * 180.0 / M_PI, 'f', 6)));
	aubo_lineEdit_Flange_ry->setText(QString(QStringLiteral("%1")).arg(QString::number(pitch * 180.0 / M_PI, 'f', 6)));
	aubo_lineEdit_Flange_rz->setText(QString(QStringLiteral("%1")).arg(QString::number(yaw * 180.0 / M_PI, 'f', 6)));

}

//获取机器人的实时路点
void Aubo_control::aubo_get_RealTime_waypoint(RSHD rshd)
{
	//const aubo_robot_namespace::wayPoint_S  *wayPoint;
	if (RS_SUCC == rs_enable_push_realtime_roadpoint(rshd, true))  //使能
	{
		if (RS_SUCC == rs_setcallback_realtime_roadpoint(rshd, QT_callback_RealTimeRoadPoint, NULL))  //注册
		{

		}
		else
		{
			std::cerr << "call rs_setcallback_realtime_roadpoint failed" << std::endl;
		}
	}
	else
		std::cerr << "call rs_enable_push_realtime_roadpoint failed!" << std::endl;
}

//获取当前机器人的位置信息
void Aubo_control::aubo_get_current_waypoint(RSHD rshd, wayPoint_S *waypoint)
{
	rs_get_current_waypoint(rshd, waypoint);
}


