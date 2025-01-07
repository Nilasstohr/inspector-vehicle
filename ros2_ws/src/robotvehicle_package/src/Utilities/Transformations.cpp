//
// Created by robot1 on 7/3/24.
//

#include <valarray>
#include "Transformations.h"

void Transformations::polarPointToCartesian(double &xp, double &yp, Pose *currentPose, double alfa, double p) {
    xp = currentPose->getX() + p*cos(alfa+currentPose->getTheta());
    yp = currentPose->getY() + p*sin(alfa+currentPose->getTheta());
}

void Transformations::recPointToGlobalReferenceFrame(double &xG, double &yG,const double xR,const double yR, const Pose *currentPose) {
    xG = cos(-currentPose->getTheta())*xR + sin(-currentPose->getTheta())*yR  +currentPose->getX();
    yG = -sin(-currentPose->getTheta())*xR + cos(-currentPose->getTheta())*yR +currentPose->getY();
}