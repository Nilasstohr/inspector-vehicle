//
// Created by robot1 on 6/28/24.
//

#include <cfloat>
#include <set>
#include <iostream>
#include "AStar.h"
#include "Navigation/NavigationExceptionDestinationBlocked.h"
#include "Utilities/Verify2DArea.h"

AStar::AStar(){

}


void AStar::update(double xs,double ys,int xd,int yd, const MatrixXd & gridMap) {
    MatrixXd visited = MatrixXd::Zero(gridMap.cols(), gridMap.rows());

    Pair src = make_pair(xs, ys);
    Pair dest = make_pair(xd, yd);

    // If the source is out of range
    if (isValid(src.first, src.second) == false) {
        printf("Source is invalid\n");
        return;
    }

    // If the destination is out of range
    if (isValid(dest.first, dest.second) == false) {
        printf("Destination is invalid\n");
        return;
    }

    // Either the source or the destination is blocked
    if (isUnBlocked(gridMap, src.first, src.second) == false) {
        return;
    }
    if (isUnBlocked(gridMap, dest.first, dest.second) == false) {
        printf("Destination position x=%d y=%d is blocked\n", dest.first, dest.second);
        return;
    }

    // If the destination cell is the same as source cell
    if (isDestination(src.first, src.second, dest) == true) {
        printf("We are already at the destination\n");
        return;
    }

    int rows = CONFIG_GRID_ROW_SIZE;
    int cols = CONFIG_GRID_COL_SIZE;
    float unvisited_cost= FLT_MAX;

    // Use dynamic allocation instead of stack arrays
    cell **cellDetails = new cell*[rows];

    for (int i = 0; i < rows; i++) {
        cellDetails[i] = new cell[cols];

        for (int j = 0; j < cols; j++) {
            cellDetails[i][j].f = unvisited_cost;
            cellDetails[i][j].g = unvisited_cost;
            cellDetails[i][j].h = unvisited_cost;
            cellDetails[i][j].parent_i = -1;
            cellDetails[i][j].parent_j = -1;
            cellDetails[i][j].closed = false;
        }
    }

    // Initialising the parameters of the starting node
    int x_parent = src.first, y_parent = src.second;
    cellDetails[x_parent][y_parent].f = 0.0;
    cellDetails[x_parent][y_parent].g = 0.0;
    cellDetails[x_parent][y_parent].h = 0.0;
    cellDetails[x_parent][y_parent].parent_i = x_parent;
    cellDetails[x_parent][y_parent].parent_j = y_parent;

    set<pPair> openList;
    openList.insert(make_pair(0.0, make_pair(x_parent, y_parent)));

    bool foundDest = false;



    int xS[8] = {-1, 1, 0, 0, -1, -1, 1, 1};
    int yS[8] = {0, 0, 1, -1, 1, -1, 1, -1};

    while (!openList.empty()) {
        pPair p = *openList.begin();
        openList.erase(openList.begin());

        x_parent = p.second.first;
        y_parent = p.second.second;
        cellDetails[x_parent][y_parent].closed = true;

        double gNew, hNew, fNew;
        int n = sizeof(xS) / sizeof(int);
        for (int k = 0; k < n; k++) {
            int xSCell = x_parent + xS[k];
            int ySCell = y_parent + yS[k];
            if (isValid(xSCell, ySCell) == true) {
                if (isDestination(xSCell, ySCell, dest) == true) {
                    cellDetails[xSCell][ySCell].parent_i = x_parent;
                    cellDetails[xSCell][ySCell].parent_j = y_parent;

                    // Need to pass cellDetails differently now - create a helper or convert
                    // Trace path before cleanup
                    stack<Pair> path;
                    int row = dest.first;
                    int col = dest.second;

                    while (!(cellDetails[row][col].parent_i == row && cellDetails[row][col].parent_j == col)) {
                        path.push(make_pair(row, col));
                        int temp_row = cellDetails[row][col].parent_i;
                        int temp_col = cellDetails[row][col].parent_j;
                        row = temp_row;
                        col = temp_col;
                    }
                    path.push(make_pair(row, col));
                    downSampleNavigationPath(src, std::move(path));

                    foundDest = true;

                    // Clean up
                    for (int i = 0; i < rows; i++) {
                        delete[] cellDetails[i];
                    }
                    delete[] cellDetails;

                    return;
                } else if (cellDetails[xSCell][ySCell].closed == false && isUnBlocked(gridMap, xSCell, ySCell) == true) {
                    gNew = cellDetails[x_parent][y_parent].g + 1.0;
                    hNew = calculateHValue(xSCell, ySCell, dest);
                    fNew = gNew + hNew;


                    // Passes if: cell is unvisited (FLT_MAX) OR we found a cheaper path to it (fNew < current f)
                    // → update cell costs and add to open list for exploration
                    if (cellDetails[xSCell][ySCell].f == unvisited_cost || cellDetails[xSCell][ySCell].f > fNew) {
                        openList.insert(make_pair(fNew, make_pair(xSCell, ySCell)));

                        cellDetails[xSCell][ySCell].f = fNew;
                        cellDetails[xSCell][ySCell].g = gNew;
                        cellDetails[xSCell][ySCell].h = hNew;
                        cellDetails[xSCell][ySCell].parent_i = x_parent;
                        cellDetails[xSCell][ySCell].parent_j = y_parent;
                    }
                }
            }
        }
    }

    if (foundDest == false)
        printf("Failed to find the Destination Cell\n");

    // Clean up
    for (int i = 0; i < rows; i++) {
        delete[] cellDetails[i];
    }
    delete[] cellDetails;

    return;
}



