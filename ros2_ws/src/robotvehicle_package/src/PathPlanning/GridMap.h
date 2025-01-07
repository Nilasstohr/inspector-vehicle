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
#include <Navigation/Navigator.h>
#include "ObstacleDetection.h"



class GridMap: MatrixHelper {
public:
    GridMap(double gridMapValueAvailable, double gridMapValueOccupied, double gridMapValueUpdateInterval);
    void update(std::vector<PointPolarForm> * scan, Pose *currentPose);
    string * obstacleSafeDistanceMapToString();
    void storeMap();
    void loadGridMap();
    void updateMapWithObstacleSafeDistance();
    void updateMapWithObstacleSafeDistance2(Lines *lines, const Pose *currentPose);
    string *mapToString();
    ObstacleDetection * getObstacleDetection();
    bool isPoseInSafeZone(Pose *currentPose);
    bool isPathBlocked(NavigationPath *navigationPath);
    MatrixXd * getMapWithSafetyDistance();
    string * scanEndPointsToString(vector<PointPolarForm> *scan, Pose *pose);
private:
    MatrixXd gridMap;
    MatrixXd gridMapWithSafetyDistance;
    LinePoints linePoints;
    void updateAvailabilityFromLinePoints();
    void updateMapPointValue(int x, int y,double value);
    string * toString(MatrixXd *gridMap);
    void addSafetyDistance();

    void updateCellIfWithinRange(int x, int y);
    double gridMapValueAvailable;
    double gridMapValueOccupied;
    double gridMapValueUpdateInterval;
    int x;
    int y;
    double mapValue;
    string gridMapString;
    string scanEndPointsString;
    ObstacleDetection obstacleDetection;


};


#endif //ROBOTVEHICLE_PACKAGE_GRIDMAP_H
