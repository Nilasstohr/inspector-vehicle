//
// Created by robot1 on 7/3/24.
//

#ifndef ROBOTVEHICLE_PACKAGE_GRIDMAP_H
#define ROBOTVEHICLE_PACKAGE_GRIDMAP_H

#include "Utilities/MatrixHelper.h"
#include "Configurations.h"
#include "Localization/Line.h"
#include "Utilities/Pose.h"
#include "LinePoints.h"
#include "Sensor/SensorData.h"
#include <iomanip>
#include <fstream>
#include "ObstacleDetection.h"



class GridMap: MatrixHelper {
public:
    GridMap(double gridMapValueAvailable, double gridMapValueOccupied, double gridMapValueUpdateInterval);
    void update(std::vector<PointPolarForm> * scan, Pose *currentPose);
    string * obstacleSafeDistanceMapToString();
    void storeMap();
    void loadGridMap();
    MatrixXd * updateMapWithObstacleSafeDistance();
    string *mapToString();
    ObstacleDetection * getObstacleDetection();
    bool isPoseInSafeZone(Pose *currentPose);

private:
    MatrixXd gridMap;
    MatrixXd gridMapCopy;
    LinePoints linePoints = LinePoints(CONFIG_DISTANCE_BETWEEN_POINTS_CM);
    void updateAvailabilityFromLinePoints();
    void updateMapPointValue(int x, int y,double value);
    string * toString(MatrixXd *gridMap);
    void addSafetyDistance();
    double gridMapValueAvailable;
    double gridMapValueOccupied;
    double gridMapValueUpdateInterval;
    int x;
    int y;
    double mapValue;
    string gridMapString;
    ObstacleDetection obstacleDetection;


};


#endif //ROBOTVEHICLE_PACKAGE_GRIDMAP_H
