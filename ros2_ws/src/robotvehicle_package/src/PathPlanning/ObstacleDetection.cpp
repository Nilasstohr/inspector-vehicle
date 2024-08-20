//
// Created by robot1 on 8/6/24.
//

#include <iostream>
#include "ObstacleDetection.h"

ObstacleDetection::ObstacleDetection():obstacleLocationType(ObstacleLocationType::NONE) {}

void ObstacleDetection::update(double distance,double angle) {
    if(obstacleLocationType==ObstacleLocationType::NONE) {
        if (distance <= (double(CONFIG_ROBOT_DIAMETER / 2) + CONFIG_SAFETY_DISTANCE)) {
            //std::cout << distance << " " << angle;
            if (angle > M_PI / 2 || angle < -M_PI / 2) {
                //std::cout << " BACK" << std::endl;
                obstacleLocationType = ObstacleLocationType::BACK;
            } else {
                //std::cout << " FRONT" << std::endl;
                obstacleLocationType = ObstacleLocationType::FRONT;
            }
        } else {
            obstacleLocationType = ObstacleLocationType::NONE;
        }
    }
}

bool ObstacleDetection::isObstacleTooClose() {
    return obstacleLocationType!=ObstacleLocationType::NONE;
}

ObstacleLocationType ObstacleDetection::getObstacleLocation() {
    return obstacleLocationType;
}

void ObstacleDetection::clear() {
    obstacleLocationType=ObstacleLocationType::NONE;
}
