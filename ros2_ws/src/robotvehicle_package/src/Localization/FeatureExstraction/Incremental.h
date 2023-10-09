//
// Created by robot1 on 9/17/23.
//

#ifndef UNTITLED4_INCREMENTAL_H
#define UNTITLED4_INCREMENTAL_H

#include <vector>
#include "Line.h"
#include "../../Sensor/PointPolarForm.h"
#include "LineStack.h"

class Incremental{
public:
    explicit Incremental(double d);
    int size();
    void update(std::vector<PointPolarForm> *scan,int scanPointsNum);
    LineStack * getLineStack();

    void reset();

private:
    Line * line;
    LineStack *lines;
    int scanPointsNum;
    double esp;

    void addLine(Line * line);
    void executeAlgoritm(std::vector<PointPolarForm> *scan);

    void addPointToLine(Line *line,PointPolarForm point);
    int  next();
    int current();
    int currentPointIndex;
    bool pointToProcess();
};

#endif //UNTITLED4_INCREMENTAL_H
