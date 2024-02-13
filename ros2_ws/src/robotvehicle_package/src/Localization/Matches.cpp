//
// Created by robot1 on 10/5/23.
//

#include "Matches.h"

Matches::Matches(int capacity) {
    vt = new LineStack(capacity);
    //printMatrix(vt->getStack(),"Vt init");
    Ht = new HStack(capacity);
    //printMatrix(Ht->getStack(),"Ht init");
    Rt = new RStack(capacity);
    //printMatrix(Rt->getRt(),"Rt init");
    matchCount=0;
}

void Matches::reset() {
    matchCount=0;
    vt->reset();
    Ht->reset();
    Rt->reset();
}

void Matches::add(const MatrixXd *vIJ, const MatrixXd *HIJ,const MatrixXd *R) {
    vt->add(vIJ);
    Ht->add(HIJ);
    Rt->add(R);
    matchCount++;
}

void Matches::addPEst(const MatrixXd *PEst) {
    this->PEst = PEst;
}

const MatrixXd * Matches::getPEst() const {
    return PEst;
}

void Matches::addXEst(const MatrixXd *xEst) {
    this->xEst = xEst;
}

const MatrixXd  Matches::getRt() const {
    return Rt->getRt();
}

const MatrixXd  Matches::getHt() const {
    return Ht->getHt();
}

const MatrixXd  Matches::getVt() const {
    return vt->getLines();
}

const MatrixXd  Matches::getVtUnstacked() const {
    return vt->unstackLines();
}

const MatrixXd *Matches::getXEst() const {
    return xEst;
}

int Matches::getMatchCount() const {
    return matchCount;
}
