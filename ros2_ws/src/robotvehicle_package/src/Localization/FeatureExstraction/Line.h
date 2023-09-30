//
// Created by robot1 on 9/23/23.
//

#ifndef ROBOTVEHICLE_PACKAGE_LINE_H
#define ROBOTVEHICLE_PACKAGE_LINE_H


#include "../../Sensor/PointRectForm.h"
#include "../../Sensor/PointPolarForm.h"
#include "math.h"

class Line {
public:
    Line();
    void addRecPointFromPolar(double theta,double d);
    void addRecPoint(double x, double y);
    double perpendicularDistance(PointPolarForm *point);
    void updateOriginLineNormal();
private:
    // line in normal form from origo.
    double alfa;
public:
    double getAlfa() const;

    void setAlfa(double alfa);

    double getR() const;

    void setR(double r);

private:
    double r;
    // line in slope form from first and last point
    double m;
    double b;
    //
    std::vector<PointRectForm> points;
    int pointsNum;
    void reset();

    void updateSlopeForm();
    double getXFromPolarForm(double theta, double d);
    double getYFromPolarForm(double theta, double d);
};


#endif //ROBOTVEHICLE_PACKAGE_LINE_H
