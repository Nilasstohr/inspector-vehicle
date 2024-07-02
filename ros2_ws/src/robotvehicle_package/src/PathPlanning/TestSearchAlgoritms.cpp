//
// Created by robot1 on 5/17/24.
//

#include "TestSearchAlgoritms.h"


TestSearchAlgoritms::TestSearchAlgoritms() {

    Pose currentPose = Pose();
    currentPose.update(0,0,0);
    PathPoint endPoint = PathPoint();
    endPoint.set(249,249);

    //MatrixXd gridMap(10,10);
    MatrixXd gridMap =  MatrixXd::Ones(GRID_ROW_SIZE,GRID_COL_SIZE);
    //gridMap.coeffRef(1,1)=255;
    //printMatrix(&gridMap,"gridMap");
    /*
    gridMap << 1, 0, 1, 1, 1, 1, 0, 1, 1, 1,
               1, 1, 1, 0, 1, 1, 1, 0, 1, 1,
               1, 1, 1, 0, 1, 1, 0, 1, 0, 1,
               0, 0, 1, 0, 1, 0, 0, 0, 0, 1,
               1, 1, 1, 0, 1, 1, 1, 0, 1, 0,
               1, 0, 1, 1, 1, 1, 0, 1, 0, 0,
               1, 0, 0, 0, 0, 1, 0, 0, 0, 1,
               1, 0, 1, 1, 1, 1, 0, 1, 1, 1,
               1, 1, 1, 0, 0, 0, 1, 0, 0, 1;
    */
    AStar *asta = new AStar(&currentPose,&endPoint,&gridMap);

 }
