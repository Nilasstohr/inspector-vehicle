//
// Created by robot1 on 7/4/24.
//

#include "LinePoints.h"

LinePoints::LinePoints(int h): h(h) {
    points = new std::vector<PathPoint>(CONFIG_MAX_NUMBER_OF_LINE_POINTS);
}

void LinePoints::update() {
    updateSlopeForm();
    pointsNum=0;
    double lastPointX = getLastPoint()->getX();
    double x=getFirstPoint()->getX();
    double y;
    double dx =h*cos(atan(getM()));
    while(true){
        x+=dx;
        if(x > lastPointX){
            break;
        }
        y= getM()*x+getB();
        points->at(pointsNum).set(x,y);
        pointsNum++;
    }
}


int LinePoints::getPointSize() {
    return pointsNum;
}

void LinePoints::getByIndex(int &x, int &y, int i) {
    x = floor(points->at(i).getX());
    y = floor(points->at(i).getY());
}
