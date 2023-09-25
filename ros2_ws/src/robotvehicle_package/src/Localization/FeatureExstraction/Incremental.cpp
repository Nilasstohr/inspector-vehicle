//
// Created by robot1 on 9/17/23.
//

#include "Incremental.h"

Incremental::Incremental(double esp) : lineNum(0),esp(esp) {}

int Incremental::getLineNum() {
    return lineNum ;
}

void Incremental::reset() {
    lineNum=0;
}

void Incremental::update(std::vector<PointPolarForm> *scan) {
    reset();
    executeAlgoritm(scan);
}

void Incremental::executeAlgoritm(std::vector<PointPolarForm> *scan) {
    Line * line = lines[lineNum];
    line->addRecPointFromPolar(scan->at(1).getAngle(),scan->at(1).getDistance());
    line->addRecPointFromPolar(scan->at(2).getAngle(),scan->at(2).getDistance());
    if(scan->size()==2){
        lineNum++;
        return;
    }
    scan->erase(scan->cbegin());
    scan->erase(scan->cbegin());
    for(int i=0; i<scan->size(); i++){

        if(esp < line->perpendicularDistance(scan->at(i)) ){

        }
    }
}


