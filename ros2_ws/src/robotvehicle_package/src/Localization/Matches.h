//
// Created by robot1 on 10/5/23.
//

#ifndef ROBOTVEHICLE_PACKAGE_MATCHES_H
#define ROBOTVEHICLE_PACKAGE_MATCHES_H

#include "HStack.h"
#include "FeatureExstraction/LineStack.h"

class Matches {
public:
    Matches(int capacity);
private:
    HStack    *hStack;
    LineStack *z;
    LineStack *zEst;
    MatrixXd  *pEst;
};


#endif //ROBOTVEHICLE_PACKAGE_MATCHES_H
