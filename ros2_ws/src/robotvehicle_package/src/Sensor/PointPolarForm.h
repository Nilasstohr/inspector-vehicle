//
// Created by robot1 on 8/10/23.
//

#ifndef ROBOTVEHICLE_PACKAGE_POINTPOLARFORM_H
#define ROBOTVEHICLE_PACKAGE_POINTPOLARFORM_H

#include <sensor_msgs/msg/detail/laser_scan__struct.hpp>

class PointPolarForm {
public:
    PointPolarForm(double angle, double distance);
    PointPolarForm();
    double getAngle() const;
    double getDistance() const;
    void setAngle(double angle);
    void setDistance(double distance);
private:
    float distance;
    float angle;
};


#endif //ROBOTVEHICLE_PACKAGE_POINTPOLARFORM_H
