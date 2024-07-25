//
// Created by robot1 on 2/18/24.
//

#ifndef ROBOTVEHICLE_PACKAGE_NAVIGATIONPATH_H
#define ROBOTVEHICLE_PACKAGE_NAVIGATIONPATH_H

#include <vector>
#include "NavigationPoint.h"

class NavigationPath {
public:
    NavigationPath();
    void addPathPoint(double x, double y, double theta);
    std::vector<NavigationPoint> *getPath() const;

    void clear();

private:
    std::vector<NavigationPoint> *path;

};


#endif //ROBOTVEHICLE_PACKAGE_NAVIGATIONPATH_H
