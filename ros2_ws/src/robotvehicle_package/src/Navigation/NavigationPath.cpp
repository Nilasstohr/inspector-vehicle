//
// Created by robot1 on 2/18/24.
//

#include "NavigationPath.h"

NavigationPath::NavigationPath(SerialInterface *serialInterface, KalmanFilter *localization):
        serialInterface(serialInterface), localization(localization){
    path = new std::vector<NavigationPoint>;
}

void NavigationPath::addPathPoint(double x, double y, double theta) {
    path->push_back(NavigationPoint(x,y,theta));
}

