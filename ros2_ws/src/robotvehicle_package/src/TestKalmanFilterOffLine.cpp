//
// Created by robot1 on 2/4/24.
//

#include "TestKalmanFilterOffLine.h"
#include <iostream>
#include "Utilities/AwaitTimer.h"
#include "PathPlanning/GridMap.h"

TestKalmanFilterOffLine::TestKalmanFilterOffLine() {
    //OdomRangeLog  * sensorLogger[160000];
    std::vector<PointPolarForm> *scan = new std::vector<PointPolarForm>;
    ifstream file;
    file.open("../../../doc/Measurements/realtime/record_cp_live1.txt");
    float angle;
    float distance;
    float posLeft;
    float posRight;
    int logCount=0;
    bool newMeasurement=false;
    if(file.is_open()){
        string line;
        while(getline(file,line)){
     //       cout << linePoints << endl;
            if(line.find('new') != std::string::npos){
                newMeasurement=true;
                if(!scan->empty()){
                    //sensorLogger[logCount] = new OdomRangeLog(posLeft, posRight, scan);
                    break;
                    scan->clear();
                    logCount++;
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

    // kalman filter

    KalmanLocalization *kalmanFilter = new KalmanLocalization(nullptr);

    gripMap = new GridMap(CONFIG_GRID_VALUE_FULL_AVAILABLE,
                                   CONFIG_GRID_VALUE_FULL_OCCUPIED,
                                   CONFIG_GRID_VALUE_UPDATE_INTERVAL);
    Pose * pose = new Pose();
    pose->update(40,40,0);
    gripMap->update(scan, pose);

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

void TestKalmanFilterOffLine::getValuesFromLine(string line, float &val1,float &val2){
    int val1Size =line.find(",");
    string sa = line.substr(0, val1Size);
    val1 = getValueFromString(sa);
    int val2Size = line.size() - (val1Size + 1);
    string sd = line.substr(val1Size + 1, val2Size);
    val2 = getValueFromString(sd);
}

float TestKalmanFilterOffLine::getValueFromString(string s){
    if(s.find('inf') != std::string::npos){
        return std::numeric_limits<float>::infinity();
    }else{
        return stod(s);
    }
}

GridMap *TestKalmanFilterOffLine::getGripMap() {
    return gripMap;
}
