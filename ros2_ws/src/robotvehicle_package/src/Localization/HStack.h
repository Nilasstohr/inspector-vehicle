//
// Created by robot1 on 10/1/23.
//

#ifndef ROBOTVEHICLE_PACKAGE_HSTACK_H
#define ROBOTVEHICLE_PACKAGE_HSTACK_H


#include "../Utilities/MatrixStackBase.h"

class HStack: private MatrixStackBase{

public:
    HStack(int capacity);
    void add(MatrixXd * H);
private:
    MatrixXd hStack;
};


#endif //ROBOTVEHICLE_PACKAGE_HSTACK_H
