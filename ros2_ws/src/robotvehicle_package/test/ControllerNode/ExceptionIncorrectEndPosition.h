//
// Created by robot1 on 12/16/24.
//

#ifndef EXCEPTIONINCORRECTENDPOSITION_H
#define EXCEPTIONINCORRECTENDPOSITION_H

#include "Utilities/BaseException.h"

class ExceptionIncorrectEndPosition: public BaseException{
public:
    ExceptionIncorrectEndPosition(double current, double expected, double tolerance, const std::string &fileName,
                                  const std::string &functionName, int codeLine);
};



#endif //EXCEPTIONINCORRECTENDPOSITION_H
