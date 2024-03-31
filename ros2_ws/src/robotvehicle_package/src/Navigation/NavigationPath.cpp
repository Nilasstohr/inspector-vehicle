//
// Created by robot1 on 2/18/24.
//

#include "NavigationPath.h"

NavigationPath::NavigationPath(){
    path = new std::vector<NavigationPoint>;
}

void NavigationPath::addPathPoint(double x, double y, double theta) {
    path->push_back(NavigationPoint(x,y,theta));
}

std::vector<NavigationPoint> *NavigationPath::getPath() const {
    return path;
}

