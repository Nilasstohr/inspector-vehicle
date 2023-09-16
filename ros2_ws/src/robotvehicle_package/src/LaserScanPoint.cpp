//
// Created by robot1 on 8/10/23.
//

#include "LaserScanPoint.h"

LaserScanPoint::LaserScanPoint(float angle, float distance): angle(angle), distance(distance) {}

float LaserScanPoint::getAngle() const {
    return angle;
}

float LaserScanPoint::getDistance() const {
    return distance;
}
