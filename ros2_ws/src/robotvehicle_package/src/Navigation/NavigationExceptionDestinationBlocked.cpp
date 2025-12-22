//
// Created by robot1 on 12/22/25.
//

#include "NavigationExceptionDestinationBlocked.h"

NavigationExceptionDestinationBlocked::NavigationExceptionDestinationBlocked(const double x, const double y)
    : std::runtime_error("Current pose is blocked at position x=" + std::to_string(x) + " y=" + std::to_string(y))
{}