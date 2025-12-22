//
// Created by robot1 on 12/19/25.
//

#ifndef NAVIGATIONEXCEPTIONNOPATHPOINTS_H
#define NAVIGATIONEXCEPTIONNOPATHPOINTS_H
#include <stdexcept>

class NavigationExceptionNoPathPoints final : public std::runtime_error
{
public:
    explicit NavigationExceptionNoPathPoints();
};

#endif //NAVIGATIONEXCEPTIONNOPATHPOINTS_H
