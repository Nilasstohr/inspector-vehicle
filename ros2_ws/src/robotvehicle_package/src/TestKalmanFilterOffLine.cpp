//
// Created by robot1 on 2/4/24.
//

#include "TestKalmanFilterOffLine.h"
#include <iostream>

TestKalmanFilterOffLine::TestKalmanFilterOffLine() {
    OdomRangeLog  * sensorLogger[160000];
    std::vector<PointPolarForm> *scan = new std::vector<PointPolarForm>;
    ifstream file;
    file.open("/home/robot1/devrepo/inspector-vehicle/ros2_ws/src/robotvehicle_package/src/log8.txt");
    float angle;
    float distance;
    float posLeft;
    float posRight;
    int logCount=0;
    bool newMeasurement=false;
    if(file.is_open()){
        string line;
        while(getline(file,line)){
            //cout << line << endl;
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
    while(sensorLogger[i]!=NULL && i<sizeof(sensorLogger)){
        //cout << "\n*********************" << i+1 << "***************************\n";
        //if(i+1==26)
        //    cout << "stop at: " << i+1 << endl;
        kalmanFilter->update(
                sensorLogger[i]->getPosLeft(),
                sensorLogger[i]->getPosRight(),sensorLogger[i]->getScan(),true);
        i++;

    }
}

void TestKalmanFilterOffLine::getValuesFromLine(string line, float &val1,float &val2){
    int sepPos =line.find(",");
    string sa = line.substr(0, sepPos);
    val1 = stod(sa);
    int sizeDistance = line.size() - (sepPos+1);
    string sd = line.substr(sepPos+1,sizeDistance-1);
    val2 = stod(sd);

}
