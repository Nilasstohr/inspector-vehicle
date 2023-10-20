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
    double getPosLeft() const;
    double getPosRight() const;
    std::vector<PointPolarForm> *getScan() const;
    void setPose(double x, double y, double theta);
    double getX() const;
    double getY() const;
    double getTheta() const;
private:
    double posLeft;
    double posRight;
    double x;
    double y;
    double theta;

private:
    std::vector<PointPolarForm> *scan;
    PointPolarForm scanPoints[1000];
    int count;
};


#endif //ROBOTVEHICLE_PACKAGE_ODOMRANGELOG_H
