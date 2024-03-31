//
// Created by robot1 on 3/19/24.
//

#include "Pose.h"

Pose::Pose() {}

void Pose::update(double x, double y, double theta) {
    this->x = x;
    this->y = y;
    this->theta =theta;
}

double Pose::getX() const {
    return x;
}

double Pose::getY() const {
    return y;
}

double Pose::getTheta() const {
    return theta;
}

