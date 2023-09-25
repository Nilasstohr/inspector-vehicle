//
// Created by robot1 on 9/23/23.
//

#include "Observations.h"

Observations::Observations(double esp, const MatrixXd &r) : esp(esp), R(r) {
    incremental = new Incremental(esp);
}

void Observations::update(std::vector<PointPolarForm> *scan) {
    incremental->update(scan);
}