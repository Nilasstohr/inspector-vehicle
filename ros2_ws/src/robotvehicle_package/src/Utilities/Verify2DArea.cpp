//
// Created by robot1 on 7/25/24.
//

#include <valarray>
#include "Verify2DArea.h"

bool Verify2DArea::isPointWithinSquareArea(const double xp, const double yp,const  int x, const int y, const double size) {
    double yMax = y + double(size/2);
    double yMin = y - double(size/2);
    double xMax = x + double(size/2);
    double xMin = x - double(size/2);
    if(xp<xMax && xp>xMin && yp<yMax && yp>yMin){
        return true;
    }
    return false;
}

double Verify2DArea::distanceBetweenPoints(double x1, double y1, int x2, int y2) {
    return abs(sqrt( pow(x2-x1,2) + pow(y2-y1,2) ));
}
