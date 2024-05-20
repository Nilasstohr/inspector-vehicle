//
// Created by robot1 on 5/14/24.
//

#include <iostream>
#include "Node.h"

Node::Node() {
    reset();
}

void Node::update(PathPoint *point, PathPoint *path, int pathSize, const MatrixXd *gridMap, MatrixXd *visited) {
    reset();
    this->point.set(point);
    int xAd;
    int yAd;
    updatePath(path,pathSize);
    xLim[0]=0;
    xLim[1]=gridMap->cols();
    yLim[0]=0;
    yLim[1]=gridMap->rows();
    int n = sizeof(vX) / sizeof(int);
    for(int i=0; i<n;i++){
        xAd = this->point.getX()+vX[i];
        yAd = this->point.getY()+vY[i];
        if(isPointWithinGraph(xAd,yAd) && !visited->coeff(xAd,yAd)){
            if(gridMap->coeff(xAd, yAd) < 10){
                adjacents[adjacentsCounter++].set(xAd,yAd);
            }
        }
    }
}



bool Node::isPointWithinGraph(int x, int y) {
    if(x < xLim[0] || x > xLim[1]){
        return false;
    }
    if(y < yLim[0] || y > yLim[1]){
        return false;
    }
    return true;
}

void Node::reset() {
    adjacentsSize=0;
    adjacentsCounter=0;
    pathSize=0;
}

void Node::updatePath(PathPoint *path, int pathSize) {
    this->pathSize = pathSize;
    for(int i=0; i<pathSize; i++){
        this->path[i].set(&path[i]);
    }
    this->path[this->pathSize++].set(&this->point);
}

PathPoint *Node::getPoint() {
    return &point;
}

int Node::getAdjacentSize() {
    return adjacentsCounter;
}

PathPoint *Node::getAdjacents() {
    return adjacents;
}

PathPoint *Node::getPathWithGoal(PathPoint *point) {
    path[pathSize++].set(point);
    printNode();
    return path;
}

PathPoint *Node::getPath() {
    return path;
}

int Node::getPathSize() {
    return pathSize;
}

void Node::printNode() {
    std::cout <<"Node (" << point.getX() << ","<< point.getY() << ") and path ";
    for(int i=0; i<pathSize; i++){
        std::cout <<"(" << path[i].getX() << ","
                        << path[i].getY()<<")"<< "->";
    }
    std::cout << std::endl;
}



