//
// Created by robot1 on 7/4/24.
//

#include "LinePoints.h"

#include <cmath>

LinePoints::LinePoints(const int h): h(h) {
    line = Line();
    LinePoints::reset();
}

void LinePoints::update() {
    line.updateSlopeForm();
    PointBase::reset();
    double lastPointX = line.getLastPoint()->getX();
    double x=line.getFirstPoint()->getX();
    double y;
    double dx =h*cos(atan(line.getM()));
    while(true){
        x+=dx;
        if(x > lastPointX){
            break;
        }
        y= line.getM()*x+line.getB();
        addPoint(x,y);
    }
}

void LinePoints::reset() {
    PointBase::reset();
    line.reset();
}

void LinePoints::setFromLine(Line *line) {
    setFromLine(line->getFirstPoint()->getX(),line->getLastPoint()->getY(),
        line->getFirstPoint()->getX(),line->getLastPoint()->getY());
}

void LinePoints::setFromLine(double x1, double y1,double x2,double y2) {
    reset();
    line.addRecPoint(x1,y1);
    line.addRecPoint(x2,y2);
    update();
}

void LinePoints::addLinePoint(double xp, double yp) {
    line.addRecPoint(xp,yp);
}
