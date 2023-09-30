//
// Created by robot1 on 8/10/23.
//

#include "PointPolarForm.h"

PointPolarForm::PointPolarForm(double angle, double distance): angle(angle), distance(distance) {}

double PointPolarForm::getAngle() const {
    return angle;
}

double PointPolarForm::getDistance() const {
    return distance;
}
