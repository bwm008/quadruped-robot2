// The MIT License

// Copyright (c) 2022
// Robot Motion and Vision Laboratory at East China Normal University
// Contact:tophill.robotics@gmail.com

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef QR_GAIT_PARAM_RECEIVER_H
#define QR_GAIT_PARAM_RECEIVER_H

#include <iostream>
#include <string>

#include <eigen3/Eigen/Dense>
#include <ros/ros.h>
#include <unitree_legged_msgs/GaitParameter.h>

// get gait param from topic for gait update
class QrGaitParamReceiver {
    public:
        QrGaitParamReceiver(ros::NodeHandle &nhIn);

        ~QrGaitParamReceiver() = default;

        void GaitParamCallback(const unitree_legged_msgs::GaitParameter::ConstPtr &input);

        Eigen::Matrix<float, 4, 1> GetStanceDuration();
       
        Eigen::Matrix<float, 4, 1> GetDutyFactor();

        Eigen::Matrix<int, 4, 1> GetInitialLegState();

        Eigen::Matrix<float, 4, 1> GetInitialLegPhase();

        float GetContactDetectionPhaseThreshold();

        bool GetFlag();

        std::string GetGaitName();

        void SetFlag();


        ros::NodeHandle &nh;
        ros::Subscriber gaitParamSub;
        std::string gaitParamTopic = "/gaitParam";

    private:
        // Whether the flag parameter has changed
        bool flag = false;
        std::string gaitName;
        Eigen::Matrix<float, 4, 1> stanceDuration;
        Eigen::Matrix<float, 4, 1> dutyFactor;
        Eigen::Matrix<int, 4, 1> initialLegState;
        Eigen::Matrix<float, 4, 1> initialLegPhase;
        float contactDetectionPhaseThreshold=0.1f;
};
#endif //QR_GAIT_PARAM_RECEIVER_H