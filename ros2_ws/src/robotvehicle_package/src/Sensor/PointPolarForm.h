//
// Created by robot1 on 8/10/23.
//

#ifndef ROBOTVEHICLE_PACKAGE_POINTPOLARFORM_H
#define ROBOTVEHICLE_PACKAGE_POINTPOLARFORM_H


#include <sensor_msgs/msg/detail/laser_scan__struct.hpp>

class PointPolarForm {
public:
    PointPolarForm(float angle, float distance);
private:
    float angle;
public:
    float getAngle() const;

    float getDistance() const;

private:
    float distance;
};


#endif //ROBOTVEHICLE_PACKAGE_POINTPOLARFORM_H
