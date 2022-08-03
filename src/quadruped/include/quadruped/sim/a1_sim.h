/*
* Copyright (c) Huawei Technologies Co., Ltd. 2021-2022. All rights reserved.
* Description: Swing Leg Controller
* Author: Xie Ming Cheng & Zhao Yao
* Create: 2021-12-16
* Notes: xx
* Modify: add head comment and add some function comments and delete some test functions. @ xie_mingcheng 2021.11.22
*/

#ifndef ROBOTS_A1_SIM_H
#define ROBOTS_A1_SIM_H

#include <ros/ros.h>
#include <sensor_msgs/Imu.h>
#include "geometry_msgs/WrenchStamped.h"
#include "unitree_legged_msgs/LowCmd.h"
#include "unitree_legged_msgs/LowState.h"
#include "unitree_legged_msgs/MotorCmd.h"
#include "unitree_legged_msgs/MotorState.h"
#include "robots/qr_robot.h"

/**
 * @brief a1 robot class in simulation.
 */
class qrRobotA1Sim : public qrRobot {
public:
    qrRobotA1Sim(ros::NodeHandle &nhIn, std::string configFilePath);

    ~qrRobotA1Sim() = default;

    void SendCommand(const std::array<float, 60> motorcmd);

    void ImuCallback(const sensor_msgs::Imu &msg);

    void FRhipCallback(const unitree_legged_msgs::MotorState &msg);

    void FRthighCallback(const unitree_legged_msgs::MotorState &msg);

    void FRcalfCallback(const unitree_legged_msgs::MotorState &msg);

    void FLhipCallback(const unitree_legged_msgs::MotorState &msg);

    void FLthighCallback(const unitree_legged_msgs::MotorState &msg);

    void FLcalfCallback(const unitree_legged_msgs::MotorState &msg);

    void RRhipCallback(const unitree_legged_msgs::MotorState &msg);

    void RRthighCallback(const unitree_legged_msgs::MotorState &msg);

    void RRcalfCallback(const unitree_legged_msgs::MotorState &msg);

    void RLhipCallback(const unitree_legged_msgs::MotorState &msg);

    void RLthighCallback(const unitree_legged_msgs::MotorState &msg);

    void RLcalfCallback(const unitree_legged_msgs::MotorState &msg);

    void FRfootCallback(const geometry_msgs::WrenchStamped &msg);

    void FLfootCallback(const geometry_msgs::WrenchStamped &msg);

    void RRfootCallback(const geometry_msgs::WrenchStamped &msg);

    void RLfootCallback(const geometry_msgs::WrenchStamped &msg);

    /**
     * @brief override the method in qrRobot
     */
    void ReceiveObservation() override;

    /**
     * @brief override the method in qrRobot
     */
    void ApplyAction(const Eigen::MatrixXf &motorCommands, MotorMode motorControlMode) override;

    /**
     * @brief override the method in qrRobot
     */
    void Step(const Eigen::MatrixXf &action, MotorMode motorControlMode) override;

    ros::NodeHandle & nh;

    unitree_legged_msgs::LowCmd lowCmd;
    //unitree_legged_msgs::LowState lowState;

    /**
     * @brief ROS joint commander publisher
     */
    ros::Publisher jointCmdPub[12];

    /**
     * @brief ROS joint state subscriber
     */
    ros::Subscriber jointStateSub[12];

    /**
     * @brief ROS foot force subscriber
     */
    ros::Subscriber footForceSub[4];

    /**
     * @brief ROS IMU state subscriber
     */
    ros::Subscriber imuSub;
};

#endif //ROBOTS_A1_SIM_H
