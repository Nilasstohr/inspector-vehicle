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

    void update(std::vector<PointPolarForm> *scan);

private:
    Line  * lines[100];
    int lineNum;
    double esp;
    void reset();
    void executeAlgoritm(std::vector<PointPolarForm> *scan);
};

#endif //UNTITLED4_INCREMENTAL_H
