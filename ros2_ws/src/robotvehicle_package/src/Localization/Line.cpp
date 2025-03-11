//
// Created by robot1 on 9/23/23.
//

#include <valarray>
#include "Line.h"

#include <iostream>
#include <Utilities/Transformations.h>

Line::Line() {
    std::vector<PointRectForm> points(700);
    this->points = points;
    reset();
    isAlreadyGlobal = false;
    collinearMatched = false;
}

void Line::addRecPointFromPolar(double theta, double d) {
    addRecPoint(getXFromPolarForm(theta,d), getYFromPolarForm(theta,d));
}

void Line::addRecPoint(double x, double y) {
    points.at(pointsNum).set(x,y);
    pointsNum++;
}

void Line::reset() {
    pointsNum=0;
}

int Line::getPointCount() const {
    return pointsNum;
}

double Line::perpendicularDistance(PointPolarForm *point) {
    updateOriginLineNormal();
    double d;
    double xp = getXFromPolarForm(point->getAngle(),point->getDistance());
    double yp = getYFromPolarForm(point->getAngle(),point->getDistance());
    double theta_p = atan2(yp,xp);
    double p       = sqrt(pow(xp,2) + pow(yp,2));
    d = p*cos( theta_p - alfa ) - r;
    return abs(d);
}

void Line::updateOriginLineNormal() {
    //updateSlopeForm();
    updateSlopeFormLeastSquare();
    updateOriginalLineNormalFromSlopeForm();
}

void Line::updateOriginalLineNormalFromSlopeForm() {
    double fi = atan(m);
    if( fi>0 )
        alfa = fi-M_PI/2;
    else
        alfa = fi+M_PI/2;
    r    = b*sin(alfa);
    correctPolarCoordinates(alfa,r);
}

void Line::updateSlopeForm() {
    int lastPointIndex = pointsNum-1;
    double y2 = points.at(lastPointIndex).getY();
    double x2 = points.at(lastPointIndex).getX();
    m = (y2-points.begin()->getY())/
        (x2-points.begin()->getX()) ;
    b = y2 - m*x2;
}

double Line::getXFromPolarForm(double theta,double d){
    return d*cos(theta);
}

double Line::getYFromPolarForm(double theta,double d){
    return d*sin(theta);
}

double Line::getAlfa() const {
    return alfa;
}

void Line::setAlfa(double alfa) {
    Line::alfa = alfa;
}

double Line::getR() const {
    return r;
}

void Line::setR(double r) {
    Line::r = r;
}


void Line::limitAngle(double &angle) {
    if(angle < -M_PI)
        angle += 2*M_PI;
    else if(angle>M_PI)
        angle -= 2*M_PI;
}

void Line::correctPolarCoordinates(double &alfa, double &r) {
    if( r < 0){
        r=-r;
       alfa-=M_PI;
    }
    limitAngle(alfa);
}

double Line::getM() {
    return m;
}

double Line::getB() {
    return b;
}

PointRectForm *Line::getFirstPoint() {
    return &points.at(0);
}

PointRectForm *Line::getLastPoint() {
    return &points.at(pointsNum-1);
}

void Line::toGlobalReferenceFrame(const Pose *currentPose) {
    if(!isAlreadyGlobal) {
        double xR;
        double yR;
        double xG;
        double yG;
        for(int i=0;i<pointsNum;i++) {
            xR = points.at(i).getX();
            yR = points.at(i).getY();
            Transformations::recPointToGlobalReferenceFrame(xG,yG,xR,yR,currentPose);
            points.at(i).set(xG,yG);
        }
        isAlreadyGlobal = true;
    }
}

double Line::getPhi() {
    double x1;
    double y1;
    double x2;
    double y2;
    getLineEndPoints(x1,y1,x2,y2);
    return atan2(y2-y1,x2-x1);
}

void Line::updateSlopeFormLeastSquare() {
    double sqSumX = 0;
    double sumX =0;
    double sumY =0;
    double sumProductXY=0;
    double n = pointsNum;
    double x;
    double y;
    for(int i=0;i<n;i++) {
        x = points.at(i).getX();
        y = points.at(i).getY();
        sqSumX+=pow(x,2);
        sumX+=x;
        sumY+=y;
        sumProductXY+=x*y;
    }
    double sxx = sqSumX - pow(sumX,2)/n;
    double sxy = sumProductXY - sumX*sumY/n;
    m = sxy/sxx;
    b = sumY/n - m*sumX/n;
}

PointRectForm *Line::getPoint(const int i) {
    return &points.at(i);
}

void Line::setCollinearMatched() {
    collinearMatched=true;
}

bool Line::hasBeenCollinearMatched() {
    return collinearMatched;
}

void Line::setM(double m) {
    this->m=m;
}

void Line::setB(double b) {
    this->b=b;
}

void Line::addRecPoints(Line *line) {
    for(int i=0;i<line->getNumberOfPoint();i++) {
        addRecPoint(line->getPoint(i)->getX(),line->getPoint(i)->getY());
    }
}

void Line::printPoints() const {
    std::cout << m << ";" << b << " ";
    for(int i=0;i<pointsNum;i++) {
        std::cout << points.at(i).getX() << ";" << points.at(i).getY() << " ";
    }
    std::cout << std::endl;
}


void Line::getLineEndPoints(double &x1,double &y1,double &x2,double &y2) {
    const double x1t = getFirstPoint()->getX();
    const double y1t = getFirstPoint()->getY();
    const double x2t = getLastPoint()->getX();
    const double y2t = getLastPoint()->getY();
    if(x1t > x2t) {
        x1=x2t;
        y1=y2t;
        x2=x1t;
        y2=y1t;
    }else {
        x1=x1t;
        y1=y1t;
        x2=x2t;
        y2=y2t;
    }
}

void Line::getParallelTransEndPoints(double &x1p,double &y1p,double &x2p,double &y2p,const int d) {
    getLineEndPoints(x1,y1,x2,y2);
    l = sqrt( pow(x1-x2,2) + pow(y1-y2,2) );
    dx = d/l*(y1-y2);
    dy = d/l*(x2-x1);
    x1p = x1+dx;
    y1p = y1+dy;
    x2p = x2+dx;
    y2p = y2+dy;
}

int Line::getNumberOfPoint() const {
    return pointsNum;
}
