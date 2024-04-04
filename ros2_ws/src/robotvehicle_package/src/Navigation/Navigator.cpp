//
// Created by robot1 on 3/19/24.
//

#include <iostream>
#include "Navigator.h"

#define GOAL_ACCEPTANCE_TRESHOLD_CM 10
#define MIN_W 1
#define MAX_W 4

Navigator::Navigator(DriverInterface *driverInterface):
        driverInterface(driverInterface){
    l=14.5;
    r=4.5;
    w=0;
    v=0;
    int G=2;
    K_ro = 0.35*G;
    K_alfa = 0.5*3;
    K_beta = -0.325;
    //K_beta = -0.325*G
}

void Navigator::update(KalmanLocalization * localization) {
    if(destinationReached){
        return;
    }
    xt = localization->getPose();
    xtGoal = &navigationPath->getPath()->at(navigationPointIndex);
    cout << "in: " << xt->getX() << " " << xt->getY() << " " << xt->getTheta() << endl;
    dx = xtGoal->getX() - xt->getX();
    dy = xtGoal->getY() - xt->getY();

    if(abs(dx) < GOAL_ACCEPTANCE_TRESHOLD_CM && abs(dy) < GOAL_ACCEPTANCE_TRESHOLD_CM){
        if(navigationPointIndex>=navigationPath->getPath()->size()-1){
            destinationReached = true;
            driverInterface->stop();
            return;
        }
        cout << navigationPointIndex << " out of " << navigationPath->getPath()->size()-1 << endl;
        navigationPointIndex++;
        return;
    }

    ro = sqrt(pow(dx,2)+pow(dy,2));
    alfa = -xt->getTheta() + atan2(dy,dx);


    beta = -xt->getTheta() - alfa;
    v= K_ro * ro;
    w= K_alfa * alfa; //+ beta*K_beta;
    wl = double(v-w*l)/r;
    wr = double(v+w*l)/r;

    if(wl<MIN_W)
        wl=MIN_W;
    else if(wl>MAX_W)
        wl=MAX_W;

    if(wr<MIN_W)
        wr=MIN_W;
    else if(wr>MAX_W)
        wr=MAX_W;

    //cout << "out: " << wl << " " << wr << endl;
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
        if(navigationPointIndex>=navigationPath->getPath()->size()){
            destinationReached = true;
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
