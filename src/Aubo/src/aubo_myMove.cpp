#include "../include/stdafx.h"
#include "../include/aubo_myMove.h"

const double M_PI = 3.14159265358979323846;


bool my_moveJ_relative(RSHD rshd, double targePos[6])
{
	bool result = false;

	RobotRecongnitionParam param;

	rs_get_robot_recognition_param(rshd, 1, &param);

	/** 接口调用: 初始化运动属性 ***/
	rs_init_global_move_profile(rshd);

	/** 接口调用: 设置关节型运动的最大加速度 ***/
	aubo_robot_namespace::JointVelcAccParam jointMaxAcc;
	jointMaxAcc.jointPara[0] = 30.0 / 180.0*M_PI;
	jointMaxAcc.jointPara[1] = 30.0 / 180.0*M_PI;
	jointMaxAcc.jointPara[2] = 30.0 / 180.0*M_PI;
	jointMaxAcc.jointPara[3] = 30.0 / 180.0*M_PI;
	jointMaxAcc.jointPara[4] = 30.0 / 180.0*M_PI;
	jointMaxAcc.jointPara[5] = 30.0 / 180.0*M_PI;   //接口要求单位是弧度
	rs_set_global_joint_maxacc(rshd, &jointMaxAcc);

	/** 接口调用: 设置关节型运动的最大速度 ***/
	aubo_robot_namespace::JointVelcAccParam jointMaxVelc;
	jointMaxVelc.jointPara[0] = 30.0 / 180.0*M_PI;
	jointMaxVelc.jointPara[1] = 30.0 / 180.0*M_PI;
	jointMaxVelc.jointPara[2] = 30.0 / 180.0*M_PI;
	jointMaxVelc.jointPara[3] = 30.0 / 180.0*M_PI;
	jointMaxVelc.jointPara[4] = 30.0 / 180.0*M_PI;
	jointMaxVelc.jointPara[5] = 30.0 / 180.0*M_PI;   //接口要求单位是弧度
	rs_set_global_joint_maxvelc(rshd, &jointMaxVelc);

	//机械臂当前路点信息
	aubo_robot_namespace::wayPoint_S wayPoint;
	//目标位置对应的关节角
	double targetRadian[ARM_DOF] = { 0 };

	if (RS_SUCC == rs_get_current_waypoint(rshd, &wayPoint))
	{
		targetRadian[0] = wayPoint.jointpos[0] + targePos[0];
		targetRadian[1] = wayPoint.jointpos[1] + targePos[1];
		targetRadian[2] = wayPoint.jointpos[2] + targePos[2];
		targetRadian[3] = wayPoint.jointpos[3] + targePos[3];
		targetRadian[4] = wayPoint.jointpos[4] + targePos[4];
		targetRadian[5] = wayPoint.jointpos[5] + targePos[5];
	}
	else
	{
		std::cerr << "get current waypoint error" << std::endl;
	}

	//运动到目标位置
	if (rs_move_joint(rshd, targetRadian) == RS_SUCC)
	{
		result = true;
		std::cout << "movej relative succ" << std::endl;
	}
	else
	{
		std::cerr << "movej failed!" << std::endl;
	}

	return result;
}

