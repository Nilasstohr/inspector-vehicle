//
// Created by robot1 on 6/28/24.
//

#ifndef ROBOTVEHICLE_PACKAGE_ASTAR_H
#define ROBOTVEHICLE_PACKAGE_ASTAR_H

#include "Utilities/Pose.h"
#include "PathPoint.h"
#include "Utilities/MatrixHelper.h"
#include "Configurations.h"
#include "Navigation/NavigationPath.h"
#include <stack>

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
    // Whether this cell has been finalized (Closed Set)
    bool closed;
};

class AStar: public MatrixHelper {
public:
    AStar();
    void update(double xs,double ys,int xd,int yd, const MatrixXd & gridMap);
    const NavigationPath & getNavigationPath();
    string pathToString();
private:
    bool isValid(int row, int col);
    bool isDestination(int row, int col, Pair dest);
    double calculateHValue(int row, int col, Pair dest);
    //bool isUnBlocked(int grid[][CONFIG_GRID_COL_SIZE], int row, int col);
    bool isUnBlocked(const MatrixXd &grid, int row, int col);
    void downSampleNavigationPath(const Pair &start, stack<Pair> &&input_path);
    NavigationPath navigationPath;
    string pathString;


};


#endif //ROBOTVEHICLE_PACKAGE_ASTAR_H
