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
    void add(const MatrixXd * vIJ, const MatrixXd *HIJ);
    void reset();
    void addPEst(const MatrixXd *PEst);
    const MatrixXd * getPEst();
    const MatrixXd * getHt();
    const MatrixXd * getVt();
    void addXEst(const Vector3d *pMatrix);

private:
    HStack    *Ht;
    LineStack *vt;
    const MatrixXd  *PEst;
    const Vector3d *xEst;
    int matchCount;
};


#endif //ROBOTVEHICLE_PACKAGE_MATCHES_H