bool my_moveJ(RSHD rshd, double targePos[6])
{
	bool result = false;

	RobotRecongnitionParam param;
	rs_get_robot_recognition_param(rshd, 1, &param);

	/** 接口调用: 初始化运动属性 ***/
	rs_init_global_move_profile(rshd);

	/** 接口调用: 设置关节型运动的最大加速度 ***/
	aubo_robot_namespace::JointVelcAccParam jointMaxAcc;
	jointMaxAcc.jointPara[0] = 30.0 / 180.0*M_PI;
	jointMaxAcc.jointPara[1] = 30.0 / 180.0*M_PI;
	jointMaxAcc.jointPara[2] = 30.0 / 180.0*M_PI;
	jointMaxAcc.jointPara[3] = 30.0 / 180.0*M_PI;
	jointMaxAcc.jointPara[4] = 30.0 / 180.0*M_PI;
	jointMaxAcc.jointPara[5] = 30.0 / 180.0*M_PI;   //接口要求单位是弧度
	rs_set_global_joint_maxacc(rshd, &jointMaxAcc);

	/** 接口调用: 设置关节型运动的最大速度 ***/
	aubo_robot_namespace::JointVelcAccParam jointMaxVelc;
	jointMaxVelc.jointPara[0] = 30.0 / 180.0*M_PI;
	jointMaxVelc.jointPara[1] = 30.0 / 180.0*M_PI;
	jointMaxVelc.jointPara[2] = 30.0 / 180.0*M_PI;
	jointMaxVelc.jointPara[3] = 30.0 / 180.0*M_PI;
	jointMaxVelc.jointPara[4] = 30.0 / 180.0*M_PI;
	jointMaxVelc.jointPara[5] = 30.0 / 180.0*M_PI;   //接口要求单位是弧度
	rs_set_global_joint_maxvelc(rshd, &jointMaxVelc);


	//运动到目标位置
	if (rs_move_joint(rshd, targePos) == RS_SUCC)
	{
		result = true;
		std::cout << "movej succ" << std::endl;
	}
	else
	{
		std::cerr << "movej failed!" << std::endl;
	}

	return result;
}

bool my_moveL(RSHD rshd, Pos pos, Ori ori)
{
	bool result = false;


	/** 接口调用: 初始化运动属性 ***/
	rs_init_global_move_profile(rshd);

	/** 接口调用: 设置关节型运动的最大加速度 ***/
	aubo_robot_namespace::JointVelcAccParam jointMaxAcc;
	jointMaxAcc.jointPara[0] = 30.0 / 180.0*M_PI;
	jointMaxAcc.jointPara[1] = 30.0 / 180.0*M_PI;
	jointMaxAcc.jointPara[2] = 30.0 / 180.0*M_PI;
	jointMaxAcc.jointPara[3] = 30.0 / 180.0*M_PI;
	jointMaxAcc.jointPara[4] = 30.0 / 180.0*M_PI;
	jointMaxAcc.jointPara[5] = 30.0 / 180.0*M_PI;   //接口要求单位是弧度
	rs_set_global_joint_maxacc(rshd, &jointMaxAcc);

	/** 接口调用: 设置关节型运动的最大速度 ***/
	aubo_robot_namespace::JointVelcAccParam jointMaxVelc;
	jointMaxVelc.jointPara[0] = 30.0 / 180.0*M_PI;
	jointMaxVelc.jointPara[1] = 30.0 / 180.0*M_PI;
	jointMaxVelc.jointPara[2] = 30.0 / 180.0*M_PI;
	jointMaxVelc.jointPara[3] = 30.0 / 180.0*M_PI;
	jointMaxVelc.jointPara[4] = 30.0 / 180.0*M_PI;
	jointMaxVelc.jointPara[5] = 30.0 / 180.0*M_PI;   //接口要求单位是弧度
	rs_set_global_joint_maxvelc(rshd, &jointMaxVelc);

	//获取当前路点信息
	aubo_robot_namespace::wayPoint_S wayPoint;

	//逆解位置信息
	aubo_robot_namespace::wayPoint_S targetPoint;

	//目标位置对应的关节角
	double targetRadian[ARM_DOF] = { 0 };

	if (RS_SUCC == rs_get_current_waypoint(rshd, &wayPoint))//获取当前的位置、姿态、关节角信息
	{
		//参考当前姿态逆解得到六个关节角
		if (RS_SUCC == rs_inverse_kin(rshd, wayPoint.jointpos, &pos, &ori, &targetPoint))//当前关节角；目标位置、目标姿态；逆解目标关节角
		{
			//将得到目标位置,将6关节角度设置为用户给定的角度（必须在+-175度）
			targetRadian[0] = targetPoint.jointpos[0];
			targetRadian[1] = targetPoint.jointpos[1];
			targetRadian[2] = targetPoint.jointpos[2];
			targetRadian[3] = targetPoint.jointpos[3];
			targetRadian[4] = targetPoint.jointpos[4];
			targetRadian[5] = targetPoint.jointpos[5];

			//直线动到目标位置
			if (RS_SUCC == rs_move_line(rshd, targetRadian))
			{
				std::cout << "at target" << std::endl;
			}
			else
			{
				std::cerr << "move joint error" << std::endl;
			}
		}
		else
		{
			std::cerr << "ik failed" << std::endl;
		}

	}
	else
	{
		std::cerr << "get current waypoint error" << std::endl;
	}

	return result;
}

