//
// Created by robot1 on 2/4/24.
//

#include "TestKalmanFilterOffLine.h"
#include <iostream>
#include "Utilities/AwaitTimer.h"

TestKalmanFilterOffLine::TestKalmanFilterOffLine() {
    OdomRangeLog  * sensorLogger[160000];
    std::vector<PointPolarForm> *scan = new std::vector<PointPolarForm>;
    ifstream file;
    file.open("/home/robot1/devrepo/inspector-vehicle/ros2_ws/build/robotvehicle_package/record2.txt");
    float angle;
    float distance;
    float posLeft;
    float posRight;
    int logCount=0;
    bool newMeasurement=false;
    if(file.is_open()){
        string line;
        while(getline(file,line)){
     //       cout << line << endl;
            if(line.find('new') != std::string::npos){
                newMeasurement=true;
                if(!scan->empty()){
                    sensorLogger[logCount] = new OdomRangeLog(posLeft, posRight, scan);
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
    KalmanFilter *kalmanFilter = new KalmanFilter();
    kalmanFilter->build(sensorLogger[0]->getScan());
    int i=0;
    //AwaitTimer *awaitTimer = new AwaitTimer();
    while(sensorLogger[i]!=NULL && i<sizeof(sensorLogger)){
        //cout << "\n*********************" << i+1 << "***************************\n";
        //if(i+1==26)
        //    cout << "stop at: " << i+1 << endl;
        //awaitTimer->reset();
        kalmanFilter->update(
                sensorLogger[i]->getPosLeft(),
                sensorLogger[i]->getPosRight(),sensorLogger[i]->getScan(),true);
        //cout << "update time kalman: " << awaitTimer->getElapsedTimeMillis() << <<endl;
        i++;


    }
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
