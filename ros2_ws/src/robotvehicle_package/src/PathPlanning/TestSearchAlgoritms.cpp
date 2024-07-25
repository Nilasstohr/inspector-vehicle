//
// Created by robot1 on 5/17/24.
//

#include <iostream>
#include "TestSearchAlgoritms.h"


TestSearchAlgoritms::TestSearchAlgoritms() {

    Pose currentPose = Pose();
    currentPose.update(0,0,0);
    PathPoint endPoint = PathPoint();
    endPoint.set(200,80);

    //MatrixXd gridMap(10,10);
    MatrixXd gridMap =  MatrixXd::Ones(CONFIG_GRID_ROW_SIZE, CONFIG_GRID_COL_SIZE);
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
    cout << "Navigation Path Found:" << asta->pathToString() << endl;
 }
