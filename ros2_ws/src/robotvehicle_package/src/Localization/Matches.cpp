//
// Created by robot1 on 10/5/23.
//

#include "Matches.h"

Matches::Matches(int capacity) {
    vt   = new LineStack(capacity);
    Ht = new HStack(capacity);
    matchCount=0;
}

void Matches::reset() {
    matchCount=0;
}

void Matches::add(const MatrixXd *vIJ, const MatrixXd *HIJ) {
    vt->add(vIJ);
    Ht->add(HIJ);
    matchCount++;
}

void Matches::addPEst(const MatrixXd *PEst) {
    this->PEst = PEst;
}

const MatrixXd *Matches::getPEst() {
    return PEst;
}

void Matches::addXEst(const Vector3d *xEst) {
    this->xEst = xEst;
}
