//
// Created by robot1 on 1/9/25.
//

#ifndef BASEEXCEPTION_H
#define BASEEXCEPTION_H
#include <stdexcept>


class BaseException: public std::runtime_error{
public:
    BaseException(const std::string &message, const std::string &exceptionName, const std::string &fileName,
                  const std::string &functionName, int codeLine);
};


#endif //BASEEXCEPTION_H
