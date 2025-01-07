//
// Created by robotcentral on 1/5/25.
//

#include "PointBase.h"

PointBase::PointBase(): pointsNum(0) {
    points = new std::vector<PathPoint>(CONFIG_MAX_NUMBER_OF_LINE_POINTS);
    PointBase::reset();
}

int PointBase::size() {
    return pointsNum;
}

void PointBase::getByIndex(int &x, int &y, int i) {
    x = std::floor(points->at(i).getX());
    y = std::floor(points->at(i).getY());
}

void PointBase::reset() {
    pointsNum=0;
}

int PointBase::getPointsNum() {
    return pointsNum;
}


void PointBase::addPointsNum(double x, double y) {
    points->at(getPointsNum()).set(x,y);
    pointsNum++;
}

void PointBase::addPoint(double x,double y) {
    points->at(pointsNum).set(x,y);
    pointsNum++;
}
