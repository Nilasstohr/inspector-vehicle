//
// Created by robot1 on 8/22/24.
//

#include "MissionController.h"
#include "rclcpp/rclcpp.hpp"
#define ROS_INFO RCUTILS_LOG_INFO

MissionController::MissionController(rclcpp::Node * node, SerialInterface &serialInterface):
driverInterface(serialInterface),
navigator(driverInterface),
gripMap(GridMap(CONFIG_GRID_VALUE_FULL_AVAILABLE,
                          CONFIG_GRID_VALUE_FULL_OCCUPIED,
                          CONFIG_GRID_VALUE_UPDATE_INTERVAL)),
linear_x(0),
angular_z(0)
{
    obstacleAvoidanceInProgress=false;
    hasMapBeenBuild=false;
    missionComplete = false;
    localization = KalmanLocalization();
    odom = Odom();
    aStar = AStar();
    //gripMap->loadGridMap();
    if (serialInterface.hasResponse()) {
        ROS_INFO(serialInterface.getResponse()->c_str());
    }
    posePublisher_ = node->create_publisher<std_msgs::msg::String>("topic_pose_string", 10);
    gridMapPublisher_ = node->create_publisher<std_msgs::msg::String>("topic_grid_map", 10);
    serialInterface.sendRequest("r");
}

Navigator & MissionController::getNavigator(){
    return navigator;
}

void MissionController::updateMapWithObstacleSafeDistance() {
    gripMap.updateMapWithObstacleSafeDistance2(
       localization.getObservations()->getLines(),localization.getPose());
}

void MissionController::generateNewPathToDestination(){
    const int x = missionPath->getCurrentGoToPoint()->getX();
    const int y = missionPath->getCurrentGoToPoint()->getY();
    auto *endPoint = new PathPoint();
    endPoint->set(x,y);
    //try {
        aStar.update(localization.getPose(),endPoint,gripMap.getMapWithSafetyDistance());
    //}   catch (const std::exception& e) {
    //    cout << "A* pathfinding exception: " << e.what() << endl;
    //}
    navigator.setNavigationPath(aStar.getNavigationPath());
}

void MissionController::build(const std::vector<PointPolarForm> & lidarScanPolarPoints) {
    localization.build(lidarScanPolarPoints);
    gripMap.updateMapWithObstacleSafeDistance2(
       localization.getMeasurementPrediction().getObservations()->getLines(),localization.getStarPose());
    generateNewPathToDestination();
    hasMapBeenBuild = true;
}

void MissionController::update(const std::vector<PointPolarForm> & lidarScanPolarPoints, double posLeft, double posRight) {
    if (!hasMapBeenBuild) {
        build(lidarScanPolarPoints);
    }
    localization.update(lidarScanPolarPoints,posLeft,posRight);
    //odom->update(sensorData->getPosLeft(),sensorData->getPosRight());
    //navigator->setNav2Velocities(angular_z,linear_x);
    updateMapAndPath(lidarScanPolarPoints,localization.getPose());

    // ./StartLidar.sh
    // ./StartSlamTF.sh
    // ./StartSlamToolBoxMapping.sh
    // ./StoreSlamToolboxMapNav2.sh
    // ./StartSlamToolBoxLocalization.sh


    navigator.update(localization);

    if(navigator.isDestinationReached()) {
        if(missionPath->isNextPointAvailable()) {
            missionPath->setNextGoToPoint();
            generateNewPathToDestination();
        }else {
            navigator.stopAndResetDisplacement();
            missionComplete=true;
        }
    }

    // this used for larger gripmap, because transmitting large data takes time
    //if(!navigator->validNavigationPath()) {
    //    publishRobotData();
    //

    //posMessage.data = localization->getPoseLastString()->c_str();
    //posePublisher_->publish(posMessage);
}


void MissionController::updateMapAndPath(const vector<PointPolarForm> & lidarScanPolarPoints, Pose *pose) {
    gripMap.update(lidarScanPolarPoints,pose);
    updateMapWithObstacleSafeDistance();
    if(gripMap.isPathBlocked(navigator.getNavigationPath())) {
        generateNewPathToDestination();
    }
}

double MissionController::getCurrentPoseX() {
    return localization.getPose()->getX();
}
double MissionController::getCurrentPoseY() {
    return localization.getPose()->getY();
}


void MissionController::endMission(){
    posMessage.data = "end";
    posePublisher_->publish(posMessage);
    //gripMap->storeMap();
    std::this_thread::sleep_for(200ms);
    cout << "ending run" << endl;
}

void MissionController::publishRobotData(const std::vector<PointPolarForm> & lidarScanPolarPoints) {
    Lines * matchedlines = localization.getMatching().getMatchedLines()->toGlobalReferenceFrame(localization.getPose());
    Lines * unmatchedlines = localization.getMatching().getUnMatchedLines()->toGlobalReferenceFrame(localization.getPose());
    Lines * maplines = localization.getMeasurementPrediction().getMapLines();
    string robotDataString;
    robotDataString.append(gripMap.obstacleSafeDistanceMapToString()->c_str());
    if(!aStar.getNavigationPath()->getPath()->empty()) {
        robotDataString.append( "\npath\n");
        robotDataString.append(aStar.pathToString());
    }
    robotDataString.append("\npose\n");
    robotDataString.append(localization.getPoseLastString()->c_str());
    robotDataString.append("\nscan\n");
    robotDataString.append(gripMap.scanEndPointsToString(lidarScanPolarPoints,
        localization.getPose())->c_str());
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
    Lines * mapLines = localization.getMeasurementPrediction().getMapLines();
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

const KalmanLocalization & MissionController::getLocalization() const {
    return localization;
}

const Odom * MissionController::getOdom() const {
    return &odom;
}

void MissionController::setCmdVel(const double linear_x, const double angular_z) {
    this->linear_x = linear_x;
    this->angular_z = angular_z;
}

double MissionController::getLinearX() const {
    return linear_x;
}
double MissionController::getAngularZ() const {
    return angular_z;
}
