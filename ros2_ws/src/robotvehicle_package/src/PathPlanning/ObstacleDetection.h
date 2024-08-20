//
// Created by robot1 on 8/6/24.
//

#ifndef ROBOTVEHICLE_PACKAGE_OBSTACLEDETECTION_H
#define ROBOTVEHICLE_PACKAGE_OBSTACLEDETECTION_H

#include "ObstacleLocation.h"
#include "../Configurations.h"
#include <cmath>

class ObstacleDetection {
public:
    ObstacleDetection();
    void update(double distance, double angle);
    bool isObstacleTooClose();
    ObstacleLocationType getObstacleLocation();

    void clear();

private:
    ObstacleLocationType obstacleLocationType;

};


#endif //ROBOTVEHICLE_PACKAGE_OBSTACLEDETECTION_H
