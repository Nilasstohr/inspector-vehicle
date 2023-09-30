//
// Created by robot1 on 9/23/23.
//

#include "Observations.h"

Observations::Observations(double eps, const MatrixXd &r) : esp(eps), R(r) {
    incremental = new Incremental(eps);
}

void Observations::update(std::vector<PointPolarForm> *scan, int scanPointsNum) {
    incremental->update(scan,scanPointsNum);
}

Line *Observations::getLineByIndex(int index) {
    return &incremental->getLines()->at(index);
}

int Observations::getLineNum() {
    return incremental->getLineNum();
}

