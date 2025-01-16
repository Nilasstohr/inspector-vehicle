//
// Created by robot1 on 7/3/24.
//

#include "GridMap.h"
#include <iostream>
#include <Utilities/Transformations.h>

#include "CirclePoints.h"

GridMap::GridMap(double gridMapValueAvailable, double gridMapValueOccupied, double gridMapValueUpdateInterval):
linePoints(LinePoints(CONFIG_DISTANCE_BETWEEN_POINTS_CM)),
gridMapValueAvailable(gridMapValueAvailable),
gridMapValueOccupied(gridMapValueOccupied),
gridMapValueUpdateInterval(gridMapValueUpdateInterval){
    gridMap = MatrixXd(CONFIG_GRID_ROW_SIZE, CONFIG_GRID_COL_SIZE).setZero();
    //addSafetyDistance();
    obstacleDetection = ObstacleDetection();
}

void GridMap::update(std::vector<PointPolarForm> * scan, Pose *currentPose) {
    double xp;
    double yp;
    double xPos = currentPose->getX();
    double yPos = currentPose->getY();
    // since the robot is at the position it must be available.
    updateMapPointValue(xPos,yPos,CONFIG_GRID_VALUE_FULL_AVAILABLE);
    obstacleDetection.clear();
    for(int i=0; static_cast<long unsigned int>(i)<scan->size(); i++){
        linePoints.reset();
        Transformations::polarPointToCartesian(xp,yp,currentPose,
                                               scan->at(i).getAngle(),
                                   scan->at(i).getDistance());
        //cout << scan->at(i).getAngle() << " " << scan->at(i).getDistance() << endl;
        obstacleDetection.update(scan->at(i).getDistance(),scan->at(i).getAngle());
        //printf("at angle %.2f endpoint x=%.2f y=%.2f\n",
        //    MathConversions::rad2deg(scan->at(i).getAngle()),xp,yp);
        // the target point of the laser is most likely occupied.
        updateMapPointValue(xp,yp,-gridMapValueUpdateInterval);
        if(xPos > xp){
            linePoints.addLinePoint(xp, yp);
            linePoints.addLinePoint(xPos, yPos);
        } else{
            linePoints.addLinePoint(xPos, yPos);
            linePoints.addLinePoint(xp, yp);
        }
        linePoints.update();
        // the points that strike through from pose to target laser point
        // is most likely available.
        updateAvailabilityFromLinePoints();
    }
    //printMatrix(&gridMap,"Map");
}

void GridMap::updateAvailabilityFromLinePoints() {
    for(int j=0; j < linePoints.size(); j++){
        linePoints.getByIndex(x, y, j);
        updateMapPointValue(x,y,gridMapValueUpdateInterval);
    }
}

