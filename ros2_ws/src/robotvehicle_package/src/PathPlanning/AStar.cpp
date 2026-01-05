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
    navigationPath = new NavigationPath();
}


void AStar::update(Pose *currentPose, PathPoint *destination, MatrixXd *gridMap) {
    this->currentPose = currentPose;
    MatrixXd visited = MatrixXd::Zero(gridMap->cols(), gridMap->rows());
    PathPoint startPoint = PathPoint();
    startPoint.set(floor(currentPose->getX()), floor(currentPose->getY()));

    Pair src = make_pair(currentPose->getX(), currentPose->getY());
    Pair dest = make_pair(destination->getX(), destination->getY());

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

    // Use dynamic allocation instead of stack arrays
    bool **closedList = new bool*[rows];
    cell **cellDetails = new cell*[rows];

    for (int i = 0; i < rows; i++) {
        closedList[i] = new bool[cols]();  // () initializes to false
        cellDetails[i] = new cell[cols];

        for (int j = 0; j < cols; j++) {
            cellDetails[i][j].f = FLT_MAX;
            cellDetails[i][j].g = FLT_MAX;
            cellDetails[i][j].h = FLT_MAX;
            cellDetails[i][j].parent_i = -1;
            cellDetails[i][j].parent_j = -1;
        }
    }

    // Initialising the parameters of the starting node
    int i = src.first, j = src.second;
    cellDetails[i][j].f = 0.0;
    cellDetails[i][j].g = 0.0;
    cellDetails[i][j].h = 0.0;
    cellDetails[i][j].parent_i = i;
    cellDetails[i][j].parent_j = j;

    set<pPair> openList;
    openList.insert(make_pair(0.0, make_pair(i, j)));

    bool foundDest = false;

    int iS[8] = {-1, 1, 0, 0, -1, -1, 1, 1};
    int jS[8] = {0, 0, 1, -1, 1, -1, 1, -1};

    while (!openList.empty()) {
        pPair p = *openList.begin();
        openList.erase(openList.begin());

        i = p.second.first;
        j = p.second.second;
        closedList[i][j] = true;

        double gNew, hNew, fNew;
        int n = sizeof(iS) / sizeof(int);
        for (int k = 0; k < n; k++) {
            int iSCell = i + iS[k];
            int jSCell = j + jS[k];
            if (isValid(iSCell, jSCell) == true) {
                if (isDestination(iSCell, jSCell, dest) == true) {
                    cellDetails[iSCell][jSCell].parent_i = i;
                    cellDetails[iSCell][jSCell].parent_j = j;

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
                    downSampleNavigationPath(&path);

                    foundDest = true;

                    // Clean up
                    for (int i = 0; i < rows; i++) {
                        delete[] closedList[i];
                        delete[] cellDetails[i];
                    }
                    delete[] closedList;
                    delete[] cellDetails;

                    return;
                } else if (closedList[iSCell][jSCell] == false && isUnBlocked(gridMap, iSCell, jSCell) == true) {
                    gNew = cellDetails[i][j].g + 1.0;
                    hNew = calculateHValue(iSCell, jSCell, dest);
                    fNew = gNew + hNew;

                    if (cellDetails[iSCell][jSCell].f == FLT_MAX || cellDetails[iSCell][jSCell].f > fNew) {
                        openList.insert(make_pair(fNew, make_pair(iSCell, jSCell)));

                        cellDetails[iSCell][jSCell].f = fNew;
                        cellDetails[iSCell][jSCell].g = gNew;
                        cellDetails[iSCell][jSCell].h = hNew;
                        cellDetails[iSCell][jSCell].parent_i = i;
                        cellDetails[iSCell][jSCell].parent_j = j;
                    }
                }
            }
        }
    }

    if (foundDest == false)
        printf("Failed to find the Destination Cell\n");

    // Clean up
    for (int i = 0; i < rows; i++) {
        delete[] closedList[i];
        delete[] cellDetails[i];
    }
    delete[] closedList;
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
bool AStar::isUnBlocked(MatrixXd * grid, int row, int col)
{
    if(grid->coeff(row,col)==CONFIG_GRID_VALUE_SAFETY ||
       grid->coeff(row,col)==CONFIG_GRID_VALUE_SAFETY_PERIPHERAL ){
        return (false);
    }
    // Returns true if the cell is not blocked else false
    if (grid->coeff(row,col) >= CONFIG_GRID_VALUE_UPDATE_INTERVAL)
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
void AStar::tracePath(cell cellDetails[][CONFIG_GRID_COL_SIZE], Pair dest)
{
    //printf("\nThe path is ");
    int row = dest.first;
    int col = dest.second;

    stack<Pair> path;

    while (!(cellDetails[row][col].parent_i == row
             && cellDetails[row][col].parent_j == col)) {
        path.push(make_pair(row, col));
        int temp_row = cellDetails[row][col].parent_i;
        int temp_col = cellDetails[row][col].parent_j;
        row = temp_row;
        col = temp_col;
    }
    path.push(make_pair(row, col));
    downSampleNavigationPath(&path);
}

void AStar::downSampleNavigationPath(stack<Pair> *path){
    navigationPath->clear();
    int x;
    int y;
    NavigationPoint lastAddedPoint(currentPose->getX(), currentPose->getY(), 0);
    while (!path->empty()) {
        x=path->top().first;
        y=path->top().second;
        path->pop();
        if(!Verify2DArea::isPointWithinSquareArea(currentPose, x, y,CONFIG_ROBOT_DIAMETER)){
            // first check if this is the last point (goal)
            if(path->empty()){
                if(navigationPath->getPath()->empty()){
                    navigationPath->addPathPoint(x,y,0);
                }else{
                    // if the distance between last path point in navigation and A-star is less than threshold
                    // we just that the A-star point
                    if(Verify2DArea::distanceBetweenPoints(
                            navigationPath->getPath()->back().getX(),
                            navigationPath->getPath()->back().getY(),x,y) < CONFIG_MIN_DISTANCE_BETWEEN_POINTS_CM)
                    {
                        navigationPath->getPath()->back().override(x,y);
                    }else{
                        navigationPath->addPathPoint(x,y,0);
                    }
                }
                break;
            }else{
                if(Verify2DArea::distanceBetweenPoints(
                        lastAddedPoint.getX(),
                        lastAddedPoint.getY(), x, y) >= CONFIG_MIN_DISTANCE_BETWEEN_POINTS_CM){
                    navigationPath->addPathPoint(x,y,0);
                    lastAddedPoint.override(x,y);
                }
            }
        }
    }
}

string AStar::pathToString() {
    pathString.clear();
    stringstream stream;
    for(NavigationPoint point: *navigationPath->getPath()) {
        stream << point.getX() << " " << point.getY() << endl;
        pathString.append(stream.str());
        stream.str(std::string());
    }
    pathString.replace(pathString.size()-1,1,"");
    return pathString;
}

NavigationPath *AStar::getNavigationPath() {
    return navigationPath;
}


