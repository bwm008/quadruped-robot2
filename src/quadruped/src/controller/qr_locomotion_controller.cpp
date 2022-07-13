// The MIT License

// Copyright (c) 2022 
// Robot Motion and Vision Laboratory at East China Normal University
// Contact: tophill.robotics@gmail.com

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

#include "controller/qr_locomotion_controller.h"

qrLocomotionController::qrLocomotionController(qrRobot *robotIn) : robot(robotIn)
{
    this->resetTime = robot->GetTimeSinceReset();
    this->timeSinceReset = 0.f;
    this->robotConfig = this->robot->GetRobotConfig();
    this->robotState = this->robot->GetRobotState();
    this->desiredSpeed = {0.f, 0.f, 0.f};
    this->desiredTwistingSpeed = 0.f;
}

void qrLocomotionController::Initialization(std::string path)
{
    this->gaitGenerator = new qrGaitGenerator(this->robot, path + "gait_config.yaml");
    std::cout << "init gaitGenerator finish\n" << std::endl;

    this->groundEstimator = new qrGroundSurfaceEstimator(this->robot, path + "terrain.yaml");
    std::cout << "init groundEsitmator finish\n" << std::endl;

    this->velocityEstimator = new qrRobotVelocityEstimator(this->robot);
    std::cout << "init robotEstimator finish\n" << std::endl;
     
    this->comPlanner = new qrComPlanner(this->robot, gaitGenerator);
    std::cout << "init comPlanner finish\n" << std::endl;

    this->footholdPlanner = new qrFootholdPlanner(this->robot, this->groundEstimator);
    std::cout << "init footholdPlanner finish\n" << std::endl;

    this->swingLegController = new qrSwingLegController(this->robot,
                                                        this->gaitGenerator,
                                                        this->velocityEstimator,
                                                        this->groundEstimator,
                                                        this->desiredSpeed,
                                                        this->desiredTwistingSpeed,
                                                        0.01f,
                                                        path + "swing_leg_controller.yaml");
    std::cout << "init swingLegController finish\n" << std::endl;

    this->stanceLegController = new qrStanceLegController(this->robot,
                                                          this->gaitGenerator,
                                                          this->velocityEstimator,
                                                          this->groundEstimator,
                                                          this->comPlanner,
                                                          this->footholdPlanner,
                                                          this->desiredSpeed,
                                                          this->desiredTwistingSpeed,
                                                          path + "stance_leg_controller.yaml");

    std::cout << "init stanceLegController finish\n" << std::endl;
}

void qrLocomotionController::Reset()
{
    this->resetTime = robot->GetTimeSinceReset();
    this->timeSinceReset = 0.;
    this->gaitGenerator->Reset(this->timeSinceReset);
    this->groundEstimator->Reset();
    this->comPlanner->Reset(this->timeSinceReset);
    this->swingLegController->Reset();
    this->stanceLegController->Reset(this->timeSinceReset);
}

void qrLocomotionController::Update()
{
    if (!this->robot->IsStop()) { // not stop = (swingSemaphore > 0) or  (swingSemaphore=0 but not switchToSwing)
        this->timeSinceReset = this->robot->GetTimeSinceReset() - resetTime;
    }
    
    // std::cout << "-------locomotion time -------- " << timeSinceReset << std::endl;
    this->gaitGenerator->Update(this->timeSinceReset);
    
    // bool switchToSwing = false;
    // if (this->robotConfig->controlMode==LocomotionMode::WALK_LOCOMOTION) {
    //     // for walk mode
    //     const Vec4<int>& newLegState = this->gaitGenerator->legState;
    //     const Vec4<int>& curLegState = this->gaitGenerator->curLegState;
    //     for(int legId =0; legId<4; legId++) {
    //         if((newLegState(legId) == LegState::SWING && curLegState(legId) == LegState::STANCE)
    //             || newLegState(legId) == LegState::USERDEFINED_SWING) {
    //                 switchToSwing=true;
    //                 break;
    //             }
    //     }    
    //     if (switchToSwing) {
    //         if (this->swingSemaphore > 0) {
    //             this->swingSemaphore--;
    //         } else if (this->swingSemaphore == 0) 
    //         {
    //             this->swingSemaphore--;
    //             this->stopTick = this->robot->GetTimeSinceReset();
    //             this->robot->stop = true; // when stop, all legs must stay stance if phase gap=0.25.
    //             printf("stop robot!============\n");
    //             posePlanner->ResetBasePose(this->timeSinceReset);
    //         } else { // swingSemaphore == -1
    //             ;
    //         }
    //     }
    // }
    
    //
    this->groundEstimator->Update();
    // switch (this->robotConfig->controlMode) {
    //     case LocomotionMode::POSITION_LOCOMOTION: {
    //         this->comPlanner->Update(this->timeSinceReset);
    //     } break; 
    //     case LocomotionMode::WALK_LOCOMOTION: {
    //         if (switchToSwing && this->swingSemaphore >=0) {
    //             // posePlanner->Update(this->timeSinceReset);
    //             printf("update pose plan finish\n");
    //         }
    //     } break;
    //     default: break;
    // }
    this->swingLegController->Update();
    this->stanceLegController->Update(robot->GetTimeSinceReset() - this->timeSinceReset);
}

std::tuple<std::vector<qrMotorCmd>, Mat3x4<float>> qrLocomotionController::GetAction()
{
    this->action.clear();
    // Returns the control ouputs (e.g. positions/torques) for all motors. type: map
    auto swingAction = this->swingLegController->GetAction();
    auto [stanceAction, qpSol] = this->stanceLegController->GetAction(); // map<int, MotorCommand>
    // copy motors' actions from subcontrollers to output variable.         
    for (int joint_id = 0; joint_id < qrRobotConfig::numMotor; ++joint_id) {
        auto it = swingAction.find(joint_id);
        if (it != swingAction.end()) {
            this->action.push_back(it->second);
        } else {
            this->action.push_back(stanceAction[joint_id]);
        }
    }
    return {this->action, qpSol};
}

std::tuple<std::vector<qrMotorCmd>, Mat3x4<float>> qrLocomotionController::GetFakeAction()
{
    this->action.clear();
    Mat3x4<float> qpSol = Mat3x4<float>::Zero();
    std::vector<qrMotorCmd> action;
    // copy motors' actions from subcontrollers to output variable.         
    for (int joint_id = 0; joint_id < qrRobotConfig::numMotor; ++joint_id) {
        this->action.push_back({0,0,0,0,0});
    }
    return {action, qpSol};
}

void qrLocomotionController::ForwardOne()
{
    this->robot->SetStop(false);
    this->stop = false; 
    this->swingSemaphore++;
    if (this->swingSemaphore==0) {
        this->swingSemaphore++;
        this->resetTime += (this->robot->GetTimeSinceReset() - this->stopTick);
    }
}
