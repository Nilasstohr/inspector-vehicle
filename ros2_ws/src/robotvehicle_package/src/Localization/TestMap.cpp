//
// Created by robot1 on 9/30/23.
//

#include "TestMap.h"

TestMap::TestMap() {

    double d = 124.4;
    double t = 1.6;
    double uncertainty = 0.1;
    Line line1;
    line1.addRecPoint(0,uncertainty);
    line1.addRecPoint(d,-uncertainty);
    Line line2;
    line2.addRecPoint(d+uncertainty,0);
    line2.addRecPoint(d,d-2*t);
    Line line3;
    line3.addRecPoint(0,d-2*t+uncertainty);
    line3.addRecPoint(d,d-2*t);
    Line line4;
    line4.addRecPoint(0+uncertainty,0);
    line4.addRecPoint(0,d-2*t);
    line1.updateOriginLineNormal();
    line2.updateOriginLineNormal();
    line3.updateOriginLineNormal();
    line4.updateOriginLineNormal();
    lineStack.add(&line1);
    lineStack.add(&line2);
    lineStack.add(&line3);
    lineStack.add(&line4);
}

LineStack *TestMap::getMap() {
    return &lineStack;
}