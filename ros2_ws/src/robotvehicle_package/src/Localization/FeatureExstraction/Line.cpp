//
// Created by robot1 on 9/23/23.
//

#include <valarray>
#include "Line.h"

Line::Line() {
    reset();
}

void Line::addRecPointFromPolar(double theta, double d) {
    points[pointsNum]->set(d*cos(theta),d*sin(theta));
    pointsNum++;
}


void Line::reset() {
    pointsNum=0;
}

double Line::perpendicularDistance(PointPolarForm &form) {

}