bool my_moveP(RSHD rshd, std::vector<std::vector<double>> wayPoint)
{
	/** 模拟业务 **/
	/** 接口调用: 初始化运动属性 ***/
	rs_init_global_move_profile(rshd);

	/** 接口调用: 设置关节型运动的最大加速度 ***/
	aubo_robot_namespace::JointVelcAccParam jointMaxAcc;
	jointMaxAcc.jointPara[0] = 30.0 / 180.0*M_PI;
	jointMaxAcc.jointPara[1] = 30.0 / 180.0*M_PI;
	jointMaxAcc.jointPara[2] = 30.0 / 180.0*M_PI;
	jointMaxAcc.jointPara[3] = 30.0 / 180.0*M_PI;
	jointMaxAcc.jointPara[4] = 30.0 / 180.0*M_PI;
	jointMaxAcc.jointPara[5] = 30.0 / 180.0*M_PI;   //接口要求单位是弧度
	rs_set_global_joint_maxacc(rshd, &jointMaxAcc);

	/** 接口调用: 设置关节型运动的最大速度 ***/
	aubo_robot_namespace::JointVelcAccParam jointMaxVelc;
	jointMaxVelc.jointPara[0] = 30.0 / 180.0*M_PI;
	jointMaxVelc.jointPara[1] = 30.0 / 180.0*M_PI;
	jointMaxVelc.jointPara[2] = 30.0 / 180.0*M_PI;
	jointMaxVelc.jointPara[3] = 30.0 / 180.0*M_PI;
	jointMaxVelc.jointPara[4] = 30.0 / 180.0*M_PI;
	jointMaxVelc.jointPara[5] = 30.0 / 180.0*M_PI;   //接口要求单位是弧度
	rs_set_global_joint_maxvelc(rshd, &jointMaxVelc);


	/** 接口调用: 初始化运动属性 ***/
	rs_init_global_move_profile(rshd);

	/** 接口调用: 设置末端型运动的最大加速度 　　直线运动属于末端型运动***/
	double endMoveMaxAcc;
	endMoveMaxAcc = 0.01;   //单位米每秒
	rs_set_global_end_max_line_acc(rshd, endMoveMaxAcc);
	rs_set_global_end_max_angle_acc(rshd, endMoveMaxAcc);


	/** 接口调用: 设置末端型运动的最大速度 直线运动属于末端型运动***/
	double endMoveMaxVelc;
	endMoveMaxVelc = 0.01;   //单位米每秒
	rs_set_global_end_max_line_velc(rshd, endMoveMaxVelc);
	rs_set_global_end_max_angle_velc(rshd, endMoveMaxVelc);

	//准备点  关节运动属于关节型运动
	rs_init_global_move_profile(rshd);
	rs_set_global_joint_maxacc(rshd, &jointMaxAcc);
	rs_set_global_joint_maxvelc(rshd, &jointMaxVelc);

	double jointAngle[aubo_robot_namespace::ARM_DOF] = { 0 };

	//关节运动至轨迹起点
	jointAngle[0] = wayPoint[0][0];//初始位姿的关节转角（单位：°）
	jointAngle[1] = wayPoint[0][1];
	jointAngle[2] = wayPoint[0][2];
	jointAngle[3] = wayPoint[0][3];
	jointAngle[4] = wayPoint[0][4];
	jointAngle[5] = wayPoint[0][5];

	int ret = rs_move_joint(rshd, jointAngle);
	if (ret != RS_SUCC)
	{
		std::cerr << "JointMove失败.　ret:" << ret << std::endl;
	}

	//MoveP
	rs_init_global_move_profile(rshd);//初始化全局运动属性

	rs_set_global_end_max_line_acc(rshd, endMoveMaxAcc);//末端的最大/最小速度/加速度
	rs_set_global_end_max_angle_acc(rshd, endMoveMaxAcc);
	rs_set_global_end_max_line_velc(rshd, endMoveMaxVelc);
	rs_set_global_end_max_angle_velc(rshd, endMoveMaxVelc);

	//for (int i = 0; i < wayPoint.size(); i++)//打印wayPoint
	//{
	//	for (int j = 0; j < wayPoint[i].size(); j++)
	//	{
	//		std::cout << wayPoint[i][j] * 180 / M_PI << std::endl;
	//	}
	//	std::cout << std::endl;
	//}

	for (int i = 0; i < wayPoint.size(); i++)//添加路点
	{
		for (int j = 0; j < wayPoint[i].size(); j++)
		{
			jointAngle[j] = wayPoint[i][j];
		}
		rs_add_waypoint(rshd, jointAngle);

	}

	//交融半径
	rs_set_blend_radius(rshd, 0.0001);
	//rs_set_circular_loop_times(rshd, 1);//圆轨迹
	if (RS_SUCC != rs_move_track(rshd, CARTESIAN_MOVEP))
	{
		std::cerr << "TrackMove failed.　ret:" << ret << std::endl;
	}
	wayPoint.clear();

	return 0;
}

