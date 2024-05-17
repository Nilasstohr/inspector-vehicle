//
// Created by robot1 on 5/14/24.
//

#include "Dijkstra.h"

Dijkstra::Dijkstra(Pose *currentPose, PathPoint *endPoint, MatrixXd *gridMap) {
    MatrixXd visited =  MatrixXd::Zero(gridMap->cols(),gridMap->rows());
    PathPoint startPoint = PathPoint();
    startPoint.set(floor(currentPose->getX()),floor(currentPose->getY()));
    nodeQueue.enQueue(&startPoint,&startPoint,1,gridMap,&visited);
    Node * node;
    PathPoint *point;
    PathPoint *path;
    int xAd;
    int yAd;
    while(!nodeQueue.isEmpty()){
        node = nodeQueue.pop();
        point = node->getPoint();
        visited(point->getX(),point->getX())=true;
        for(int i=0; i<node->getAdjacentSize(); i++){
            point = &node->getAdjacents()[i];
            xAd=point->getX();
            yAd=point->getY();
            if(xAd==endPoint->getX() && yAd==endPoint->getY()){
                path = node->getPathWithGoal(endPoint);
                return;
            }
            nodeQueue.enQueue(point,node->getPath(),
                              node->getPathSize(),gridMap,&visited);
        }
    }
}
