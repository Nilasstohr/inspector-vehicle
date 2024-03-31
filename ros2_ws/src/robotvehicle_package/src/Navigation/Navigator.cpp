//
// Created by robot1 on 3/19/24.
//

#include <iostream>
#include "Navigator.h"

#define GOAL_ACCEPTANCE_TRESHOLD 4

Navigator::Navigator(DriverInterface *driverInterface):
        driverInterface(driverInterface){
    l=14.5;
    r=4.5;
    w=0;
    v=0;
    int G=3;
    K_ro = 0.35*G;
    K_alfa = 0.5*G;
    K_beta = -0.325;
    //K_beta = -0.325*G
}

void Navigator::update(KalmanLocalization * localization) {
    if(distinationReached){
        return;
    }
    xt = localization->getPose();
    xtGoal = &navigationPath->getPath()->at(navigationPointIndex);
    cout << "in: " << xt->getX() << " " << xt->getY() << " " << xt->getTheta() << endl;
    dx = xtGoal->getX() - xt->getX();
    dy = xtGoal->getY() - xt->getY();

    if(abs(dx) < GOAL_ACCEPTANCE_TRESHOLD && abs(dy) < GOAL_ACCEPTANCE_TRESHOLD){
        if(navigationPointIndex>=navigationPath->getPath()->size()-1){
            distinationReached = true;
            driverInterface->stop();
            return;
        }
        navigationPointIndex++;
        return;
    }

    ro = sqrt(pow(dx,2)+pow(dy,2));
    alfa = -xt->getTheta() + atan2(dy,dx);

/*
    if(dy<0){
        alfa = -xt->getTheta() + atan2(dy,dx)+2*M_PI;
    }else{
        alfa = -xt->getTheta() + atan2(dy,dx);
        if(alfa < -3*M_PI_2){
            // direction of robot is in four qradant
            alfa = 2*M_PI - xt->getTheta() + atan2(dy,dx);
        }
    }
*/
    //  40.156 40.244 0.000284356
    beta = -xt->getTheta() - alfa;
    v= K_ro * ro;
    w= K_alfa * alfa; //+ beta*K_beta;
    wl = double(v-w*l)/r;
    wr = double(v+w*l)/r;
    if(wl<1)
        wl=1;
    if(wr<1)
        wr=1;
    //cout << "out: " << wl << " " << wr << endl;
    driverInterface->setAngularVelocity(wl,wr);
}

void Navigator::update() {
    if(distinationReached){
        return;
    }
    xt = new Pose();
    xt->update(40.156, 40.244, 0.000284356);
    xtGoal = &navigationPath->getPath()->at(navigationPointIndex);
    cout << "in: " << xt->getX() << " " << xt->getY() << " " << xt->getTheta() << endl;
    dx = xtGoal->getX() - xt->getX();
    dy = xtGoal->getY() - xt->getY();

    if(abs(dx) < 2 && abs(dy) < 2){
        if(navigationPointIndex>=navigationPath->getPath()->size()){
            distinationReached = true;
            driverInterface->stop();
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
    beta = -xt->getTheta() - alfa;
    v= K_ro * ro;
    w= K_alfa * alfa;
    wl = double(v-w*l)/r;
    wr = double(v+w*l)/r;
    cout << "out: " << wl << " " << wr << endl;
    if(wr > 10){
        cout << "faulty output" << endl;
        return;
    }
    //driverInterface->setAngularVelocity(wl,wr);
}

void Navigator::setNavigationPath(NavigationPath *navigationPath) {
    this->navigationPath = navigationPath;
    navigationPointIndex = 0 ;
    distinationReached =false;
}

bool Navigator::isDistinationReached() {
    return distinationReached;
}
