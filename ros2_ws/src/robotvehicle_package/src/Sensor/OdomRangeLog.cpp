//
// Created by robot1 on 8/10/23.
//

#include "OdomRangeLog.h"
#define M_PI 3.1415926535897932384626433832795
#define RAD2DEG(x) ((x)*180./M_PI)

OdomRangeLog::OdomRangeLog(double posLeft, double posRight, sensor_msgs::msg::LaserScan::SharedPtr scan):
posLeft(posLeft),posRight(posRight){
    this->scan = new std::vector<LaserScanPoint>;
    int count = scan->scan_time / scan->time_increment;
    float angle;
    float distance;
    for(int i = 0; i < count; i++) {
        angle = RAD2DEG(scan->angle_min + scan->angle_increment * i);
        distance = scan->ranges[i];
        this->scan->push_back(LaserScanPoint(angle, distance));
        //ROS_INFO(": [% i,%f, %f]",i, degree, scan->ranges[i]);
    }
}

double OdomRangeLog::getPosLeft() const {
    return posLeft;
}

double OdomRangeLog::getPosRight() const {
    return posRight;
}

std::vector<LaserScanPoint> *OdomRangeLog::getScan() const {
    return scan;
}
