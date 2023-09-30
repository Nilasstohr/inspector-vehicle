//
// Created by robot1 on 9/17/23.
//

#include "Incremental.h"
#include "rclcpp/rclcpp.hpp"
#define ROS_INFO RCUTILS_LOG_INFO

Incremental::Incremental(double esp) :
        lineNum(0),
        esp(esp),
        currentPointIndex(0),
        scanPointsNum(0)
{
    std::vector<Line> lines(100);
    this->lines = lines;
}

int Incremental::getLineNum() {
    return lineNum ;
}

void Incremental::reset() {
    lineNum=0;
    scanPointsNum=0;
    currentPointIndex=0;
}

void Incremental::update(std::vector<PointPolarForm> *scan,int scanPointsNum) {
    reset();
    this->scanPointsNum = scanPointsNum;
    executeAlgoritm(scan);
}

void Incremental::executeAlgoritm(std::vector<PointPolarForm> *scan) {
    Line *line = &lines.at(lineNum);
    // add first to point to line;
    addPointToLine(line,scan->at(next()));
    addPointToLine(line,scan->at(next()));
    // make sure that there are at least 1 to process to continue
    if(!pointToProcess()){
        lineNum++;
        return;
    }
    PointPolarForm *point;
    while(true){
        point = &scan->at(current());
        if(line->perpendicularDistance(point) < esp ){
            addPointToLine(line,*point);
            if(!pointToProcess()){
                lineNum++;
                return;
            }
        }
        else{
            lineNum++;
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

int Incremental::next() {
    return currentPointIndex++;
}

bool Incremental::pointToProcess() {
    return scanPointsNum - (currentPointIndex+1) > 0;
}

int Incremental::current() {
    return currentPointIndex;
}

std::vector<Line> *Incremental::getLines() {
    return &lines;
}
