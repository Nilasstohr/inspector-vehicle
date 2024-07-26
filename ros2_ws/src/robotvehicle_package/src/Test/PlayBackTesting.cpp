//
// Created by robot1 on 2/4/24.
//

#include "PlayBackTesting.h"
#include <iostream>
#include "../Utilities/AwaitTimer.h"
#include "../PathPlanning/GridMap.h"

PlayBackTesting::PlayBackTesting() {
    gripMap = new GridMap(CONFIG_GRID_VALUE_FULL_AVAILABLE,
                          CONFIG_GRID_VALUE_FULL_OCCUPIED,
                          CONFIG_GRID_VALUE_UPDATE_INTERVAL);
    gripMap->loadGridMap();
    localization = new KalmanLocalization(nullptr);
    recording = new std::vector<SensorRecord>;
    hasMapBeenBuild=false;
    LoadRecords();

    // kalman filter

    //kalmanFilter->build(scan);

    //kalmanFilter->build(sensorLogger[0]->getScan());
    int i=0;
    //AwaitTimer *awaitTimer = new AwaitTimer();
    //while(sensorLogger[i]!=NULL && i<sizeof(sensorLogger)){
        //cout << "\n*********************" << i+1 << "***************************\n";
        //if(i+1==26)
        //    cout << "stop at: " << i+1 << endl;
        //awaitTimer->reset();
        //kalmanFilter->update(
        //        sensorLogger[i]->getPosLeft(),
        //        sensorLogger[i]->getPosRight(),sensorLogger[i]->getScan(),true);
        //cout << "update time kalman: " << awaitTimer->getElapsedTimeMillis() << <<endl;
        i++;


    //}
}

void PlayBackTesting::LoadRecords(){
    std::vector<PointPolarForm> *scan = new std::vector<PointPolarForm>;
    ifstream file;
    file.open("../../../doc/Measurements/realtime/record.txt");
    float angle;
    float distance;
    float posLeft;
    float posRight;
    bool newMeasurement=false;
    if(file.is_open()){
        string line;
        while(getline(file,line)){
            //       cout << linePoints << endl;
            if(line.find('new') != std::string::npos){
                newMeasurement=true;
                if(!scan->empty()){
                    recording->push_back(SensorRecord(scan,posLeft,posRight));
                    scan->clear();
                }
                continue;
            }
            if(newMeasurement){
                getValuesFromLine(line, posLeft, posRight);
                newMeasurement=false;
                continue;
            }
            getValuesFromLine(line,angle,distance);
            if(!isinf(distance))
                scan->push_back(PointPolarForm(angle, distance));
        }
    }else{
        printf("file open failed");
        return;
    }
    file.close();
    /*
    while(!recording->empty()){
         cout << "new" << endl;
         cout << recording->begin()->getPosLeft() << "," << recording->begin()->getPosRight() << endl;

         cout << recording->begin()->getScan()->at(50).getAngle() << ","
              << recording->begin()->getScan()->at(50).getDistance() << endl;

         cout << recording->begin()->getScan()->at(51).getAngle() << ","
              << recording->begin()->getScan()->at(51).getDistance() << endl;

         recording->erase(recording->begin());
    }
    */
}

void PlayBackTesting::update(){
    if(!hasMapBeenBuild){
        localization->build(recording->begin()->getScan());
        hasMapBeenBuild=true;
    }
   localization->update(recording->begin()->getScan(),
                         recording->begin()->getPosLeft(),
                         recording->begin()->getPosRight());
    //cout << *localization->getPoseLastString() << endl;
    gripMap->update(recording->begin()->getScan(),localization->getPose());
    recording->erase(recording->begin());
}

void PlayBackTesting::getValuesFromLine(string line, float &val1, float &val2){
    int val1Size =line.find(",");
    string sa = line.substr(0, val1Size);
    val1 = getValueFromString(sa);
    int val2Size = line.size() - (val1Size + 1);
    string sd = line.substr(val1Size + 1, val2Size);
    val2 = getValueFromString(sd);
}

float PlayBackTesting::getValueFromString(string s){
    if(s.find('inf') != std::string::npos){
        return std::numeric_limits<float>::infinity();
    }else{
        return stod(s);
    }
}

GridMap *PlayBackTesting::getGripMap() {
    return gripMap;
}

bool PlayBackTesting::hasRecordsToProcess() {
    return !recording->empty();
}

KalmanLocalization *PlayBackTesting::getLocalization() {
    return localization;
}
