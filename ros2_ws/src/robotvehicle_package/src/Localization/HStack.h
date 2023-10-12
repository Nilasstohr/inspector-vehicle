//
// Created by robot1 on 10/1/23.
//

#ifndef ROBOTVEHICLE_PACKAGE_HSTACK_H
#define ROBOTVEHICLE_PACKAGE_HSTACK_H


#include "../Utilities/MatrixStackBase.h"

class HStack: public MatrixStackBase{
public:
    HStack(int capacity);
    void add(const MatrixXd * H);
    MatrixXd * getHByIndex(int j);
    MatrixXd getHt();
private:
    MatrixXd H;
};


#endif //ROBOTVEHICLE_PACKAGE_HSTACK_H
