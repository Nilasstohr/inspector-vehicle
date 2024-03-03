//
// Created by robot1 on 2/26/24.
//

#ifndef ROBOTVEHICLE_PACKAGE_SENSORRECORDER_H
#define ROBOTVEHICLE_PACKAGE_SENSORRECORDER_H


#include "PointPolarForm.h"
#include "../Utilities/AwaitTimer.h"

class SensorRecorder {
public:
    SensorRecorder();
    void startRecord(uint64_t seconds);
    void writeNewRecord(double wheelTravelLeft, double wheelTravelRight);
    void writeScanPoint(float scanAngleRad, float scanDistanceCm);
    void endRecord();
    bool hasRecordTimeExceeded();
private:
    std::ofstream out;
    AwaitTimer * awaitTimer;
    void writeNewKeyWord();
};


#endif //ROBOTVEHICLE_PACKAGE_SENSORRECORDER_H
