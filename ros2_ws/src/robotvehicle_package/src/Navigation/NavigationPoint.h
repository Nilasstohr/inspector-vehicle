//
// Created by robot1 on 2/18/24.
//

#ifndef ROBOTVEHICLE_PACKAGE_NAVIGATIONPOINT_H
#define ROBOTVEHICLE_PACKAGE_NAVIGATIONPOINT_H

class NavigationPoint {
public:
    NavigationPoint(double x,double y, double theta);
    double getX() const;
    double getY() const;
    double getTheta() const;
    double override(double x, double y);
private:
    double x;
    double y;
    double theta;

};


#endif //ROBOTVEHICLE_PACKAGE_NAVIGATIONPOINT_H
