//
// Created by robot1 on 3/19/24.
//

#ifndef ROBOTVEHICLE_PACKAGE_POSE_H
#define ROBOTVEHICLE_PACKAGE_POSE_H


class Pose {
public:
    Pose();
    void update(double x,double y, double theta);

private:
    double x;
    double y;
    double theta;
public:
    double getX() const;

    double getY() const;

    double getTheta() const;
};


#endif //ROBOTVEHICLE_PACKAGE_POSE_H
