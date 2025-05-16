#include "robot_control.h"
//构造函数
int capture_index = 0;
RobotController::RobotController(){}

//aubo机器人TCP连接
void RobotController::whenRobotConnect()
{
    aubo->moveToThread(QThread::currentThread());
    bool result_flag = aubo->aubo_connect(g_rshd, ROBOT_ADDR, ROBOT_PORT);
    if(result_flag)
        emit appendMessageLog(QString("与aubo机器人成功连接"));
    else
        emit appendMessageLog(QString("aubo机器人连接出错"));
}

//aubo机器人断开连接
void RobotController::whenRobotDisconnect()
{
    bool result_flag = aubo->aubo_disconnect(g_rshd);
    if (result_flag)
        emit appendMessageLog(QString("与aubo机器人成功断开连接"));
    else
        emit appendMessageLog(QString("aubo机器人断开连接出错"));
}
std::string RobotController::num2fixed_str(int i)
{
    char ss[10];
    sprintf(ss, "%02d", i);
    return ss;
}
void RobotController::whenGetCurrentWaypoint()
{
    //当前路点定义
    aubo_robot_namespace::wayPoint_S wayPoint;

    //获取当前路点
    if (RS_SUCC == rs_get_current_waypoint(g_rshd, &wayPoint))
    {
        emit appendMessageLog(QString("当前路点坐标为："));

        //输出6个关节角（角度方式）
        vector<double> joints_angle;
        joints_angle.push_back(wayPoint.jointpos[0] * 180.0 / M_PI);
        joints_angle.push_back(wayPoint.jointpos[1] * 180.0 / M_PI);
        joints_angle.push_back(wayPoint.jointpos[2] * 180.0 / M_PI);
        joints_angle.push_back(wayPoint.jointpos[3] * 180.0 / M_PI);
        joints_angle.push_back(wayPoint.jointpos[4] * 180.0 / M_PI);
        joints_angle.push_back(wayPoint.jointpos[5] * 180.0 / M_PI);
        QString joint_string1 = QString(QStringLiteral("关节角(deg)：\r\n%1,%2,%3,%4,%5,%6")).arg(QString::number(joints_angle[0], 'f', 6), QString::number(joints_angle[1], 'f', 6), QString::number(joints_angle[2], 'f', 6), QString::number(joints_angle[3], 'f', 6), QString::number(joints_angle[4],'f',6), QString::number(joints_angle[5], 'f', 6));
        emit appendMessageLog(joint_string1);

        //输出末端法兰坐标 x,y,z,rx,ry,rz  cartPos
        vector<double> robot_flange_coordinate;
        robot_flange_coordinate.push_back(wayPoint.cartPos.position.x);
        robot_flange_coordinate.push_back(wayPoint.cartPos.position.y);
        robot_flange_coordinate.push_back(wayPoint.cartPos.position.z);
        double roll, pitch, yaw;
        //四元数转欧拉角
        toEulerAngle(wayPoint.orientation.x, wayPoint.orientation.y, wayPoint.orientation.z, wayPoint.orientation.w, roll, pitch, yaw);
        robot_flange_coordinate.push_back(roll * 180.0 / M_PI);
        robot_flange_coordinate.push_back(pitch * 180.0 / M_PI);
        robot_flange_coordinate.push_back(yaw * 180.0 / M_PI);
        QString joint_string2 = QString(QStringLiteral("x,y,z,rx,ry,rz：\r\n%1,%2,%3,%4,%5,%6")).arg(QString::number(robot_flange_coordinate[0], 'f', 6), QString::number(robot_flange_coordinate[1], 'f', 6), QString::number(robot_flange_coordinate[2], 'f', 6), QString::number(robot_flange_coordinate[3], 'f', 6), QString::number(robot_flange_coordinate[4], 'f', 6), QString::number(robot_flange_coordinate[5], 'f', 6));
        emit appendMessageLog(joint_string2);


        //输出末端法兰坐标，到xml文件
        string output_file ="./data/calib/camera" + std::to_string(cameraIndex)+ "/pos/robotpos" + num2fixed_str(capture_index++) + ".xml";  //输出xml文件名
        cv::FileStorage fs(output_file, cv::FileStorage::WRITE);
        fs << "Position0" << robot_flange_coordinate[0];
        fs << "Position1" << robot_flange_coordinate[1];
        fs << "Position2" << robot_flange_coordinate[2];
        fs << "Position3" << robot_flange_coordinate[3];
        fs << "Position4" << robot_flange_coordinate[4];
        fs << "Position5" << robot_flange_coordinate[5];
    }
    else
       emit appendMessageLog(QString("未能获取当前路点坐标"));
}

