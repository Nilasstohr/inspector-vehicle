//
// Created by robot1 on 8/22/24.
//

#include "MissionController.h"
#include "rclcpp/rclcpp.hpp"
#define ROS_INFO RCUTILS_LOG_INFO
#define DESTINATION_X 202
#define DESTINATION_Y 54

MissionController::MissionController(rclcpp::Node * node,SerialInterface * serialInterface) {
    obstacleAvoidanceInProgress=false;
    hasMapBeenBuild=false;
    DriverInterface *driverInterface = new DriverInterface(serialInterface);
    sensorData = new SensorData(driverInterface);
    localization = new KalmanLocalization();
    gripMap = new GridMap(CONFIG_GRID_VALUE_FULL_AVAILABLE,
                          CONFIG_GRID_VALUE_FULL_OCCUPIED,
                          CONFIG_GRID_VALUE_UPDATE_INTERVAL);
    navigator = new Navigator(driverInterface);
    aStar = new AStar();
    gripMap->loadGridMap();
    if (serialInterface->hasResponse()) {
        ROS_INFO(serialInterface->getResponse()->c_str());
    }
    posePublisher_ = node->create_publisher<std_msgs::msg::String>("topic_pose_string", 10);
    gridMapPublisher_ = node->create_publisher<std_msgs::msg::String>("topic_grid_map", 10);
    serialInterface->sendRequest("r");
}

SensorData * MissionController::getSensorData(){
    return sensorData;
}

Navigator * MissionController::getNavigator(){
    return navigator;
}

void MissionController::initiateNavigationPath() {
    updateMapWithObstacleSafeDistance();
    generateNewPathToDestination(DESTINATION_X, DESTINATION_Y);
}

void MissionController::updateMapWithObstacleSafeDistance() {
    gripMap->updateMapWithObstacleSafeDistance2(
       localization->getObservations()->getLines(),localization->getPose());
}

void MissionController::generateNewPathToDestination(int x, int y){
    PathPoint *endPoint = new PathPoint();
    endPoint->set(x,y);
    aStar->update(localization->getPose(),endPoint,gripMap->getMapWithSafetyDistance());
    navigator->setNavigationPath(aStar->getNavigationPath());
    return;

    //cout << "Navigation Path Found:" << aStar->pathToString() << endl;
    /*
   NavigationPath *navigationPath = new NavigationPath();
   navigationPath->addPathPoint(60,40,0);
   navigationPath->addPathPoint(77,34,0);
   navigationPath->addPathPoint(93,29,0);
   navigationPath->addPathPoint(110,27,0);
   navigationPath->addPathPoint(125,27,0);
   navigationPath->addPathPoint(140,27,0);
   navigationPath->addPathPoint(156,27,0);
   navigationPath->addPathPoint(174,26,0);
   navigationPath->addPathPoint(189,29,0);
   navigationPath->addPathPoint(200,39,0);
   navigationPath->addPathPoint(205,52,0);
   navigationPath->addPathPoint(205,65,0);
   navigationPath->addPathPoint(201,79,0);
   navigationPath->addPathPoint(190,89,0);
   navigationPath->addPathPoint(176,93,0);
   navigationPath->addPathPoint(163,96,0);
   navigationPath->addPathPoint(153,96,0);
   navigationPath->addPathPoint(143,96,0);
   navigationPath->addPathPoint(131,95,0);
   navigationPath->addPathPoint(119,90.5,0);
   navigationPath->addPathPoint(110,80,0);
   navigationPath->addPathPoint(100,70,0);
   navigationPath->addPathPoint(86,63,0);
   navigationPath->addPathPoint(70,55,0);
   navigationPath->addPathPoint(59,50,0);
   navigationPath->addPathPoint(50,45,0);
   navigationPath->addPathPoint(40,40,0);
   navigator->setNavigationPath(navigationPath());
   */

    /*
    navigationPath->addPathPoint(40,50,0);
    navigationPath->addPathPoint(40,60,0);
    navigationPath->addPathPoint(40,70,0);
    navigationPath->addPathPoint(40,80,0);
    navigationPath->addPathPoint(40,90,0);
    navigationPath->addPathPoint(50,90,0);
    navigationPath->addPathPoint(60,90,0);
    navigationPath->addPathPoint(70,90,0);
    navigationPath->addPathPoint(80,90,0);
    navigationPath->addPathPoint(90,90,0);
    navigationPath->addPathPoint(90,100,0);
    navigator->setNavigationPath(navigationPath());
     */

}

void MissionController::build() {
    localization->build(sensorData);
    gripMap->updateMapWithObstacleSafeDistance2(
       localization->getMeasurementPrediction()->getObservations()->getLines(),localization->getStarPose());
    generateNewPathToDestination(DESTINATION_X, DESTINATION_Y);
    hasMapBeenBuild = true;
}

