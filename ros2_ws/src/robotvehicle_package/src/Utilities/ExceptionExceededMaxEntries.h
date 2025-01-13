//
// Created by robot1 on 1/9/25.
//

#ifndef EXCEPTIONEXCEEDEDMAXLINES_H
#define EXCEPTIONEXCEEDEDMAXLINES_H
#include "BaseException.h"


class ExceptionExceededMaxEntries: public BaseException {
public:
    ExceptionExceededMaxEntries(int max, const std::string &message, const std::string &fileName,
                              const std::string &functionName, int codeLine);
};

#endif //EXCEPTIONEXCEEDEDMAXLINES_H
