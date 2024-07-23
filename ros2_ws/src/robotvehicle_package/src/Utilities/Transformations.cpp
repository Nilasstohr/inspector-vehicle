//
// Created by robot1 on 7/3/24.
//

#include <valarray>
#include "Transformations.h"

void Transformations::polarPointToCartesian(double &xp, double &yp, Pose *currentPose, double alfa, double p) {
    xp = currentPose->getX() + p*cos(alfa+currentPose->getTheta());
    yp = currentPose->getY() + p*sin(alfa+currentPose->getTheta());
}
