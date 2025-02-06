//
// Created by robot1 on 9/23/23.
//

#include "Observations.h"

Observations::Observations(double eps, const MatrixXd &r) : esp(eps), R_(r) {
    incremental = new Incremental(eps);

}

void Observations::update(std::vector<PointPolarForm> *scan, int scanPointsNum) {
    incremental->update(scan,scanPointsNum);
    //printLineStack();
}

int Observations::size() const {
    return incremental->size();
}

LineStack * Observations::getLinesStack() {
    return incremental->getLineStack();
}

Lines * Observations::getLines() {
    return incremental->getLines();
}

void Observations::printLineStack() {
    string zScanMeasurement = "-------z (scan measurement {R})-------";
    getLinesStack()->printLines(zScanMeasurement.data());
}

 MatrixXd * Observations::z(int i)  {
    return getLinesStack()->getLineByIndex(i);
}

const MatrixXd *Observations::R() const {
    return &R_;
}

void Observations::reset() {
    incremental->reset();
}
