//
// Created by robot1 on 9/30/23.
//

#ifndef ROBOTVEHICLE_PACKAGE_TESTMAP_H
#define ROBOTVEHICLE_PACKAGE_TESTMAP_H


#include <vector>
#include "Line.h"
#include "LineStack.h"

class TestMap: public MatrixHelper{
public:
    TestMap();
    LineStack * getMap();
private:
    LineStack lineStack = LineStack(4);
};


#endif //ROBOTVEHICLE_PACKAGE_TESTMAP_H
