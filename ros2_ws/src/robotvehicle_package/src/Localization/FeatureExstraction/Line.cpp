//
// Created by robot1 on 9/23/23.
//

#include <valarray>
#include "Line.h"

Line::Line() {
    std::vector<PointRectForm> points(300);
    this->points = points;
    reset();
}

void Line::addRecPointFromPolar(double theta, double d) {
    addRecPoint(getXFromPolarForm(theta,d), getYFromPolarForm(theta,d));
}

void Line::addRecPoint(double x, double y) {
    points.at(pointsNum).set(x,y);
    pointsNum++;
}

void Line::reset() {
    pointsNum=0;
}
double Line::perpendicularDistance(PointPolarForm *point) {
    updateOriginLineNormal();
    double d;
    double xp = getXFromPolarForm(point->getAngle(),point->getDistance());
    double yp = getYFromPolarForm(point->getAngle(),point->getDistance());
    double theta_p = atan2(yp,xp);
    double p       = sqrt(pow(xp,2) + pow(yp,2));
    d = p*cos( theta_p - alfa ) - r;
    return abs(d);
}

void Line::updateOriginLineNormal() {
    updateSlopeForm();
    double fi = atan(m);
    if( fi>0 )
        alfa = fi-M_PI/2;
    else
        alfa = fi+M_PI/2;
    r    = b*sin(alfa);
}

void Line::updateSlopeForm() {
    int lastPointIndex = pointsNum-1;
    double y2 = points.at(lastPointIndex).getY();
    double x2 = points.at(lastPointIndex).getX();
    m = (y2-points.begin()->getY())/
        (x2-points.begin()->getX()) ;
    b = y2 - m*x2;
}

double Line::getXFromPolarForm(double theta,double d){
    return d*cos(theta);
}

double Line::getYFromPolarForm(double theta,double d){
    return d*sin(theta);
}

double Line::getAlfa() const {
    return alfa;
}

void Line::setAlfa(double alfa) {
    Line::alfa = alfa;
}

double Line::getR() const {
    return r;
}

void Line::setR(double r) {
    Line::r = r;
}


