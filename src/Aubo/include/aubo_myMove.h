#pragma once

#include <string>
#include "rsdef.h"
#include "stdafx.h"

//相对轴动 输入：句柄，运动目标（关节角）
bool my_moveJ_relative(RSHD rshd, double targePos[6]);

//绝对轴动 输入：句柄，运动目标（关节角）
bool my_moveJ(RSHD rshd, double targePos[6]);

//直线运动 输入：句柄，位置，姿态（RPY角）
bool my_moveL(RSHD rshd,Pos pos, Ori ori);

//轨迹运动 输入：句柄，路点（6关节角）
bool my_moveP(RSHD rshd, std::vector<std::vector<double>> wayPoint);

//旋转运动 输入：句柄，路点（位置和PRY角）,旋转轴，旋转角度（°）
bool my_moveR(RSHD rshd, double targePos[6], Move_Rotate_Axis axis, double rotateAngle);

//路点转换 输入：句柄，给定路径点（位置+RPY角）存放文件名 输出：路点（6关节角）
bool positionToJoint(RSHD rshd,std::string fileName,std::vector<std::vector<double>> &wayPoint);

//含工具路点转换 输入：句柄，给定路径点（位置+RPY角）存放文件名 输出：路点（6关节角）
bool positionToJointTool(RSHD rshd, std::string fileName, ToolInEndDesc tool, std::vector<std::vector<double>> &wayPoint);
