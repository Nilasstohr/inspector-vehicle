//
// Created by robot1 on 10/1/23.
//

#include <iostream>
#include "LineStack.h"


LineStack::LineStack(int capacity) :MatrixStackBase(capacity,2){
    stack = MatrixXd(this->capacity * 2, 1);
    line = MatrixXd(2,1);
}

void LineStack::add(Line *line) {
    add(line->getAlfa(),line->getR());
}

void LineStack::add(double alfa, double r) {
    if(stackNum >= capacity){
        throw ExceptionExceededMaxEntries(capacity,"Lines in Stack",
          __FILE__,__func__,__LINE__);
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

void LineStack::setByIndex(int j,double alfa,double r) {
    stack(mapIndexFirst(j), 0)=alfa;
    stack(mapIndexSecond(j), 0)=r;
}

void LineStack::getByIndex(int j,double &alfa,double &r){
    alfa = getAlfaByIndex( j);
    r    = getRByIndex(j);
}

 MatrixXd *LineStack::getLineByIndex(int j) {
    line(0,0)= getAlfaByIndex(j);
    line(1,0)= getRByIndex(j);
    return &line;
}

const MatrixXd LineStack::getLines() {
    return stack.block(0,0,index,1);
}

void LineStack::printLines(char * name){
    MatrixXd horLines = unstackLines();
    MatrixHelper::printMatrix(&horLines,name, 2);
}
 MatrixXd LineStack::unstackLines(){
    MatrixXd linesUnstack = MatrixXd(2, stackNum);
    MatrixXd linesStack = getLines();
    int j=0;
    int i=0;
    while(j<stackNum){
        linesUnstack(0, j) = linesStack.coeffRef(i++, 0);
        linesUnstack(1, j)  = linesStack.coeffRef(i++, 0);
        j++;
    }
    return linesUnstack;
}

