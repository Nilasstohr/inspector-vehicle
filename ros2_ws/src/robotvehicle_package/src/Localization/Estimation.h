//
// Created by robot1 on 10/9/23.
//

#ifndef ROBOTVEHICLE_PACKAGE_ESTIMATION_H
#define ROBOTVEHICLE_PACKAGE_ESTIMATION_H


#include "Matching.h"

class Estimation: public MatrixHelper{
public:
    Estimation(MatrixXd xt, MatrixXd Pt);
    void update(Matching * matching,const MatrixXd* xEst,const MatrixXd* pEst);

    const MatrixXd * getXt() const;

    const MatrixXd * getPt() const;

private:
    MatrixXd xt;
    MatrixXd Pt;
};


#endif //ROBOTVEHICLE_PACKAGE_ESTIMATION_H
