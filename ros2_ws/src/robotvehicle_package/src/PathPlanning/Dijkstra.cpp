//
// Created by robot1 on 5/14/24.
//

#include "Dijkstra.h"

Dijkstra::Dijkstra(Pose *currentPose, PathPoint endPoint, MatrixXd *gridMap) {
    MatrixXd visited = MatrixXd(gridMap->cols(),gridMap->rows());
    PathPoint startPoint = PathPoint();
    startPoint.set(floor(currentPose->getX()),floor(currentPose->getY()));
//    nodeQueue.enQueue();
}
