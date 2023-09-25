//
// Created by robot1 on 8/10/23.
//

#include "PointPolarForm.h"

PointPolarForm::PointPolarForm(float angle, float distance): angle(angle), distance(distance) {}

float PointPolarForm::getAngle() const {
    return angle;
}

float PointPolarForm::getDistance() const {
    return distance;
}
