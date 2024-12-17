//
// Created by robot1 on 2/4/24.
//

#include "RecordHandler.h"

using namespace std;

RecordHandler::RecordHandler() {
    recording = new std::vector<SensorRecord>;
    LoadRecords();
}

void RecordHandler::LoadRecords(){
    recordNum = 0;
    string newMeasurementId = "new";
    std::vector<PointPolarForm> *scan = new std::vector<PointPolarForm>;
    ifstream file;
    //string filePath =  filesystem::current_path();
    //filePath.append("/TestRecord2.txt");
    //cout << filesystem::current_path() << "%%%%%% " << filePath << endl;
    file.open(TEST_CONFIG_RESOURCE_FILE_RECORD);
    float angle;
    float distance;
    float posLeft;
    float posRight;
    bool newMeasurement=false;
    if(file.is_open()){
        string line;
        while(getline(file,line)){
            //       cout << linePoints << endl;
            if(line.find(newMeasurementId) != std::string::npos){
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
}

void RecordHandler::update(SensorData * sensorData){
    SensorRecord * record = &recording->at(recordNum++);
    sensorData->update(record->getScan(),
                       record->getPosLeft(),
                       record->getPosRight());
}

void RecordHandler::getValuesFromLine(string line, float &val1, float &val2){
    int val1Size =line.find(",");
    string sa = line.substr(0, val1Size);
    val1 = getValueFromString(sa);
    int val2Size = line.size() - (val1Size + 1);
    string sd = line.substr(val1Size + 1, val2Size);
    val2 = getValueFromString(sd);
}

float RecordHandler::getValueFromString(string s){
    string infIdentifier = "inf";
    if(s.find(infIdentifier) != std::string::npos){
        return std::numeric_limits<float>::infinity();
    }else{
        return stod(s);
    }
}


bool RecordHandler::hasRecordsToProcess() {
    return recordNum<recording->size();
}


