//
// Created by robot1 on 2/18/24.
//

#ifndef ROBOTVEHICLE_PACKAGE_NAVIGATIONPATH_H
#define ROBOTVEHICLE_PACKAGE_NAVIGATIONPATH_H

#include <vector>
#include "NavigationPoint.h"
#include "../Localization/KalmanFilter.h"
#include "../Utilities/SerialInterface.h"

class NavigationPath {
public:
    NavigationPath(SerialInterface* serialInterface,KalmanFilter* localization);
    void addPathPoint(double x, double y, double theta);
    void update();
private:
    std::vector<NavigationPoint> *path;
    SerialInterface* serialInterface;
    KalmanFilter * localization;

};


#endif //ROBOTVEHICLE_PACKAGE_NAVIGATIONPATH_H
