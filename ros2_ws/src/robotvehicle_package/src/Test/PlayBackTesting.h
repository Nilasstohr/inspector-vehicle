//
// Created by robot1 on 2/4/24.
//

#ifndef ROBOTVEHICLE_PACKAGE_PLAYBACKTESTING_H
#define ROBOTVEHICLE_PACKAGE_PLAYBACKTESTING_H

#include "../Sensor/PointRectForm.h"
#include "../Localization/KalmanLocalization.h"
#include "../PathPlanning/GridMap.h"
#include "SensorRecord.h"
#include <fstream>
using namespace std;

class PlayBackTesting {
public:
    PlayBackTesting();
    GridMap * getGripMap();
    KalmanLocalization * getLocalization();
    bool hasRecordsToProcess();
    void update();
private:
    vector<SensorRecord> *recording;
    GridMap *gripMap;
    KalmanLocalization * localization;
    bool hasMapBeenBuild;
    void LoadRecords();
    void getValuesFromLine(string line, float &val1,float &val2);
    float getValueFromString(string s);
};


#endif //ROBOTVEHICLE_PACKAGE_PLAYBACKTESTING_H
