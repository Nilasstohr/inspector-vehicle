//
// Created by robot1 on 2/18/24.
//

#include "NavigationPath.h"

#include <stdexcept>

#include "NavigationExceptionNoPathPoints.h"
#include "NavigationExceptionPathPointAlreadyCompleted.h"

NavigationPath::NavigationPath(): pathIndex(0) {
    path = new std::vector<NavigationPoint>;
}

void NavigationPath::addPathPoint(double x, double y, double theta) {
    path->push_back(NavigationPoint(x,y,theta));
}

std::vector<NavigationPoint> *NavigationPath::getPath() const {
    return path;
}

void NavigationPath::clear() {
    path->clear();
}

NavigationPoint * NavigationPath::getCurrentGoToPoint() const {
    if(path->empty()){
        throw NavigationExceptionNoPathPoints();
    }
    return &path->at(pathIndex);
}

void NavigationPath::setNextGoToPoint() {
    if(pathIndex + 1 == path->size()){
        throw NavigationExceptionPathPointAlreadyCompleted();
    }
    pathIndex++;
}

bool NavigationPath::isNextPointAvailable() const {
    return  pathIndex + 1 < path->size();
}

