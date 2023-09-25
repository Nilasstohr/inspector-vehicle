//
// Created by robot1 on 9/23/23.
//

#ifndef ROBOTVEHICLE_PACKAGE_LINE_H
#define ROBOTVEHICLE_PACKAGE_LINE_H


#include "../../Sensor/PointRectForm.h"
#include "../../Sensor/PointPolarForm.h"

class Line {
public:
    explicit Line();
    void addRecPointFromPolar(double theta,double d);
    double perpendicularDistance(PointPolarForm &form);

private:
    double alfa;
    double r;
    PointRectForm* points[700];
    int pointsNum;
    void reset();
};


#endif //ROBOTVEHICLE_PACKAGE_LINE_H
