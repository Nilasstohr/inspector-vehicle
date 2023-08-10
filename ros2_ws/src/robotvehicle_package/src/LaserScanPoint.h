//
// Created by robot1 on 8/10/23.
//

#ifndef ROBOTVEHICLE_PACKAGE_LASERSCANPOINT_H
#define ROBOTVEHICLE_PACKAGE_LASERSCANPOINT_H


#include <sensor_msgs/msg/detail/laser_scan__struct.hpp>

class LaserScanPoint {
public:
    LaserScanPoint(float angle, float distance);
private:
    float angle;
    float distance;
};


#endif //ROBOTVEHICLE_PACKAGE_LASERSCANPOINT_H
