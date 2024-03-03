//
// Created by robot1 on 2/4/24.
//

#ifndef ROBOTVEHICLE_PACKAGE_TESTKALMANFILTEROFFLINE_H
#define ROBOTVEHICLE_PACKAGE_TESTKALMANFILTEROFFLINE_H

#include "Sensor/OdomRangeLog.h"
#include "Sensor/PointRectForm.h"
#include "Localization/KalmanFilter.h"
#include <fstream>
using namespace std;

class TestKalmanFilterOffLine {
public:
    TestKalmanFilterOffLine();
    void getValuesFromLine(string line, float &val1,float &val2);

    float getValueFromString(string s);
};


#endif //ROBOTVEHICLE_PACKAGE_TESTKALMANFILTEROFFLINE_H