bool my_moveR(RSHD rshd, double targePos[6], Move_Rotate_Axis axis, double rotateAngle)
{
	//初始化运动属性
	rs_init_global_move_profile(rshd);

	//设置关节运动最大加速度
	aubo_robot_namespace::JointVelcAccParam jointMaxAcc;
	jointMaxAcc.jointPara[0] = 30 * M_PI / 180;
	jointMaxAcc.jointPara[1] = 30 * M_PI / 180;
	jointMaxAcc.jointPara[2] = 30 * M_PI / 180;
	jointMaxAcc.jointPara[3] = 30 * M_PI / 180;
	jointMaxAcc.jointPara[4] = 30 * M_PI / 180;
	jointMaxAcc.jointPara[5] = 30 * M_PI / 180;
	rs_set_global_joint_maxacc(rshd, &jointMaxAcc);

	//设置关节运动最大速度
	aubo_robot_namespace::JointVelcAccParam jointMaxVelc;
	jointMaxVelc.jointPara[0] = 30 * M_PI / 180;
	jointMaxVelc.jointPara[1] = 30 * M_PI / 180;
	jointMaxVelc.jointPara[2] = 30 * M_PI / 180;
	jointMaxVelc.jointPara[3] = 30 * M_PI / 180;
	jointMaxVelc.jointPara[4] = 30 * M_PI / 180;
	jointMaxVelc.jointPara[5] = 30 * M_PI / 180;
	rs_set_global_joint_maxvelc(rshd, &jointMaxVelc);

	//起始路点
	Pos toolInBase_pos;//工具在基坐标系下的位置
	toolInBase_pos.x = targePos[0];
	toolInBase_pos.y = targePos[1];
	toolInBase_pos.z = targePos[2];

	Ori toolInBase_ori;//工具在基坐标系下的姿态 四元数
	Rpy toolInBase_rpy;//工具在基坐标系下的姿态 RPY角
	toolInBase_rpy.rx = targePos[3] / 180.0 * M_PI;
	toolInBase_rpy.ry = targePos[4] / 180.0 * M_PI;
	toolInBase_rpy.rz = targePos[5] / 180.0 * M_PI;
	rs_rpy_to_quaternion(rshd, &toolInBase_rpy, &toolInBase_ori);

	aubo_robot_namespace::CoordCalibrateByJointAngleAndTool baseCoord;//基坐标系
	baseCoord.coordType = aubo_robot_namespace::BaseCoordinate;

	ToolInEndDesc tool;//工具相对于法兰盘中心的位姿(机器人工具标定结果)
	Rpy tool_rpy;
	tool.toolInEndPosition.x = -0.000012;
	tool.toolInEndPosition.y = -0.000249;
	tool.toolInEndPosition.z = 0.413230;
	tool_rpy.rx = 0.000000 / 180.0 * M_PI;
	tool_rpy.ry = 0.000000 / 180.0 * M_PI;
	tool_rpy.rz = 0.000000 / 180.0 * M_PI;
	rs_rpy_to_quaternion(rshd, &tool_rpy, &tool.toolInEndOrientation);

	Pos flangeCenterPosOnBase;//法兰盘中心在基坐标系下的位置
	Ori flangeCenterOriOnBase;//法兰盘中心在基坐标系下的姿态（四元数）
	double flangeCenterOnBase[6] = { 0 };
	if (RS_SUCC == rs_user_to_base(rshd, &toolInBase_pos, &toolInBase_ori, &baseCoord, &tool, &flangeCenterPosOnBase, &flangeCenterOriOnBase))
	{
		double startPointJointAngle[aubo_robot_namespace::ARM_DOF] = { 0.0 / 180.0*M_PI,  0.0 / 180.0*M_PI,  0.0 / 180.0*M_PI, 0.0 / 180.0*M_PI, 0.0 / 180.0*M_PI,0.0 / 180.0*M_PI };
		aubo_robot_namespace::wayPoint_S wayPointi;
		

		if (RS_SUCC == rs_inverse_kin(rshd, startPointJointAngle, &flangeCenterPosOnBase, &flangeCenterOriOnBase, &wayPointi))//逆解求关节角
		{
			for (int i = 0; i < 6; i++)
			{
				flangeCenterOnBase[i] = wayPointi.jointpos[i];
			}
			std::cout << "ik succ" << std::endl;
		}
		else
		{
			std::cerr << "ik failed" << std::endl;
		}
	}

	//关节运动到起始路点
	rs_move_joint(rshd, flangeCenterOnBase, true);

	//设置工具
	rs_set_tool_kinematics_param(rshd, &tool);

	//旋转轴和旋转角度
	double angle = rotateAngle * M_PI / 180;

	//设置工具坐标系
	aubo_robot_namespace::CoordCalibrateByJointAngleAndTool endCoord;
	endCoord.coordType = aubo_robot_namespace::EndCoordinate;
	endCoord.toolDesc = tool;

	//旋转运动：工具末端在基坐标系下绕axis+方向旋转angle度，当前位置保持不变
	rs_move_rotate(rshd, &baseCoord, &axis, angle, true);

	return 0;
}

