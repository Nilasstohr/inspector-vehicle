//
// Created by robot1 on 5/14/24.
//

#ifndef ROBOTVEHICLE_PACKAGE_NODE_H
#define ROBOTVEHICLE_PACKAGE_NODE_H


#include "PathPoint.h"
#include "../Utilities/MatrixHelper.h"

class Node: MatrixHelper {
public:
    Node();
    void update(PathPoint *point, PathPoint *path, int pathSize, const MatrixXd *gridMap, MatrixXd *visited);

    PathPoint *getPoint();

    int getAdjacentSize();

    PathPoint *getAdjacents();

    PathPoint *getPathWithGoal(PathPoint *point);

    PathPoint *getPath();

    int getPathSize();

    void printNode();

private:
    PathPoint path[1000];
    PathPoint  point;
    int pathSize;
    int xLim[2];
    int yLim[2];
    PathPoint adjacents[8];
    int adjacentsSize;
    int adjacentsCounter;
    int vX[8] = {1,0,-1,0, 1, -1, -1, 1};
    int vY[8] = {0,1,0,-1,1,1,-1,-1};

    bool isPointWithinGraph(int x, int y);
    void reset();
    void updatePath(PathPoint *path, int pathSize);
};


#endif //ROBOTVEHICLE_PACKAGE_NODE_H