bool AStar::isValid(int row, int col)
{
    // Returns true if row number and column number
    // is in range
    return (row >= 0) && (row < CONFIG_GRID_ROW_SIZE) && (col >= 0)
           && (col < CONFIG_GRID_COL_SIZE);
}

// A Utility Function to check whether the given cell is
// blocked or not
bool AStar::isUnBlocked(const MatrixXd & grid, int row, int col)
{
    if(grid.coeff(row,col)==CONFIG_GRID_VALUE_SAFETY ||
       grid.coeff(row,col)==CONFIG_GRID_VALUE_SAFETY_PERIPHERAL ){
        return (false);
    }
    // Returns true if the cell is not blocked else false
    if (grid.coeff(row,col) >= CONFIG_GRID_VALUE_UPDATE_INTERVAL)
        return (true);
    else
        return (false);
}

// A Utility Function to check whether destination cell has
// been reached or not
bool AStar::isDestination(int row, int col, Pair dest)
{
    if (row == dest.first && col == dest.second)
        return (true);
    else
        return (false);
}

// A Utility Function to calculate the 'h' heuristics.
double AStar::calculateHValue(int row, int col, Pair dest)
{
    // Return using the distance formula
    return ((double)sqrt(
            (row - dest.first) * (row - dest.first)
            + (col - dest.second) * (col - dest.second)));
}

// A Utility Function to trace the path from the source
// to destination

void AStar::downSampleNavigationPath(const Pair &start,stack<Pair> &&input_path){
    stack<Pair> process_path = std::move(input_path);
    navigationPath.clear();
    NavigationPoint lastAddedPoint(start.first, start.second, 0);
    while (!process_path.empty()) {
        const int x = process_path.top().first;
        const int y = process_path.top().second;
        process_path.pop();
        if(!Verify2DArea::isPointWithinSquareArea(start.first,start.second, x, y,CONFIG_ROBOT_DIAMETER)){
            // first check if this is the last point (goal)
            if(process_path.empty()){
                if(navigationPath.isEmpty()){
                    navigationPath.addPathPoint(x,y,0);
                }else{
                    // if the distance between last path point in navigation and A-star is less than threshold
                    // we just that the A-star point
                    if(Verify2DArea::distanceBetweenPoints(
                            navigationPath.getPath().back().getX(),
                            navigationPath.getPath().back().getY(),x,y) < CONFIG_MIN_DISTANCE_BETWEEN_POINTS_CM)
                    {
                        navigationPath.overrideBack(x,y);
                    }else{
                        navigationPath.addPathPoint(x,y,0);
                    }
                }
                break;
            }else{
                if(Verify2DArea::distanceBetweenPoints(
                        lastAddedPoint.getX(),
                        lastAddedPoint.getY(), x, y) >= CONFIG_MIN_DISTANCE_BETWEEN_POINTS_CM){
                    navigationPath.addPathPoint(x,y,0);
                    lastAddedPoint.override(x,y);
                }
            }
        }
    }
}

string AStar::pathToString() {
    pathString.clear();
    stringstream stream;
    for(const NavigationPoint & point: navigationPath.getPath()) {
        stream << point.getX() << " " << point.getY() << endl;
        pathString.append(stream.str());
        stream.str(std::string());
    }
    pathString.replace(pathString.size()-1,1,"");
    return pathString;
}

 const NavigationPath & AStar::getNavigationPath() {
    return navigationPath;
}


