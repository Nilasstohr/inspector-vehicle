//
// Created by robot1 on 10/1/23.
//

#include "LineStack.h"

LineStack::LineStack(int capacity) :MatrixStackBase(capacity,2){
    stack = MatrixXd(this->capacity * 2, 1);
    line = new MatrixXd(2,1);
}

void LineStack::add(Line *line) {
    add(line->getAlfa(),line->getR());
}

void LineStack::add(double alfa, double r) {
    if(stackNum >= capacity){
        throw "max number of lines exceeds";
    }
    stack(index++, 0)=alfa;
    stack(index++, 0)=r;
    stackNum++;
}

void LineStack::add(const MatrixXd *line) {
    add(line->coeff(0,0),line->coeff(1,0));
}

double LineStack::getAlfaByIndex(int j) const{
    return stack(mapIndexFirst(j), 0);
}

double LineStack::getRByIndex(int j) const{
    return stack(mapIndexSecond(j), 0);
}

const MatrixXd *LineStack::getLine() {
    return line;
}