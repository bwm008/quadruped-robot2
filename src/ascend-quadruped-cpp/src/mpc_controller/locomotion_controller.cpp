/* 
* Copyright (c) Huawei Technologies Co., Ltd. 2021-2022. All rights reserved.        
* Description: a interface of robot locomotion controller.
* Author: Zhu Yijie
* Create: 2021-10-25
* Notes: xx
* Modify: init the file. @ Zhu Yijie
*/

#include "mpc_controller/locomotion_controller.h"
namespace Quadruped {
    qrLocomotionController::qrLocomotionController(Robot *robotIn,
                                               qrGaitGenerator *gaitGeneratorIn,
                                               RobotEstimator *stateEstimatorIn,
                                               qrGroundSurfaceEstimator *groundEstimatorIn,
                                               qrComPlanner *comPlannerIn,
                                               qrPosePlanner *posePlannerIn,
                                               qrSwingLegController *swingLegControllerIn,
                                               qrStanceLegController *stanceLegControllerIn)
    :
        robot(robotIn), gaitGenerator(gaitGeneratorIn), stateEstimator(stateEstimatorIn), groundEstimator(groundEstimatorIn), comPlanner(comPlannerIn),
        posePlanner(posePlannerIn), swingLegController(swingLegControllerIn), stanceLegController(stanceLegControllerIn)
    {
        resetTime = robot->GetTimeSinceReset();
        timeSinceReset = 0.;
    }

    void qrLocomotionController::Reset()
    {
        resetTime = robot->GetTimeSinceReset();
        timeSinceReset = 0.;
        gaitGenerator->Reset(timeSinceReset);
        stateEstimator->Reset(timeSinceReset);
        groundEstimator->Reset(timeSinceReset);
        comPlanner->Reset(timeSinceReset);
        posePlanner->Reset(timeSinceReset);
        swingLegController->Reset(timeSinceReset);
        stanceLegController->Reset(timeSinceReset);
    }

    void qrLocomotionController::Update()
    {
        if (!robot->stop) { // not stop = (swingSemaphore > 0) or  (swingSemaphore=0 but not switchToSwing)
            timeSinceReset = robot->GetTimeSinceReset() - resetTime;
        }
        
        // std::cout << "-------locomotion time -------- " << timeSinceReset << std::endl;
        gaitGenerator->Update(timeSinceReset);
        groundEstimator->Update(timeSinceReset);
        stateEstimator->Update(timeSinceReset);
        swingLegController->Update(timeSinceReset);
        stanceLegController->Update(robot->GetTimeSinceReset() - resetTime);
    }

    std::tuple<std::vector<MotorCommand>, Eigen::Matrix<float, 3, 4>> qrLocomotionController::GetAction()
    {
        action.clear();
        // Returns the control ouputs (e.g. positions/torques) for all motors. type: map
        auto swingAction = swingLegController->GetAction();
        auto [stanceAction, qpSol] = stanceLegController->GetAction(); // map<int, MotorCommand>
        std::vector<MotorCommand> action;
        // copy motors' actions from subcontrollers to output variable.         
        for (int joint_id = 0; joint_id < RobotConfig::numMotors; ++joint_id) {
            auto it = swingAction.find(joint_id);
            if (it != swingAction.end()) {
                action.push_back(it->second);
            } else {
                action.push_back(stanceAction[joint_id]);
            }
        }
        return {action, qpSol};
    }
} // namespace Quadruped
