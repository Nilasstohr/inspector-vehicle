//
// Created by robot1 on 8/10/23.
//

#ifndef ROBOTVEHICLE_PACKAGE_ODOMRANGELOG_H
#define ROBOTVEHICLE_PACKAGE_ODOMRANGELOG_H


#include <sensor_msgs/msg/detail/laser_scan__struct.hpp>
#include "PointPolarForm.h"

class OdomRangeLog {
public:
    OdomRangeLog(double posLeft, double posRight, sensor_msgs::msg::LaserScan::SharedPtr scan);
private:
    double posLeft;
public:
    double getPosLeft() const;

    double getPosRight() const;

private:
    double posRight;
    std::vector<PointPolarForm> *scan;
public:
    std::vector<PointPolarForm> *getScan() const;
};


#endif //ROBOTVEHICLE_PACKAGE_ODOMRANGELOG_H
