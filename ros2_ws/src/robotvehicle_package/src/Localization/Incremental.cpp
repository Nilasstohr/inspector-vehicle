//
// Created by robot1 on 9/17/23.
//

#include "Incremental.h"
#include "rclcpp/rclcpp.hpp"
#define ROS_INFO RCUTILS_LOG_INFO

Incremental::Incremental(double esp) :
        esp(esp),
        currentPointIndex(0),
        scanPointsNum(0)
{
    this->lineStack = new LineStack(700);
    this->line = new Line();
    lines = Lines();
}

int Incremental::size() {
    return lineStack->size() ;
}

void Incremental::reset() {
    scanPointsNum=0;
    currentPointIndex=0;
    lineStack->reset();
    lines.reset();
}

void Incremental::update(std::vector<PointPolarForm> *scan,int scanPointsNum) {
    reset();
    this->scanPointsNum = scanPointsNum;
    executeAlgoritm(scan);
}

void Incremental::executeAlgoritm(std::vector<PointPolarForm> *scan) {
    // add first to point to linePoints;
    addPointToLine(line,scan->at(next()));
    addPointToLine(line,scan->at(next()));
    // make sure that there are at least 1 to process to continue
    if(!pointToProcess()){
        addLine(line);
        return;
    }
    PointPolarForm *point;
    while(true){
        point = &scan->at(current());
        if(line->perpendicularDistance(point) < esp ){
            addPointToLine(line,*point);
            if(!pointToProcess()){
                addLine(line);
                return;
            }
        }
        else{
            if(!isinf(point->getDistance()))
                addLine(line);
            if(!pointToProcess()){
                return;
            }
            break;
        }
        next();
    }
    executeAlgoritm(scan);
}


void Incremental::addPointToLine(Line *line, PointPolarForm point) {
    line->addRecPointFromPolar(point.getAngle(),point.getDistance());
}

void Incremental::addLine(Line *line) {
    line->updateOriginLineNormal();
    lineStack->add(line->getAlfa(), line->getR());
    lines.addLine(line);
    line->reset();
}

int Incremental::next() {
    return currentPointIndex++;
}

bool Incremental::pointToProcess() {
    return scanPointsNum - (currentPointIndex+1) > 0;
}

int Incremental::current() {
    return currentPointIndex;
}

LineStack *Incremental::getLineStack() {
    return lineStack;
}

Lines * Incremental::getLines() {
    return &lines;
}


