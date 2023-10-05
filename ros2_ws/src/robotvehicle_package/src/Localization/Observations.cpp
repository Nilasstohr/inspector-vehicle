//
// Created by robot1 on 9/23/23.
//

#include "Observations.h"

Observations::Observations(double eps, const MatrixXd &r) : esp(eps), R(r) {
    incremental = new Incremental(eps);
}

void Observations::update(std::vector<PointPolarForm> *scan, int scanPointsNum) {
    incremental->update(scan,scanPointsNum);
    printLineStack();
}

int Observations::getLineNum() {
    return incremental->getLineNum();
}

LineStack *Observations::getLinesStack() {
    return incremental->getLineStack();
}

void Observations::printLineStack() {
    getLinesStack()->printMatrix("----z----");
}
