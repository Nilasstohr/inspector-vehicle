//
// Created by robot1 on 2/18/24.
//

#include "NavigationPoint.h"

NavigationPoint::NavigationPoint(double x, double y, double theta):
x(x),y(y),theta(theta) {}

double NavigationPoint::getX() const {
    return x;
}

double NavigationPoint::getY() const {
    return y;
}

double NavigationPoint::getTheta() const {
    return theta;
}
