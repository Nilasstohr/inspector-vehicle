//
// Created by robot1 on 2/26/24.
//

#ifndef ROBOTVEHICLE_PACKAGE_SENSORRECORDER_H
#define ROBOTVEHICLE_PACKAGE_SENSORRECORDER_H

#include <fstream>

#include "Utilities/AwaitTimer.h"
#include "KalmanLocalization.h"

class SensorRecorder {
public:
    SensorRecorder();
    void startRecord(uint64_t seconds);
    void endRecord();
    bool hasRecordTimeExceeded();
    void update(SensorData *sensorData);
private:
    std::ofstream out;
    AwaitTimer * awaitTimer;
    void writeNewRecord(double wheelTravelLeft, double wheelTravelRight);
    void writeScanPoint(float scanAngleRad, float scanDistanceCm);
    void writeNewKeyWord();


};


#endif //ROBOTVEHICLE_PACKAGE_SENSORRECORDER_H