void MissionController::update(){
    if (!hasMapBeenBuild) {
       build();
    }
    localization->update(sensorData);
    updateMapAndPath(sensorData->getScanPolarForm(),localization->getPose());
    navigator->update(localization);
    //posMessage.data = localization->getPoseLastString()->c_str();
    //posePublisher_->publish(posMessage);
    //publishRobotData();
    /*
    if(gripMap->getObstacleDetection()->isObstacleTooClose()){
        if(!obstacleAvoidanceInProgress){
            navigator->stop();
            gripMap->updateMapWithObstacleSafeDistance();
            if(gripMap->getObstacleDetection()->getObstacleLocation()==ObstacleLocationType::FRONT){
                navigator->backwardSlow();
                cout << "--------Obstacle detected (FRONT)----------- \n" << endl;
            }else{
                navigator->forwardSlow();
                cout << "--------Obstacle detected (BACK)----------- \n" << endl;
            }
        }
        obstacleAvoidanceInProgress = true;
    }else{
        if(obstacleAvoidanceInProgress){
            if(gripMap->isPoseInSafeZone(localization->getPose())){
                navigator->stop();
                cout << "######################### Obstacle cleared ########################" << endl;
                generateNewPathToDestination(DESTINATION_X, DESTINATION_Y);
                obstacleAvoidanceInProgress=false;
            }
        }else{
            navigator->update(localization);
        }
        posMessage.data = localization->getPoseLastString()->c_str();
        posePublisher_->publish(posMessage);
    }*/
}


void MissionController::updateMapAndPath(vector<PointPolarForm> *scan, Pose *pose) {
    gripMap->update(scan,pose);
    updateMapWithObstacleSafeDistance();
    //if(gripMap->isPathBlocked(navigator->getNavigationPath())) {
    //    generateNewPathToDestination(DESTINATION_X, DESTINATION_Y);
   // }
}

double MissionController::getCurrentPoseX() {
    return localization->getPose()->getX();
}
double MissionController::getCurrentPoseY() {
    return localization->getPose()->getY();
}

void MissionController::endMission(){
    posMessage.data = "end";
    posePublisher_->publish(posMessage);
    //gripMap->storeMap();
    std::this_thread::sleep_for(200ms);
    cout << "ending run" << endl;
}

void MissionController::publishRobotData() {
    Lines * matchedlines = localization->getMatching()->getMatchedLines()->toGlobalReferenceFrame(localization->getPose());
    Lines * unmatchedlines = localization->getMatching()->getUnMatchedLines()->toGlobalReferenceFrame(localization->getPose());
    Lines * maplines = localization->getMeasurementPrediction()->getMapLines();
    string robotDataString;
    robotDataString.append(gripMap->obstacleSafeDistanceMapToString()->c_str());
    robotDataString.append( "\npath\n");
    robotDataString.append(aStar->pathToString());
    robotDataString.append("\npose\n");
    robotDataString.append(localization->getPoseLastString()->c_str());
    robotDataString.append("\nscan\n");
    robotDataString.append(gripMap->scanEndPointsToString(sensorData->getScanPolarForm(),
        localization->getPose())->c_str());
    robotDataString.append("\nmatchedlines\n");
    robotDataString.append(matchedlines->toString()->c_str());
    if(unmatchedlines->size()>0){
        robotDataString.append("\nunmatchedlines\n");
        robotDataString.append(unmatchedlines->toString()->c_str());
    }
    robotDataString.append("\nmaplines\n");
    robotDataString.append(maplines->toString()->c_str());
    auto robotDataMessage = std_msgs::msg::String();
    robotDataMessage.data = robotDataString.c_str();
    gridMapPublisher_->publish(robotDataMessage);
}

void MissionController::resetRobotData() const {
    const string robotDataString("reset");
    auto robotDataMessage = std_msgs::msg::String();
    robotDataMessage.data = robotDataString;
    gridMapPublisher_->publish(robotDataMessage);
}

void MissionController::printMap() {
    Lines * mapLines = localization->getMeasurementPrediction()->getMapLines();
    for(int i=0; i<mapLines->size(); i++) {
        Line * line = mapLines->getLine(i);
        //cout << line->getM() << ";" << line->getB() << " ";
        //cout << line->getAlfa() << ";" << line->getR() << " ";
        for(int j=0; j<line->getPointCount(); j++) {
            cout << line->getPoint(j)->getX() <<";"<< line->getPoint(j)->getY() << " ";
        }
        cout << endl;
        /*
        Line * line = mapLines->getLine(i);
        cout << line->getAlfa() <<" "<< line->getR() <<" "<< line->getM() <<" "<< line->getB() <<" "<<
            line->getFirstPoint()->getX() <<" "<<
            line->getFirstPoint()->getY() <<" "<<
            line->getLastPoint()->getX()  <<" "<<
            line->getLastPoint()->getY()  << endl;
        */
    }
}
