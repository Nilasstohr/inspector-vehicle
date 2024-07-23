//
// Created by robot1 on 6/28/24.
//

#ifndef ROBOTVEHICLE_PACKAGE_ASTAR_H
#define ROBOTVEHICLE_PACKAGE_ASTAR_H

#include "../Utilities/Pose.h"
#include "PathPoint.h"
#include "../Utilities/MatrixHelper.h"
#include "../Configurations.h"

// Creating a shortcut for int, int pair type
typedef pair<int, int> Pair;

// Creating a shortcut for pair<int, pair<int, int>> type
typedef pair<double, pair<int, int> > pPair;

// A structure to hold the necessary parameters
struct cell {
    // Row and Column index of its parent
    // Note that 0 <= i <= CONFIG_GRID_ROW_SIZE-1 & 0 <= j <= CONFIG_GRID_COL_SIZE-1
    int parent_i, parent_j;
    // f = g + h
    double f, g, h;
};

class AStar: public MatrixHelper {
public:
    AStar(Pose *currentPose, PathPoint *destination, MatrixXd *grid);
private:
    bool isValid(int row, int col);
    bool isDestination(int row, int col, Pair dest);
    double calculateHValue(int row, int col, Pair dest);
    void tracePath(cell cellDetails[][CONFIG_GRID_COL_SIZE], Pair dest);
    //bool isUnBlocked(int grid[][CONFIG_GRID_COL_SIZE], int row, int col);
    bool isUnBlocked(MatrixXd *grid, int row, int col);
};


#endif //ROBOTVEHICLE_PACKAGE_ASTAR_H
