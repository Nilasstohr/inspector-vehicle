//
// Created by robot1 on 9/24/23.
//

#ifndef ROBOTVEHICLE_PACKAGE_POINTRECTFORM_H
#define ROBOTVEHICLE_PACKAGE_POINTRECTFORM_H


class PointRectForm {
public:
    PointRectForm();

    void set(double x, double y);
    double getX() const;
    double getY() const;
private:
    double x;
    double y;
};


#endif //ROBOTVEHICLE_PACKAGE_POINTRECTFORM_H
