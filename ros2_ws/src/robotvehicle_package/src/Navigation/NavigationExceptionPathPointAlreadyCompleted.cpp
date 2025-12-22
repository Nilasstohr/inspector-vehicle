//
// Created by robot1 on 12/19/25.
//

#include "NavigationExceptionPathPointAlreadyCompleted.h"

NavigationExceptionPathPointAlreadyCompleted::NavigationExceptionPathPointAlreadyCompleted()
    : std::runtime_error("Next point was executed but all path point already completed")
{}