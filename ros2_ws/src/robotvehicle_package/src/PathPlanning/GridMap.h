//
// Created by robot1 on 7/3/24.
//

#ifndef ROBOTVEHICLE_PACKAGE_GRIDMAP_H
#define ROBOTVEHICLE_PACKAGE_GRIDMAP_H


#include "../Utilities/MatrixHelper.h"
#include "../Configurations.h"
#include "../Localization/FeatureExstraction/Line.h"
#include "../Utilities/Pose.h"
#include "../Utilities/Transformations.h"
#include "LinePoints.h"
#include "../Sensor/SensorData.h"
#include <iomanip>
#include <iostream>
#include <fstream>

#define GRID_MAP_FILE_NAME "GridMap.txt"

class GridMap: MatrixHelper {
public:
    GridMap(double gridMapValueAvailable, double gridMapValueOccupied, double gridMapValueUpdateInterval);
    void update(std::vector<PointPolarForm> * scan, Pose *currentPose);
    string * toString();
    void storeMap();
    void loadGridMap();
    MatrixXd * map();
private:
    MatrixXd gridMap;
    LinePoints linePoints = LinePoints(CONFIG_DISTANCE_BETWEEN_POINTS_CM);
    void updateAvailabilityFromLinePoints();
    void updateMapPointValue(int x, int y,double value);
    double gridMapValueAvailable;
    double gridMapValueOccupied;
    double gridMapValueUpdateInterval;
    int x;
    int y;
    double mapValue;
    string gridMapString;



};


#endif //ROBOTVEHICLE_PACKAGE_GRIDMAP_H
