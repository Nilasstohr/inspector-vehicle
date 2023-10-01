//
// Created by robot1 on 10/1/23.
//

#include "LineStack.h"

LineStack::LineStack(int capacity) :MatrixStackBase(capacity){
    lineStack = MatrixXd(this->capacity*2,1);
}

void LineStack::add(Line *line) {
    add(line->getAlfa(),line->getR());
}

void LineStack::add(double alfa, double r) {
    if(stackNum >= capacity){
        throw "max number of lines exceeds";
    }
    lineStack(index++,0)=alfa;
    lineStack(index++,0)=r;
    stackNum++;
}

double LineStack::getAlfaByIndex(int j) const{
    return lineStack(j,0);
}

double LineStack::getRByIndex(int j) const{
    return lineStack(j+1,0);
}