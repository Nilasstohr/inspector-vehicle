//
// Created by robotcentral on 1/5/25.
//

#include "CirclePoints.h"

CirclePoints::CirclePoints(const int points):
xc(0), yc(0), angle(0),xCir(0),yCir(0),startAngle(0),angleSize(2*M_PI) {
    dt = (2 * M_PI) / points;
}

void CirclePoints::update() {
    reset();
    angle=startAngle;
    while(angle<=startAngle+angleSize) {
        xCir=r*cos(angle) +xc;
        yCir=r*sin(angle) +yc;
        addPoint(xCir,yCir);
        angle+=dt;
    }
}

void CirclePoints::setCenterPoint(double r, double xc, double yc, double startAngle, double angleSize) {
    this->r= r;
    this->xc = xc;
    this->yc = yc;
    this->startAngle=startAngle;
    this->angleSize=angleSize;
}





