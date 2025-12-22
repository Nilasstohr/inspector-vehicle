//
// Created by robot1 on 12/19/25.
//

#include "NavigationExceptionNoPathPoints.h"

NavigationExceptionNoPathPoints::NavigationExceptionNoPathPoints()
    : std::runtime_error("No path points in path list")
{}