bool positionToJoint(RSHD rshd, std::string fileName, std::vector<std::vector<double>> &wayPoint)
{

	std::ifstream infile;
	infile.open(fileName);
	double x, y, z, rx, ry, rz;
	std::vector<std::vector<double>> positionPoint;
	std::vector<double> readPoint;
	while (infile >> x >> y >> z >> rx >> ry >> rz)//读取给定路径点（工具末端将工具末端在基坐标系的位姿）
	{
		readPoint.push_back(x);
		readPoint.push_back(y);
		readPoint.push_back(z);
		readPoint.push_back(rx);
		readPoint.push_back(ry);
		readPoint.push_back(rz);
		positionPoint.push_back(readPoint);
		readPoint.clear();
	}

	double startPointJointAngle[aubo_robot_namespace::ARM_DOF] = { 0.0 / 180.0*M_PI,  0.0 / 180.0*M_PI,  0.0 / 180.0*M_PI, 0.0 / 180.0*M_PI, 0.0 / 180.0*M_PI,0.0 / 180.0*M_PI };
	aubo_robot_namespace::Pos targetPosition;
	aubo_robot_namespace::Rpy rpy;
	aubo_robot_namespace::Ori targetOri;

	for (int i = 0; i < positionPoint.size(); i++)//将给定路径点转换为关节角
	{
		targetPosition.x = positionPoint[i][0];
		targetPosition.y = positionPoint[i][1];
		targetPosition.z = positionPoint[i][2];

		rpy.rx = positionPoint[i][3] / 180.0 * M_PI;
		rpy.ry = positionPoint[i][4] / 180.0 * M_PI;
		rpy.rz = positionPoint[i][5] / 180.0 * M_PI;

		rs_rpy_to_quaternion(rshd, &rpy, &targetOri);//RPY转四元数

		aubo_robot_namespace::wayPoint_S wayPointi;
		std::vector<double> jointAngle;
		if (RS_SUCC == rs_inverse_kin(rshd, startPointJointAngle, &targetPosition, &targetOri, &wayPointi))//逆解求关节角
		{
			jointAngle.push_back(wayPointi.jointpos[0]);
			jointAngle.push_back(wayPointi.jointpos[1]);
			jointAngle.push_back(wayPointi.jointpos[2]);
			jointAngle.push_back(wayPointi.jointpos[3]);
			jointAngle.push_back(wayPointi.jointpos[4]);
			jointAngle.push_back(wayPointi.jointpos[5]);
			wayPoint.push_back(jointAngle);
			jointAngle.clear();
			std::cout << "ik succ" << std::endl;

			for (int j = 0; j < 6; j++)
			{
				startPointJointAngle[j] = wayPointi.jointpos[j];
			}
		}
		else
		{
			std::cerr << "ik failed" << std::endl;
		}
	}

	return 0;
}

