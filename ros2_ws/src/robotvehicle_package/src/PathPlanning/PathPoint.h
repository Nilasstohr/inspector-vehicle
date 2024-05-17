//
// Created by robot1 on 5/14/24.
//

#ifndef ROBOTVEHICLE_PACKAGE_PATHPOINT_H
#define ROBOTVEHICLE_PACKAGE_PATHPOINT_H


class PathPoint {
public:
    void set(PathPoint* point);
    void set(int x, int y);

    int getX();
    int getY();
private:
    int x;
    int y;
};


#endif //ROBOTVEHICLE_PACKAGE_PATHPOINT_H
