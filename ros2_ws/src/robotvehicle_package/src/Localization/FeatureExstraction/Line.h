//
// Created by robot1 on 9/23/23.
//

#ifndef ROBOTVEHICLE_PACKAGE_LINE_H
#define ROBOTVEHICLE_PACKAGE_LINE_H


#include "../../Sensor/PointRectForm.h"
#include "../../Sensor/PointPolarForm.h"
#include "../../Utilities/MathConstants.h"

class Line {
public:
    explicit Line();
    void addRecPointFromPolar(double theta,double d);
    double perpendicularDistance(PointPolarForm point);

private:
    // line in normal form from origo.
    double alfa;
    double r;
    // line in slope form from first and last point
    double m;
    double b;
    //
    PointRectForm* points[700];
    int pointsNum;
    void reset();
    void updateOriginLineNormal();
    void updateSlopeForm();
    double getXFromPolarForm(double theta, double d);
    double getYFromPolarForm(double theta, double d);
};


#endif //ROBOTVEHICLE_PACKAGE_LINE_H
