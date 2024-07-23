//
// Created by robot1 on 7/4/24.
//

#ifndef ROBOTVEHICLE_PACKAGE_LINEPOINTS_H
#define ROBOTVEHICLE_PACKAGE_LINEPOINTS_H


#include "../Localization/FeatureExstraction/Line.h"
#include "PathPoint.h"
#include "../Configurations.h"

class LinePoints: public Line{
public:
    LinePoints(int h);
    void update();
    int getPointSize();
    void getByIndex(int &x, int &y, int i);
private:
    int h;
    std::vector<PathPoint> *points;
    int pointsNum;
    // calculation details
};


#endif //ROBOTVEHICLE_PACKAGE_LINEPOINTS_H
