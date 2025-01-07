//
// Created by robot1 on 7/4/24.
//

#ifndef ROBOTVEHICLE_PACKAGE_LINEPOINTS_H
#define ROBOTVEHICLE_PACKAGE_LINEPOINTS_H


#include "Localization/Line.h"
#include "PathPoint.h"
#include "Configurations.h"
#include "PointBase.h"

class LinePoints final : public PointBase{
public:
    explicit LinePoints(int h);
    void update() override;
    void reset() override;
    void setFromLine(Line * line);
    void setFromLine(double x1, double y1, double x2, double y2);
    void addLinePoint(double xp, double yp);
private:
    int h;
    Line line;
    // calculation details
};


#endif //ROBOTVEHICLE_PACKAGE_LINEPOINTS_H
