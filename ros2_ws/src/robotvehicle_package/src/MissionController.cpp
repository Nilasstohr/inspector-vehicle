//
// Created by robot1 on 8/22/24.
//

#include "MissionController.h"
#include "rclcpp/rclcpp.hpp"
#define ROS_INFO RCUTILS_LOG_INFO

MissionController::MissionController(rclcpp::Node * node,SerialInterface * serialInterface) {
    obstacleAvoidanceInProgress=false;
    hasMapBeenBuild=false;
    missionComplete = false;
    auto *driverInterface = new DriverInterface(serialInterface);
    sensorData = new SensorData(driverInterface);
    localization = new KalmanLocalization();
    odom = new Odom();
    gripMap = new GridMap(CONFIG_GRID_VALUE_FULL_AVAILABLE,
                          CONFIG_GRID_VALUE_FULL_OCCUPIED,
                          CONFIG_GRID_VALUE_UPDATE_INTERVAL);
    navigator = new Navigator(driverInterface);
    aStar = new AStar();
    //gripMap->loadGridMap();
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

void MissionController::updateMapWithObstacleSafeDistance() {
    gripMap->updateMapWithObstacleSafeDistance2(
       localization->getObservations()->getLines(),localization->getPose());
}

void MissionController::generateNewPathToDestination(){
    const int x = missionPath->getCurrentGoToPoint()->getX();
    const int y = missionPath->getCurrentGoToPoint()->getY();
    auto *endPoint = new PathPoint();
    endPoint->set(x,y);
    //try {
        aStar->update(localization->getPose(),endPoint,gripMap->getMapWithSafetyDistance());
    //}   catch (const std::exception& e) {
    //    cout << "A* pathfinding exception: " << e.what() << endl;
    //}
    navigator->setNavigationPath(aStar->getNavigationPath());
}

void MissionController::build() {
    localization->build(sensorData);
    gripMap->updateMapWithObstacleSafeDistance2(
       localization->getMeasurementPrediction()->getObservations()->getLines(),localization->getStarPose());
    generateNewPathToDestination();
    hasMapBeenBuild = true;
}

void MissionController::update(){
    //if (!hasMapBeenBuild) {
    //   build();
   // }
    //localization->update(sensorData);
    odom->update(sensorData->getPosLeft(),sensorData->getPosRight());
    //updateMapAndPath(sensorData->getScanPolarForm(),localization->getPose());

    // ./StartLidar.sh
    // ./StartSlamTF.sh
    // ./StartSlamToolBoxMapping.sh
    // ./StoreSlamToolboxMapNav2.sh
    // ./StartSlamToolBoxLocalization.sh

/*
    navigator->update(localization);

    if(navigator->isDestinationReached()) {
        if(missionPath->isNextPointAvailable()) {
            missionPath->setNextGoToPoint();
            generateNewPathToDestination();
        }else {
            navigator->stopAndResetDisplacement();
            missionComplete=true;
        }
    }
*/



    // this used for larger gripmap, because transmitting large data takes time
    //if(!navigator->validNavigationPath()) {
    //    publishRobotData();
    //}

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
    if(gripMap->isPathBlocked(navigator->getNavigationPath())) {
        generateNewPathToDestination();
    }
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
    if(!aStar->getNavigationPath()->getPath()->empty()) {
        robotDataString.append( "\npath\n");
        robotDataString.append(aStar->pathToString());
    }
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

bool MissionController::isMissionComplete() const {
    return missionComplete;
}

void MissionController::setMissionPath(NavigationPath * missionPath) {
    this->missionPath = missionPath;
}

KalmanLocalization *MissionController::getLocalization() const {
    return localization;
}

Odom * MissionController::getOdom() {
    return odom;
}
