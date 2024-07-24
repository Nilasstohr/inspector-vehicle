//
// Created by robot1 on 2/26/24.
//

#include <fstream>
#include <iostream>
#include "SensorRecorder.h"

SensorRecorder::SensorRecorder() {
    awaitTimer = new AwaitTimer();
}

void SensorRecorder::startRecord(uint64_t seconds) {
    out = std::ofstream("../../../doc/Measurements/realtime/record.txt");
    awaitTimer->setTimeout(seconds,AwaitTimer::TimeUnit::seconds);
    awaitTimer->start();
}

void SensorRecorder::update(SensorData *sensorData) {
    /*
    cout << "new" << endl;
    cout << sensorData->getPosLeft() << "," << sensorData->getPosRight() << endl;
    cout << sensorData->getScanPolarForm()->at(50).getAngle() << ","
         << sensorData->getScanPolarForm()->at(50).getDistance() << endl;
    cout << sensorData->getScanPolarForm()->at(51).getAngle() << ","
         << sensorData->getScanPolarForm()->at(51).getDistance() << endl;
    */
    writeNewRecord(sensorData->getPosLeft(),sensorData->getPosRight());
    for(PointPolarForm scanPoint: *sensorData->getScanPolarForm()){
        writeScanPoint(scanPoint.getAngle(),scanPoint.getDistance());
    }
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




