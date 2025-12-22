//
// Created by robot1 on 12/22/25.
//

#ifndef NAVIGATIONEXCEPTIONCURRENTPOSEBLOCKED_H
#define NAVIGATIONEXCEPTIONCURRENTPOSEBLOCKED_H

#include <stdexcept>

class NavigationExceptionDestinationBlocked final : public std::runtime_error
{
public:
    explicit NavigationExceptionDestinationBlocked(double x,double y);
};


#endif //NAVIGATIONEXCEPTIONCURRENTPOSEBLOCKED_H
