//
// Created by robot1 on 2/4/24.
//

#ifndef ROBOTVEHICLE_PACKAGE_RECORDHANDLER_H
#define ROBOTVEHICLE_PACKAGE_RECORDHANDLER_H

#include "PathPlanning/GridMap.h"
#include "Localization/KalmanLocalization.h"
#include "SensorRecord.h"
#include "TestConfigurations.h"

using namespace std;

class RecordHandler {
public:
    RecordHandler();
    bool hasRecordsToProcess();
    void update(SensorData *sensorData);

private:
    vector<SensorRecord> *recording;
    long unsigned int recordNum;
    void LoadRecords();
    void getValuesFromLine(string line, float &val1,float &val2);
    float getValueFromString(string s);
};


#endif //ROBOTVEHICLE_PACKAGE_RECORDHANDLER_H
