//
// Created by robotcentral on 1/5/25.
//

#ifndef POINTBASE_H
#define POINTBASE_H

#include <vector>
#include "PathPoint.h"
#include <cmath>
#include <Configurations.h>

class PointBase {
public:
    PointBase();
    int size();
    void getByIndex(int &x, int &y, int i);
    virtual ~PointBase() = default;
protected:
    virtual void reset();
    virtual void update() =0;
    int getPointsNum();
    void addPointsNum(double x, double y);
    void addPoint(double x, double y);
private:
    std::vector<PathPoint> *points;
    int pointsNum;
};


#endif //POINTBASE_H
