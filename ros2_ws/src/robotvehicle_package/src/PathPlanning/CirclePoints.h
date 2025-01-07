//
// Created by robotcentral on 1/5/25.
//

#ifndef CIRCLEPOINTS_H
#define CIRCLEPOINTS_H
#include "PointBase.h"


class CirclePoints: public PointBase{
public:
    CirclePoints(int points);
    void update() override;
    void setCenterPoint(double r,double xc,double yc,double startAngle,double angleSize);
private:
    double r;
    double xc;
    double yc;
    double angle;
    double xCir;
    double yCir;
    double startAngle;
    double angleSize;
    double dt;
};



#endif //CIRCLEPOINTS_H
