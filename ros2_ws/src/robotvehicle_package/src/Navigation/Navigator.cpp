//
// Created by robot1 on 3/19/24.
//

#include <iostream>
#include "Navigator.h"

#define GOAL_ACCEPTANCE_TRESHOLD_CM 5
#define MIN_W_NORMAL 1
#define MAX_W_NORMAL 4
#define MIN_W_TURN -4

#define DEG2RAD(x) ((x)*M_PI/180)

Navigator::Navigator(DriverInterface *driverInterface):
        driverInterface(driverInterface){
    l=14.5;
    r=4.5;
    w=0;
    v=0;
    int G=2;
    K_ro = 0.35*G;
    K_alfa = 0.5*3;
}

void Navigator::update(KalmanLocalization * localization) {
    if(destinationReached){
        return;
    }
    xt = localization->getPose();
    xtGoal = &navigationPath->getPath()->at(navigationPointIndex);
    //cout << "in: " << xt->getX() << " " << xt->getY() << " " << xt->getTheta() << endl;
    dx = xtGoal->getX() - xt->getX();
    dy = xtGoal->getY() - xt->getY();

    if(abs(dx) < GOAL_ACCEPTANCE_TRESHOLD_CM && abs(dy) < GOAL_ACCEPTANCE_TRESHOLD_CM){
        //! static_cast<long unsigned int>(navigationPointIndex)> is satisfiying compile warining
        if(static_cast<long unsigned int>(navigationPointIndex)>=navigationPath->getPath()->size()-1){
            destinationReached = true;
            driverInterface->stopAndResetDisplacement();
            return;
        }
        //cout << navigationPointIndex << " out of " << navigationPath->getPath()->size()-1 << endl;
        navigationPointIndex++;
        return;
    }

    ro = sqrt(pow(dx,2)+pow(dy,2));
    alfa = -xt->getTheta() + atan2(dy,dx);
    if(dx < 0 && dy < 0){
        alfa+=2*M_PI;
    }

    if(abs(alfa) > DEG2RAD(20)){
        v=0;
        setLowerVelocityLimit(MIN_W_TURN);
    }else{
        v= K_ro * ro;
        setLowerVelocityLimit(MIN_W_NORMAL);
    }

    w= K_alfa * alfa;
    wl = double(v-w*l)/r;
    wr = double(v+w*l)/r;

    if(wl < wMin)
        wl=wMin;
    else if(wl > MAX_W_NORMAL)
        wl=MAX_W_NORMAL;

    if(wr < wMin)
        wr=wMin;
    else if(wr > MAX_W_NORMAL)
        wr=MAX_W_NORMAL;
    //cout << "wl= "<< wl << " wr= " << wr  << endl;
    driverInterface->setAngularVelocity(wl,wr);
}

void Navigator::update() {
    if(destinationReached){
        return;
    }
    xt = new Pose();
    xt->update(40.156, 40.244, 0.000284356);
    xtGoal = &navigationPath->getPath()->at(navigationPointIndex);
    cout << "in: " << xt->getX() << " " << xt->getY() << " " << xt->getTheta() << endl;
    dx = xtGoal->getX() - xt->getX();
    dy = xtGoal->getY() - xt->getY();

    if(abs(dx) < 2 && abs(dy) < 2){
        if(static_cast<long unsigned int>(navigationPointIndex)>=navigationPath->getPath()->size()){
            destinationReached = true;
            stopAndResetDisplacement();
            return;
        }
        navigationPointIndex++;
        return;
    }

    ro = sqrt(pow(dx,2)+pow(dy,2));
    //if(dy<0){
    //    alfa = -xt->getTheta() + atan2(dy,dx)+2*M_PI;
    //}else{
        alfa = -xt->getTheta() + atan2(dy,dx);
        if(alfa < -3*M_PI_2){
            // direction of robot is in four qradant
            alfa = 2*M_PI - xt->getTheta() + atan2(dy,dx);
        }
    //}
    //  40.156 40.244 0.000284356
    v= K_ro * ro;
    w= K_alfa * alfa;
    wl = double(v-w*l)/r;
    wr = double(v+w*l)/r;
    cout << "out: " << wl << " " << wr << endl;
    if(wr > 10){
        cout << "faulty output" << endl;
        return;
    }
    driverInterface->setAngularVelocity(wl,wr);
}

void Navigator::setNavigationPath(NavigationPath *navigationPath) {
    this->navigationPath = navigationPath;
    navigationPointIndex = 0 ;
    destinationReached =false;
}

bool Navigator::isDestinationReached() {
    return destinationReached;
}

void Navigator::setLowerVelocityLimit(double wMin) {
    this->wMin = wMin;
}

void Navigator::stopAndResetDisplacement() {
    driverInterface->stopAndResetDisplacement();
}

void Navigator::backwardSlow() {
    driverInterface->setAngularVelocity(-MAX_W_NORMAL,-MAX_W_NORMAL);
}

void Navigator::forwardSlow() {
    driverInterface->setAngularVelocity(MAX_W_NORMAL,MAX_W_NORMAL);
}

void Navigator::stop() {
    driverInterface->stop();
}

NavigationPath * Navigator::getNavigationPath() {
    return navigationPath;
}