void GridMap::updateMapPointValue(int x, int y,double gridMapValueUpdateInterval){
    if(x>0 && y>0){
        mapValue = gridMap.coeff(x,y);
        if(mapValue==CONFIG_GRID_VALUE_SAFETY_PERIPHERAL)
            return;
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

string * GridMap::obstacleSafeDistanceMapToString() {
    return toString(&gridMapWithSafetyDistance);
}
string * GridMap::mapToString() {
    return toString(&gridMap);
}

string * GridMap:: toString(MatrixXd *gridMap) {
    gridMapString.clear();
    stringstream stream;
    for(int i=0; i<gridMap->rows(); i++){
        for(int j=0; j<gridMap->cols();j++){
            stream << std::setprecision(2) << gridMap->coeff(i,j) << " ";
            gridMapString.append(stream.str());
            stream.str(std::string());
        }
        gridMapString.replace(gridMapString.size()-1,1,"\n");
    }
    gridMapString.replace(gridMapString.size()-1,1,"");
    return &gridMapString;
}

void GridMap::storeMap(){
    std::ofstream out(CONFIG_RESOURCE_FILE_GRID_MAP);
    out << *mapToString();
    out.close();
}

void GridMap::loadGridMap(){
    std::ifstream file;
    file.open(CONFIG_RESOURCE_FILE_GRID_MAP);
    if(file.is_open()){
        string line;
        int row=0;
        int col;
        while(getline(file,line)) {
            std::stringstream ss(line);
            string valueStr;
            col=0;
            while(ss >> valueStr){
                if(gridMap.coeffRef(row,col)!=CONFIG_GRID_VALUE_SAFETY_PERIPHERAL){
                    gridMap.coeffRef(row,col)= atof(valueStr.c_str());
                }
                col++;
            }
            row++;
        }
    }else{
        printf("file open failed");
        return;
    }
    file.close();
}

void GridMap::updateMapWithObstacleSafeDistance() {
    gridMapWithSafetyDistance=gridMap;
    int radius = CONFIG_ROBOT_DIAMETER/2+CONFIG_SAFETY_DISTANCE*3;
    int x;
    int y;
    double value;
    for(int i=0; i<gridMapWithSafetyDistance.cols(); i++){
        for(int j=0; j<gridMapWithSafetyDistance.rows();j++){
            x=j;
            y=i;
            value = gridMapWithSafetyDistance.coeffRef(x,y);
            if(value!=CONFIG_GRID_VALUE_SAFETY &&
               value!=CONFIG_GRID_VALUE_SAFETY_PERIPHERAL &&
               value<CONFIG_GRID_VALUE_UPDATE_INTERVAL){
                for(int k=y-radius; k<y+radius; k++){
                    for(int l=x-radius; l<x+radius; l++){
                        if( (k>=0 && k<gridMapWithSafetyDistance.rows()) && (l>=0 && l<gridMapWithSafetyDistance.cols())){
                            if(gridMapWithSafetyDistance.coeffRef(l,k)!=CONFIG_GRID_VALUE_SAFETY_PERIPHERAL) {
                                if ((pow(l - x, 2) + pow(k - y, 2)) < pow(radius, 2)) {
                                    gridMapWithSafetyDistance.coeffRef(l, k) = CONFIG_GRID_VALUE_SAFETY;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    //printMatrix(&gridMap,"gridMap",0);
}

void GridMap::updateMapWithObstacleSafeDistance2(Lines *lines, const Pose *currentPose) {
    gridMapWithSafetyDistance=gridMap;
    constexpr int h=CONFIG_OBSTACLE_SAFETY_CONTOUR_LINE_SLOPE_X_INTERVAL_CM;
    constexpr int r = CONFIG_ROBOT_DIAMETER/2+CONFIG_SAFETY_DISTANCE;
    LinePoints linePoints(h);
    LinePoints linePointsParallelLeft1(h);
    LinePoints linePointsParallelRight1(h);
    LinePoints linePointsParallelLeft2(h);
    LinePoints linePointsParallelRight2(h);

    auto circlePoints1 = CirclePoints(CONFIG_OBSTACLE_SAFETY_CONTOUR_CIRCLE_POINTS);
    auto circlePoints2 = CirclePoints(CONFIG_OBSTACLE_SAFETY_CONTOUR_CIRCLE_POINTS);

    int x;
    int y;
    double x1;
    double x2;
    double y1;
    double y2;
    int xc;
    int yc;
    double startAngle1;
    double startAngle2;
    double tolerance = MathConversions::deg2rad(CONFIG_OBSTACLE_SAFETY_CONTOUR_CIRCLE_ANGLE_TOL_DEG);
    double angleSize = M_PI+tolerance;

    for(int i=0; i<lines->size();i++) {
        Line *line = lines->getLine(i);
        line->toGlobalReferenceFrame(currentPose);
        // original line
        line->getLineEndPoints(x1,y1,x2,y2);
        startAngle1 = line->getPhi()+M_PI/2-tolerance;
        startAngle2 = line->getPhi()+M_PI/2+M_PI-tolerance;

        for(int k=r-CONFIG_MIN_DISTANCE_BETWEEN_POINTS_CM; k<=r;k++) {
            circlePoints1.setCenterPoint(k,x1,y1,startAngle1,angleSize);
            circlePoints1.update();
            circlePoints2.setCenterPoint(k,x2,y2,startAngle2,angleSize);
            circlePoints2.update();
            for(int j=0; j<circlePoints1.size(); j++) {
                circlePoints1.getByIndex(xc,yc,j);
                updateCellIfWithinRange(xc,yc);
                circlePoints2.getByIndex(xc,yc,j);
                updateCellIfWithinRange(xc,yc);
            }
        }

        linePoints.setFromLine(x1,y1,x2,y2);

        for(int k=r-CONFIG_MIN_DISTANCE_BETWEEN_POINTS_CM; k<=r;k++) {
            // parallel shifted to the left of original line
            line->getParallelTransEndPoints(x1,y1,x2,y2,k);
            linePointsParallelLeft1.setFromLine(x1,y1,x2,y2);

            // parallel shifted to the right of original line
            line->getParallelTransEndPoints(x1,y1,x2,y2,-k);
            linePointsParallelRight1.setFromLine(x1,y1,x2,y2);

            for(int j=0; j< linePoints.size();j++) {
                linePointsParallelLeft1.getByIndex(x,y,j);
                updateCellIfWithinRange(x,y);

                linePointsParallelRight1.getByIndex(x,y,j);
                updateCellIfWithinRange(x,y);
            }
        }

    }
}

void GridMap::updateCellIfWithinRange(int x, int y) {
    if( x>=0 && x<gridMapWithSafetyDistance.rows() && y>=0 && y<gridMapWithSafetyDistance.cols() ){
        gridMapWithSafetyDistance.coeffRef(x, y) = CONFIG_GRID_VALUE_SAFETY;
    }
}

void GridMap::displayPointIfWithin(double x1, double y1, double x2, double y2) {
    if(x1>110 && x1<170 && y1>40 && y1<90) {
        if(x2>110 && x2<170 && y2>40 && y2<90) {
            cout<< x1 << " " << y1 << " " << x2 << " " << y2 << endl;
        }
    }
}

void GridMap::addSafetyDistance() {
    int safetyDistance = CONFIG_ROBOT_DIAMETER/2 + CONFIG_SAFETY_DISTANCE;
    int squareNum;
    double value = CONFIG_GRID_VALUE_SAFETY_PERIPHERAL;
    while(safetyDistance>0){
        squareNum=safetyDistance-1;
        for(int col=squareNum; col<gridMap.cols()-squareNum; col++){
            //cout << col << ":" << squareNum << endl;
            gridMap.coeffRef(squareNum,col)=value;
            //printMatrix(&gridMap,"gridMap",0);
        }
        for(int col=squareNum; col<gridMap.cols()-squareNum; col++){
            //cout << col << ":" << squareNum << endl;
            gridMap.coeffRef(gridMap.rows()-safetyDistance,col)=value;
            //printMatrix(&gridMap,"gridMap",0);
        }
        for(int row=squareNum; row<gridMap.rows()-squareNum; row++){
            //cout << row << ":" << squareNum << endl;
            gridMap.coeffRef(row,squareNum)=value;
            //printMatrix(&gridMap,"gridMap",0);
        }
        for(int row=squareNum; row<gridMap.rows()-squareNum; row++){
            //cout << row << ":" << squareNum << endl;
            gridMap.coeffRef(row,gridMap.cols()-safetyDistance)=value;
            //printMatrix(&gridMap,"gridMap",0);
        }
        safetyDistance--;
        //printMatrix(&gridMap,"gridMap",0);
    }
    //printMatrix(&gridMap,"gridMap",0);
}

ObstacleDetection *GridMap::getObstacleDetection() {
    return &obstacleDetection;
}

bool GridMap::isPoseInSafeZone(Pose *currentPose) {
    return gridMapWithSafetyDistance.coeffRef(
            static_cast<int>(currentPose->getX()), static_cast<int>(currentPose->getX()))!=CONFIG_GRID_VALUE_SAFETY;
}

bool GridMap::isPathBlocked(NavigationPath *navigationPath) {
    int x;
    int y;
    int radius = CONFIG_OBSTACLE_SAFETY_CHECK_RADIUS;
    for(NavigationPoint point: *navigationPath->getPath()){
        x = point.getX();
        y = point.getY();
        for(int k=y-radius; k<y+radius; k++){
            for(int l=x-radius; l<x+radius; l++){
                if( (k>=0 && k<gridMapWithSafetyDistance.rows()) && (l>=0 && l<gridMapWithSafetyDistance.cols())){
                    if ((pow(l - x, 2) + pow(k - y, 2)) < pow(radius, 2)) {
                        if(gridMapWithSafetyDistance.coeffRef(l, k) != CONFIG_GRID_VALUE_FULL_AVAILABLE) {
                             return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

MatrixXd * GridMap::getMapWithSafetyDistance() {
    return &gridMapWithSafetyDistance;
}

string * GridMap::scanEndPointsToString(vector<PointPolarForm> *scan, Pose *pose) {
    scanEndPointsString.clear();
    stringstream stream;
    double xp;
    double yp;
    for(int i=0; static_cast<long unsigned int>(i)<scan->size(); i++) {
        Transformations::polarPointToCartesian(xp,yp,pose,
                                               scan->at(i).getAngle(),
                                   scan->at(i).getDistance());
        stream << MathConversions::rad2deg(scan->at(i).getAngle())<< " " << xp << " "<< yp << endl;
        scanEndPointsString.append(stream.str());
        stream.str(std::string());
    }
    return &scanEndPointsString;
}



