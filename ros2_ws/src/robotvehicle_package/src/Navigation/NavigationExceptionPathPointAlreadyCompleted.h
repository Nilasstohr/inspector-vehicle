//
// Created by robot1 on 12/19/25.
//

#ifndef NAVIGATIONEXCEPTIONPATHPOINTALREADYCOMPLETED_H
#define NAVIGATIONEXCEPTIONPATHPOINTALREADYCOMPLETED_H
#include <stdexcept>

class NavigationExceptionPathPointAlreadyCompleted final : public std::runtime_error
{
public:
    explicit NavigationExceptionPathPointAlreadyCompleted();
};


#endif //NAVIGATIONEXCEPTIONPATHPOINTALREADYCOMPLETED_H
