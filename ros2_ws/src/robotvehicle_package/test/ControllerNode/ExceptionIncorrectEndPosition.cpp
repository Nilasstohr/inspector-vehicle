//
// Created by robot1 on 12/16/24.
//

#include "ExceptionIncorrectEndPosition.h"

ExceptionIncorrectEndPosition::ExceptionIncorrectEndPosition(double current,double expected,double tolerance):
std::runtime_error("Position not accepted"),current(current),expected(expected),tolerance(tolerance){}

char const * ExceptionIncorrectEndPosition::what() const throw()
{
    return std::runtime_error::what();
}
