//
// Created by robot1 on 5/17/24.
//

#include "TestDijkstra.h"
#include "../Localization/Pose.h"

TestDijkstra::TestDijkstra() {
    Pose currentPose = Pose();
    currentPose.update(0,0,0);
    PathPoint endPoint = PathPoint();
    endPoint.set(1,2);
    MatrixXd gridMap;
    gridMap << 255, 255, 255,
               255,  0,  255,
               255, 255, 255;
    Dijkstra(&currentPose,&endPoint,&gridMap);
}
