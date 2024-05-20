//
// Created by robot1 on 5/17/24.
//

#include "TestDijkstra.h"

TestDijkstra::TestDijkstra() {

    Pose currentPose = Pose();
    currentPose.update(0,0,0);
    PathPoint endPoint = PathPoint();
    endPoint.set(10,10);

    //MatrixXd gridMap(10,10);
    MatrixXd gridMap =  MatrixXd::Zero(20,20);
    gridMap.coeffRef(1,1)=255;
    //printMatrix(&gridMap,"gridMap");
    /*
    gridMap << 255, 255, 255, 255, 255, 255,255, 255, 255, 255,
               255,  0,  255, 255, 255, 255,255, 255, 255, 255,
               255, 255, 255, 255, 255, 255,255, 255, 255, 255,
               255, 255, 255, 255, 255, 255,255, 255, 255, 255,
               255, 255, 255, 255, 255, 255,255, 255, 255, 255,
               255, 255, 255, 255, 255, 255,255, 255, 255, 255,
               255, 255, 255, 255, 255, 255,255, 255, 255, 255,
               255, 255, 255, 255, 255, 255,255, 255, 255, 255,
               255, 255, 255, 255, 255, 255,255, 255, 255, 255,
               255, 255, 255, 255, 255, 255,255, 255, 255, 255;
     */
    Dijkstra *dijkstra = new Dijkstra(&currentPose,&endPoint,&gridMap);
    //Dijkstra(&currentPose,&endPoint,&gridMap);
 }
