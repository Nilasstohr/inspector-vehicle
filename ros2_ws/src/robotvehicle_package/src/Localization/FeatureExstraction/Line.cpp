//
// Created by robot1 on 9/23/23.
//

#include <valarray>
#include "Line.h"

Line::Line() {
    reset();
}

void Line::addRecPointFromPolar(double theta, double d) {
    points[pointsNum]->set(getXFromPolarForm(theta,d), getYFromPolarForm(theta,d));
    pointsNum++;
}
void Line::reset() {
    pointsNum=0;
}
double Line::perpendicularDistance(PointPolarForm point) {
    updateOriginLineNormal();
    double d;
    double xp = getXFromPolarForm(point.getAngle(),point.getDistance());
    double yp = getYFromPolarForm(point.getAngle(),point.getDistance());
    double theta_p = atan2(yp,xp);
    double p       = sqrt(pow(xp,2) + pow(yp,2));
    d = p*cos( theta_p - alfa ) - r;
    return abs(d);
}

void Line::updateOriginLineNormal() {
    updateSlopeForm();
    double fi = atan(m);
    if( fi>0 )
        alfa = fi-PI/2;
    else
        alfa = fi+PI/2;
    r    = b*sin(alfa);
}

void Line::updateSlopeForm() {
    double y2 = points[pointsNum]->getY();
    double x2 = points[pointsNum]->getX();
    m = (y2-points[0]->getY())/
        (x2-points[0]->getX()) ;
    b = y2 - m*x2;
}

double Line::getXFromPolarForm(double theta,double d){
    return d*cos(theta);
}

double Line::getYFromPolarForm(double theta,double d){
    return d*sin(theta);
}
