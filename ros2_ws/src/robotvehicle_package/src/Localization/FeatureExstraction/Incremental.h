//
// Created by robot1 on 9/17/23.
//

#ifndef UNTITLED4_INCREMENTAL_H
#define UNTITLED4_INCREMENTAL_H

#include <vector>
#include "Line.h"
#include "../../Sensor/PointPolarForm.h"

class Incremental{
public:
    explicit Incremental(double d);
    int getLineNum();
    void update(std::vector<PointPolarForm> *scan,int scanPointsNum);
    std::vector<Line> * getLines();
private:
    std::vector<Line> lines;
    int lineNum;
    int scanPointsNum;
    double esp;
    void reset();
    void executeAlgoritm(std::vector<PointPolarForm> *scan);

    void addPointToLine(Line *line,PointPolarForm point);
    int  next();
    int current();
    int currentPointIndex;
    bool pointToProcess();
};

#endif //UNTITLED4_INCREMENTAL_H
