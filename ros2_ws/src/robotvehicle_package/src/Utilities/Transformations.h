//
// Created by robot1 on 7/3/24.
//

#ifndef ROBOTVEHICLE_PACKAGE_TRANSFORMATIONS_H
#define ROBOTVEHICLE_PACKAGE_TRANSFORMATIONS_H

#include "Pose.h"

class Transformations {
public:
    Transformations() = delete;
    static void polarPointToCartesian(double &xp,double &yp,Pose * currentPose,double alfa,double p);
};


#endif //ROBOTVEHICLE_PACKAGE_TRANSFORMATIONS_H