bool positionToJointTool(RSHD rshd, std::string fileName, ToolInEndDesc tool, std::vector<std::vector<double>> &wayPoint)
{
	std::ifstream infile;
	infile.open(fileName);
	double x, y, z, rx, ry, rz;
	std::vector<std::vector<double>> positionPoint;
	std::vector<double> readPoint;
	while (infile >> x >> y >> z >> rx >> ry >> rz)//读取给定路径点（工具末端将工具末端在基坐标系的位姿）
	{
		readPoint.push_back(x);
		readPoint.push_back(y);
		readPoint.push_back(z);
		readPoint.push_back(rx);
		readPoint.push_back(ry);
		readPoint.push_back(rz);
		positionPoint.push_back(readPoint);
		readPoint.clear();
	}

	//增加工具，将工具末端在基坐标系的位姿->法兰盘在基坐标系的位姿
	std::ofstream onfile;
	onfile.open("jointAngle.txt");
	for (int i = 0; i < positionPoint.size(); i++)//将给定路径点转换为关节角
	{
		Pos toolInBase_pos;//工具在基坐标系下的位置
		toolInBase_pos.x = positionPoint[i][0];
		toolInBase_pos.y = positionPoint[i][1];
		toolInBase_pos.z = positionPoint[i][2];

		Ori toolInBase_ori;//工具在基坐标系下的姿态 四元数
		Rpy toolInBase_rpy;//工具在基坐标系下的姿态 RPY角
		toolInBase_rpy.rx = positionPoint[i][3] / 180.0 * M_PI;
		toolInBase_rpy.ry = positionPoint[i][4] / 180.0 * M_PI;
		toolInBase_rpy.rz = positionPoint[i][5] / 180.0 * M_PI;
		if (RS_SUCC == rs_rpy_to_quaternion(rshd, &toolInBase_rpy, &toolInBase_ori))//RPY转四元数
		{
			//std::cout << "工具末端在基坐标系下的位置: ";
			//std::cout << "(" << toolInBase_pos.x << ", " << toolInBase_pos.y << ", " << toolInBase_pos.z << ")";
			//std::cout << std::endl;

			//std::cout << "工具末端在基坐标系下的姿态（欧拉角）: ";
			//std::cout << "(" << toolInBase_rpy.rx * 180 / M_PI << ", " << toolInBase_rpy.ry * 180 / M_PI << ", " << toolInBase_rpy.rz * 180 / M_PI << ")";
			//std::cout << std::endl;
		}
		std::cout << std::endl;

		aubo_robot_namespace::CoordCalibrateByJointAngleAndTool baseCoord;//基坐标系
		baseCoord.coordType = aubo_robot_namespace::BaseCoordinate;

		Pos flangeCenterPosOnBase;//法兰盘中心在基坐标系下的位置
		Ori flangeCenterOriOnBase;//法兰盘中心在基坐标系下的姿态（四元数）
		//工具末端在基坐标系的位姿->法兰盘在基坐标系的位姿
		if (RS_SUCC == rs_user_to_base(rshd, &toolInBase_pos, &toolInBase_ori, &baseCoord, &tool, &flangeCenterPosOnBase, &flangeCenterOriOnBase))
		{
			aubo_robot_namespace::Rpy flangeCenterRpyOnBase;//法兰盘中心在基坐标系下的姿态（欧拉角）
			rs_quaternion_to_rpy(rshd, &flangeCenterOriOnBase, &flangeCenterRpyOnBase);

			//std::cout << "法兰盘中心在基坐标系下的位置: ";
			//std::cout << "(" << flangeCenterPosOnBase.x << ", " << flangeCenterPosOnBase.y << ", " << flangeCenterPosOnBase.z << ")";
			//std::cout << std::endl;

			//std::cout << "法兰盘中心在基坐标系下的姿态（欧拉角）: ";
			//std::cout << "(" << flangeCenterRpyOnBase.rx * 180 / M_PI << ", " << flangeCenterRpyOnBase.ry * 180 / M_PI << ", " << flangeCenterRpyOnBase.rz * 180 / M_PI << ")";
			//std::cout << std::endl;

			//将数据存入txt
			/*onfile << flangeCenterPosOnBase.x << " " << flangeCenterPosOnBase.y << " " << flangeCenterPosOnBase.z
				<< flangeCenterRpyOnBase.rx * 180 / M_PI << " " << flangeCenterRpyOnBase.ry * 180 / M_PI << " " << flangeCenterRpyOnBase.rz * 180 / M_PI << std::endl;*/

			double startPointJointAngle[aubo_robot_namespace::ARM_DOF] = { 0.0 / 180.0*M_PI,  0.0 / 180.0*M_PI,  0.0 / 180.0*M_PI, 0.0 / 180.0*M_PI, 0.0 / 180.0*M_PI,0.0 / 180.0*M_PI };
			aubo_robot_namespace::wayPoint_S wayPointi;
			std::vector<double> jointAngle;

			if (RS_SUCC == rs_inverse_kin(rshd, startPointJointAngle, &flangeCenterPosOnBase, &flangeCenterOriOnBase, &wayPointi))//逆解求关节角
			{
				jointAngle.push_back(wayPointi.jointpos[0]);
				jointAngle.push_back(wayPointi.jointpos[1]);
				jointAngle.push_back(wayPointi.jointpos[2]);
				jointAngle.push_back(wayPointi.jointpos[3]);
				jointAngle.push_back(wayPointi.jointpos[4]);
				jointAngle.push_back(wayPointi.jointpos[5]);
				wayPoint.push_back(jointAngle);
				jointAngle.clear();
				std::cout << "ik succ" << std::endl;

				for (int j = 0; j < 6; j++)
				{
					startPointJointAngle[j] = wayPointi.jointpos[j];
				}
			}
			else
			{
				std::cerr << "ik failed" << std::endl;
			}
		}
	}
		onfile.close();

		return 0;
}
