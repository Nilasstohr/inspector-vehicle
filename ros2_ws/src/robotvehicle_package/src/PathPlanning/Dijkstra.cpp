//
// Created by robot1 on 5/14/24.
//

#include <iostream>
#include "Dijkstra.h"

Dijkstra::Dijkstra(Pose *currentPose, PathPoint *endPoint, MatrixXd *gridMap) {
    MatrixXd visited =  MatrixXd::Zero(gridMap->cols(),gridMap->rows());
    PathPoint startPoint = PathPoint();
    startPoint.set(floor(currentPose->getX()),floor(currentPose->getY()));
    nodeQueue.enQueue(&startPoint,&startPoint,0,gridMap,&visited);
    Node * node;
    PathPoint *point;
    PathPoint *path;
    int xAd;
    int yAd;
    int p=0;
    int q=0;
    while(!nodeQueue.isEmpty()){
        node = nodeQueue.pop();
        q++;
        //node->printNode();
        point = node->getPoint();
        visited(point->getX(),point->getY())=true;
        //printMatrix(&visited,"visited matrix");
        for(int i=0; i<node->getAdjacentSize(); i++){
            point = &node->getAdjacents()[i];
            xAd=point->getX();
            yAd=point->getY();
            if(xAd==endPoint->getX() && yAd==endPoint->getY()){
                path = node->getPathWithGoal(endPoint);
                cout << "popped: " << q << " added:"<< p << endl;
                return;
            }
            p++;
            nodeQueue.enQueue(point,node->getPath(),
                              node->getPathSize(),gridMap,&visited);

        }
    }
}
