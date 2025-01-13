//
// Created by robot1 on 1/9/25.
//

#include "ExceptionExceededMaxEntries.h"

#include "DirectoryHandling.h"

ExceptionExceededMaxEntries::ExceptionExceededMaxEntries(int max,const std::string &message,
                                                         const std::string& fileName,const std::string& functionName,const int codeLine):
BaseException("Exceeded max number of "+message+" which is " +std::to_string(max),__FILE__,fileName, functionName,codeLine) {
}
