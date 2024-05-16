//
// Created by robot1 on 5/14/24.
//

#include "PathPoint.h"

void PathPoint::set(int x, int y) {
    this->x =x;
    this->y =y;
}

int PathPoint::getY() {
    return y;
}

int PathPoint::getX() {
    return x;
}
