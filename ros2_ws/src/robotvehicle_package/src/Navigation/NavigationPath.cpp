//
// Created by robot1 on 2/18/24.
//

#include "NavigationPath.h"

#include <stdexcept>

#include "NavigationExceptionNoPathPoints.h"
#include "NavigationExceptionPathPointAlreadyCompleted.h"

NavigationPath::NavigationPath(): pathIndex(0) {
}

void NavigationPath::addPathPoint(double x, double y, double theta) {
    path.push_back(NavigationPoint(x,y,theta));
}

const std::vector<NavigationPoint> & NavigationPath::getPath() const {
    return path;
}

void NavigationPath::overrideBack(const  int x, const int y) {
    if(path.empty()){
        throw NavigationExceptionNoPathPoints();
    }
    path.back().override(x,y);
}

bool NavigationPath::isEmpty() const {
    return path.empty();
}

unsigned int NavigationPath::size() const {
    return path.size();
}

void NavigationPath::clear() {
    path.clear();
}

NavigationPoint const & NavigationPath::getCurrentGoToPoint() const {
    if(path.empty()){
        throw NavigationExceptionNoPathPoints();
    }
    return path.at(pathIndex);
}

void NavigationPath::setNextGoToPoint() {
    if(pathIndex + 1 == path.size()){
        throw NavigationExceptionPathPointAlreadyCompleted();
    }
    pathIndex++;
}

bool NavigationPath::isNextPointAvailable() const {
    return  pathIndex + 1 < path.size();
}

