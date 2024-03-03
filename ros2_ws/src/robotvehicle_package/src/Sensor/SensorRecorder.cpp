//
// Created by robot1 on 2/26/24.
//

#include <fstream>
#include "SensorRecorder.h"

SensorRecorder::SensorRecorder() {
    awaitTimer = new AwaitTimer();
}

void SensorRecorder::startRecord(uint64_t seconds) {
    out = std::ofstream("../../../doc/Measurements/realtime/record.txt");
    awaitTimer->setTimeout(seconds,AwaitTimer::TimeUnit::seconds);
    awaitTimer->start();
}

void SensorRecorder::writeNewRecord(double wheelTravelLeft, double wheelTravelRight) {
    writeNewKeyWord();
    out << wheelTravelLeft << "," << wheelTravelRight << std::endl;
}

void SensorRecorder::writeScanPoint(float scanAngleRad, float scanDistanceCm) {
    out << scanAngleRad << "," << scanDistanceCm << std::endl;
}

void SensorRecorder::endRecord() {
    writeNewKeyWord();
    out.close();
}

bool SensorRecorder::hasRecordTimeExceeded() {
    return awaitTimer->hasWaitingTimeExceeded();
}

void SensorRecorder::writeNewKeyWord(){
    out << "new" << std::endl;
}



