//
// Created by robot1 on 7/3/24.
//

#include <iomanip>
#include <iostream>
#include "GridMap.h"


GridMap::GridMap(double gridMapValueAvailable, double gridMapValueOccupied, double gridMapValueUpdateInterval):
gridMapValueAvailable(gridMapValueAvailable),
gridMapValueOccupied(gridMapValueOccupied),
gridMapValueUpdateInterval(gridMapValueUpdateInterval){
    gridMap = MatrixXd(CONFIG_GRID_ROW_SIZE, CONFIG_GRID_COL_SIZE).setZero();
}

void GridMap::update(std::vector<PointPolarForm> * scan, Pose *currentPose) {
    double xp;
    double yp;
    double xPos = currentPose->getX();
    double yPos = currentPose->getY();
    // since the robot is at the position it must be available.
    updateMapPointValue(xPos,yPos,CONFIG_GRID_VALUE_FULL_AVAILABLE);
    for(int i=0; i<scan->size(); i++){
        linePoints.reset();
        Transformations::polarPointToCartesian(xp,yp,currentPose,
                                               scan->at(i).getAngle(),
                                   scan->at(i).getDistance());
        // the target point of the laser is most likely occupied.
        updateMapPointValue(xp,yp,-gridMapValueUpdateInterval);
        if(xPos > xp){
            linePoints.addRecPoint(xp, yp);
            linePoints.addRecPoint(xPos, yPos);
        } else{
            linePoints.addRecPoint(xPos, yPos);
            linePoints.addRecPoint(xp, yp);
        }
        linePoints.update();
        // the points that strike through from pose to target laser point
        // is most likely available.
        updateAvailabilityFromLinePoints();
    }
    //printMatrix(&gridMap,"Map");
}

void GridMap::updateAvailabilityFromLinePoints() {

    for(int j=0; j < linePoints.getPointSize(); j++){
        linePoints.getByIndex(x, y, j);
        updateMapPointValue(x,y,gridMapValueUpdateInterval);
    }
}

void GridMap::updateMapPointValue(int x, int y,double gridMapValueUpdateInterval){
    if(x>0 && y>0){
        mapValue = gridMap.coeff(x,y);
        mapValue += gridMapValueUpdateInterval;
        //cout << mapValue << endl;
        if(mapValue>gridMapValueAvailable){
            mapValue=gridMapValueAvailable;
        }else if(mapValue<gridMapValueOccupied){
            mapValue=gridMapValueOccupied;
        }
        gridMap.coeffRef(x,y)=mapValue;
        //printMatrix(&gridMap,"Map");
    }
}

string * GridMap::toString() {
    gridMapString.clear();
    stringstream stream;
    for(int i=0; i<gridMap.rows(); i++){
        for(int j=0; j<gridMap.cols();j++){
            stream << std::setprecision(2) << gridMap.coeff(i,j) << " ";
            gridMapString.append(stream.str());
            stream.str(std::string());
        }
        gridMapString.replace(gridMapString.size()-1,1,"\n");
    }
    gridMapString.replace(gridMapString.size()-1,1,"");
    return &gridMapString;
}

MatrixXd *GridMap::map() {
    return &gridMap;
}




