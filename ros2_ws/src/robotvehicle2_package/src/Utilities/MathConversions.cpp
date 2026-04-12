//
// Created by robot1 on 9/27/23.
//

#include "MathConversions.h"
#include "math.h"

double MathConversions::deg2rad(double deg) {
    return (deg * (M_PI / 180));
}

double MathConversions::rad2deg(double rad) {
    return (rad * (180 / M_PI));
}