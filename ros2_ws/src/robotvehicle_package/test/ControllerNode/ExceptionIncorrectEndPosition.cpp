//
// Created by robot1 on 12/16/24.
//

#include "ExceptionIncorrectEndPosition.h"

ExceptionIncorrectEndPosition:: ExceptionIncorrectEndPosition(double current, double expected, double tolerance,
        const std::string& fileName,
        const std::string& functionName, int codeLine):
BaseException("Position parameter was "+std::to_string(current)+
        " cm expected "+std::to_string(expected)+ " cm +-"+std::to_string(tolerance) ,
        __FILE__,fileName,functionName,codeLine){}

