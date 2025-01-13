//
// Created by robot1 on 10/5/23.
//

#ifndef ROBOTVEHICLE_PACKAGE_MATCHES_H
#define ROBOTVEHICLE_PACKAGE_MATCHES_H

#include "HStack.h"
#include "LineStack.h"
#include "RStack.h"
#include "Utilities/ExceptionExceededMaxEntries.h"

class Matches: MatrixHelper {
public:
    Matches(int capacity);
    void add(const MatrixXd *vIJ, const MatrixXd *HIJ, const MatrixXd *R);
    void reset();
    void addPEst(const MatrixXd *PEst);
    const MatrixXd * getPEst() const;
    const MatrixXd * getXEst() const;
    const MatrixXd  getHt() const;
    const MatrixXd  getVt() const;
    const MatrixXd getVtUnstacked() const;
    const MatrixXd  getRt() const;
    void addXEst(const MatrixXd *xEst);
    int getMatchCount() const;
private:
    HStack    *Ht;
    LineStack *vt;
    RStack *Rt;
    const MatrixXd  *PEst;
    const MatrixXd *xEst;
    int matchCount;
    int capacity;
};


#endif //ROBOTVEHICLE_PACKAGE_MATCHES_H
