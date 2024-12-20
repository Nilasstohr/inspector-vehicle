//
// Created by robot1 on 3/19/24.
//

#ifndef ROBOTVEHICLE_PACKAGE_NAVIGATOR_H
#define ROBOTVEHICLE_PACKAGE_NAVIGATOR_H

#include "Localization/KalmanLocalization.h"
#include "Utilities/Pose.h"
#include "Host/DriverInterface.h"
#include "NavigationPath.h"

class Navigator {
public:
    Navigator(DriverInterface * driverInterface);
    void update(KalmanLocalization *localization);
    void update();
    void setNavigationPath(NavigationPath * navigationPath);
    bool isDestinationReached();
    void stopAndResetDisplacement();
    void backwardSlow();
    void forwardSlow();
    void stop();
    NavigationPath* getNavigationPath();

private:
    void setLowerVelocityLimit(double wMin);
    DriverInterface* driverInterface;
    Pose *xt;
    NavigationPath * navigationPath;
    NavigationPoint *xtGoal;
    int navigationPointIndex;
    double dx;
    double dy;
    double ro;
    double alfa;
    double K_ro;
    double v;
    double K_alfa;
    double w;
    double wl;
    double wr;
    double l; // distance between wheels
    double r; // radius of wheel
    bool destinationReached;
    double wMin;
};



#endif //ROBOTVEHICLE_PACKAGE_NAVIGATOR_H
