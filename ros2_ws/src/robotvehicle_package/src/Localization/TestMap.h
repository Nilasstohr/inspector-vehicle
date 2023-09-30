//
// Created by robot1 on 9/30/23.
//

#ifndef ROBOTVEHICLE_PACKAGE_TESTMAP_H
#define ROBOTVEHICLE_PACKAGE_TESTMAP_H


#include <vector>
#include "FeatureExstraction/Line.h"

class TestMap {
public:
    TestMap();
    const std::vector<Line> * getMap();
private:
    std::vector<Line> lines;

};


#endif //ROBOTVEHICLE_PACKAGE_TESTMAP_H
