//
// Created by robot1 on 9/24/23.
//

#include "PointRectForm.h"

PointRectForm::PointRectForm():x(0),y(0) {}

void PointRectForm::set(double x, double y) {
    this->x = x;
    this->y = y;
}

double PointRectForm::getX() const {
    return x;
}

double PointRectForm::getY() const {
    return y;
}


