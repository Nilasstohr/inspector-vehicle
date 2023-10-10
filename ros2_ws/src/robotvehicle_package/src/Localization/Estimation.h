//
// Created by robot1 on 10/9/23.
//

#ifndef ROBOTVEHICLE_PACKAGE_ESTIMATION_H
#define ROBOTVEHICLE_PACKAGE_ESTIMATION_H


#include "Matching.h"

class Estimation: public MatrixHelper{
public:
    Estimation();
    void update(Matching * matching);
private:
    MatrixXd *xt;
    MatrixXd *Pt;
};


#endif //ROBOTVEHICLE_PACKAGE_ESTIMATION_H
