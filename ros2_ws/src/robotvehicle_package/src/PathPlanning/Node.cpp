//
// Created by robot1 on 5/14/24.
//

#include "Node.h"

Node::Node() {
    reset();
}

void Node::update(PathPoint point, PathPoint *path, int pathSize,const MatrixXd *graph, MatrixXd *visited) {
    reset();
    int xAd;
    int yAd;
    updatePath(path,pathSize);
    xLim[0]=0;
    xLim[1]=graph->cols();
    yLim[0]=0;
    yLim[1]=graph->rows();
    for(int i=0; i++; sizeof(vX)){
        xAd = point.getX()+vX[i];
        yAd = point.getY()+vY[i];
        if(isPointWithinGraph(xAd,yAd) && !visited->coeff(xAd,yAd)){
            if(graph->coeff(xAd,yAd)>200){
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
    for(int i=0; i<pathSize; i++){
        this->path[i].set(path[i].getX(),path[i].getY());
    }
}
