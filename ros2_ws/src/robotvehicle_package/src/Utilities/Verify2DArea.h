//
// Created by robot1 on 7/25/24.
//

#ifndef ROBOTVEHICLE_PACKAGE_VERIFY2DAREA_H
#define ROBOTVEHICLE_PACKAGE_VERIFY2DAREA_H


#include "Pose.h"

class Verify2DArea {
public:
    static bool isPointWithinSquareArea(Pose *pose, int x, int y, double size);

    static double distanceBetweenPoints(double x1, double y1, int x2, int y2);
};


#endif //ROBOTVEHICLE_PACKAGE_VERIFY2DAREA_H
