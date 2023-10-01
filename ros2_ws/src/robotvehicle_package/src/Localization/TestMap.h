//
// Created by robot1 on 9/30/23.
//

#ifndef ROBOTVEHICLE_PACKAGE_TESTMAP_H
#define ROBOTVEHICLE_PACKAGE_TESTMAP_H


#include <vector>
#include "FeatureExstraction/Line.h"
#include "FeatureExstraction/LineStack.h"

class TestMap {
public:
    TestMap();
    LineStack * getMap();
private:
    LineStack lineStack = LineStack(4);
};


#endif //ROBOTVEHICLE_PACKAGE_TESTMAP_H
