//
// Created by robot1 on 6/28/24.
//

#include <cfloat>
#include <set>
#include <iostream>
#include "AStar.h"
#include "../Utilities/Verify2DArea.h"

AStar::AStar(){
    navigationPath = new NavigationPath();
}

void AStar::update(Pose *currentPose, PathPoint *destination, MatrixXd *gridMap) {
    this->currentPose=currentPose;
    MatrixXd visited =  MatrixXd::Zero(gridMap->cols(),gridMap->rows());
    PathPoint startPoint = PathPoint();
    startPoint.set(floor(currentPose->getX()),floor(currentPose->getY()));

    Pair src  = make_pair(currentPose->getX(), currentPose->getY());
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
    if (isUnBlocked(gridMap, src.first, src.second) == false
        || isUnBlocked(gridMap, dest.first, dest.second)
           == false) {
        printf("Source or the destination is blocked\n");
        return;
    }

    // If the destination cell is the same as source cell
    if (isDestination(src.first, src.second, dest)
        == true) {
        printf("We are already at the destination\n");
        return;
    }

    // Create a closed list and initialise it to false which
    // means that no cell has been included yet This closed
    // list is implemented as a boolean 2D array
    bool closedList[CONFIG_GRID_ROW_SIZE][CONFIG_GRID_COL_SIZE];
    memset(closedList, false, sizeof(closedList));

    // Declare a 2D array of structure to hold the details
    // of that cell
    cell cellDetails[CONFIG_GRID_ROW_SIZE][CONFIG_GRID_COL_SIZE];

    int i, j;

    for (i = 0; i < CONFIG_GRID_ROW_SIZE; i++) {
        for (j = 0; j < CONFIG_GRID_COL_SIZE; j++) {
            cellDetails[i][j].f = FLT_MAX;
            cellDetails[i][j].g = FLT_MAX;
            cellDetails[i][j].h = FLT_MAX;
            cellDetails[i][j].parent_i = -1;
            cellDetails[i][j].parent_j = -1;
        }
    }

    // Initialising the parameters of the starting node
    i = src.first, j = src.second;
    cellDetails[i][j].f = 0.0;
    cellDetails[i][j].g = 0.0;
    cellDetails[i][j].h = 0.0;
    cellDetails[i][j].parent_i = i;
    cellDetails[i][j].parent_j = j;

    /*
     Create an open list having information as-
     <f, <i, j>>
     where f = g + h,
     and i, j are the row and column index of that cell
     Note that 0 <= i <= CONFIG_GRID_ROW_SIZE-1 & 0 <= j <= CONFIG_GRID_COL_SIZE-1
     This open list is implemented as a set of pair of pair.
     */
    set<pPair> openList;

    // Put the starting cell on the open list and set its
    // 'f' as 0
    openList.insert(make_pair(0.0, make_pair(i, j)));

    // We set this boolean value as false as initially
    // the destination is not reached.
    bool foundDest = false;

    int iS[8] = {-1, 1,0, 0, -1, -1,1, 1};

    int jS[8] = { 0, 0,1,-1,  1, -1,1,-1};

    while (!openList.empty()) {
        pPair p = *openList.begin();
        // Remove this vertex from the open list
        openList.erase(openList.begin());
        //std::cout << "List Size: " << openList.size() << std::endl;

        // Add this vertex to the closed list
        i = p.second.first;
        j = p.second.second;
        closedList[i][j] = true;


        /*
         Generating all the 8 successor of this cell

             N.W   N   N.E
               \   |   /
                \  |  /
             W----Cell----E
                  / | \
                /   |  \
             S.W    S   S.E

         Cell-->Popped Cell (i, j)
         N -->  North       (i-1, j)
         S -->  South       (i+1, j)
         E -->  East        (i, j+1)
         W -->  West        (i, j-1)
         N.E--> North-East  (i-1, j+1)
         N.W--> North-West  (i-1, j-1)
         S.E--> South-East  (i+1, j+1)
         S.W--> South-West  (i+1, j-1)*/

        // To store the 'g', 'h' and 'f' of the 8 successors
        double gNew, hNew, fNew;
        int n = sizeof(iS) / sizeof(int);
        for(int k=0; k<n;k++){
            int iSCell = i + iS[k];
            int jSCell = j + jS[k];
            if (isValid(iSCell, jSCell) == true) {
                // If the destination cell is the same as the
                // current successor
                if (isDestination(iSCell, jSCell, dest) == true) {
                    isDestination(iSCell, jSCell, dest);
                    // Set the Parent of the destination cell
                    cellDetails[iSCell][jSCell].parent_i = i;
                    cellDetails[iSCell][jSCell].parent_j = j;
                    //printf("The destination cell is found\n");
                    tracePath(cellDetails, dest);
                    foundDest = true;
                    return;
                }
                    // If the successor is already on the closed
                    // list or if it is blocked, then ignore it.
                    // Else do the following
                else if (closedList[iSCell][jSCell] == false
                         && isUnBlocked(gridMap, iSCell, jSCell)
                            == true) {
                    gNew = cellDetails[i][j].g + 1.0;
                    hNew = calculateHValue(iSCell, jSCell, dest);
                    fNew = gNew + hNew;

                    // If it isn’t on the open list, add it to
                    // the open list. Make the current square
                    // the parent of this square. Record the
                    // f, g, and h costs of the square cell
                    //                OR
                    // If it is on the open list already, check
                    // to see if this path to that square is
                    // better, using 'f' cost as the measure.
                    if (cellDetails[iSCell][jSCell].f == FLT_MAX
                        || cellDetails[iSCell][jSCell].f > fNew) {
                        openList.insert(make_pair(
                                fNew, make_pair(iSCell, jSCell)));

                        // Update the details of this cell
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
    // When the destination cell is not found and the open
    // list is empty, then we conclude that we failed to
    // reach the destination cell. This may happen when the
    // there is no way to destination cell (due to
    // blockages)
    if (foundDest == false)
        printf("Failed to find the Destination Cell\n");

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
                            navigationPath->getPath()->back().getY(),x,y) < CONFIG_MIN_DISTANCE_BETWEEN_POINTS_CM){
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


