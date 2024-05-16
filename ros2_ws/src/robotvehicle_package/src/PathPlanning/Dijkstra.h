//
// Created by robot1 on 5/14/24.
//

#ifndef ROBOTVEHICLE_PACKAGE_DIJKSTRA_H
#define ROBOTVEHICLE_PACKAGE_DIJKSTRA_H

#include "../Navigation/NavigationPoint.h"
#include "../Localization/Pose.h"
#include "../Utilities/MatrixHelper.h"
#include "NodeQueue.h"

class Dijkstra: public MatrixHelper {
public:
    Dijkstra(Pose * currentPose,PathPoint endPoint, MatrixXd *gridMap);
private:
    NodeQueue nodeQueue;
};


#endif //ROBOTVEHICLE_PACKAGE_DIJKSTRA_H
