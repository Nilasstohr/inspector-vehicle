//
// Created by robot1 on 9/23/23.
//

#ifndef ROBOTVEHICLE_PACKAGE_LINE_H
#define ROBOTVEHICLE_PACKAGE_LINE_H


#include "Sensor/PointRectForm.h"
#include "Sensor/PointPolarForm.h"
#include <vector>
#include <Utilities/Pose.h>

class Line {
public:
    Line();
    void addRecPointFromPolar(double theta,double d);
    void addRecPoint(double x, double y);
    double perpendicularDistance(PointPolarForm *point);
    void updateOriginLineNormal();
    double getAlfa() const;
    void setAlfa(double alfa);
    double getR() const;
    void setR(double r);
    void toGlobalReferenceFrame(const Pose *currentPose);
    double getPhi();
    void updateSlopeFormLeastSquare();
    static void limitAngle(double &angle);
    static void correctPolarCoordinates(double &alfa, double &r);
    void reset();
    int getPointCount() const;
    void getLineEndPoints(double &x1,double &y1,double &x2,double &y2);
    void getParallelTransEndPoints(double &x1p,double &y1p,double &x2p,double &y2p,const int d);
    int getNumberOfPoint() const;

    PointRectForm * getFirstPoint();
    PointRectForm * getLastPoint();

    // line points support.
    void updateSlopeForm();
    double getM();
    double getB();

private:
    // linePoints in normal form from origo.
    double r;
    double alfa;
    // linePoints in slope form from first and last point
    double m;
    double b;
    //
    bool isAlreadyGlobal;
    std::vector<PointRectForm> points;
    int pointsNum;

    double getXFromPolarForm(double theta, double d);
    double getYFromPolarForm(double theta, double d);

    // parallel transition
    double x1;
    double y1;
    double x2;
    double y2;
    double l;
    double dx;
    double dy;
};


#endif //ROBOTVEHICLE_PACKAGE_LINE_H
