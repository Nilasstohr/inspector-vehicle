//
// Created by robot1 on 2/4/24.
//

#ifndef ROBOTVEHICLE_PACKAGE_RECORDHANDLER_H
#define ROBOTVEHICLE_PACKAGE_RECORDHANDLER_H

#include "../Sensor/PointRectForm.h"
#include "../Localization/KalmanLocalization.h"
#include "../PathPlanning/GridMap.h"
#include "SensorRecord.h"
#include <fstream>
using namespace std;

class RecordHandler {
public:
    RecordHandler();
    bool hasRecordsToProcess();
    void update(SensorData *sensorData);

private:
    vector<SensorRecord> *recording;
    void LoadRecords();
    void getValuesFromLine(string line, float &val1,float &val2);
    float getValueFromString(string s);
};


#endif //ROBOTVEHICLE_PACKAGE_RECORDHANDLER_H
