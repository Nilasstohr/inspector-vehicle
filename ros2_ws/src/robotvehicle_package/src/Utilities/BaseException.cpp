//
// Created by robot1 on 1/9/25.
//

#include "BaseException.h"
#include "DirectoryHandling.h"

BaseException::BaseException(const std::string &message, const std::string &exceptionName, const std::string &fileName,
                  const std::string &functionName, int codeLine):
runtime_error("\n\n######################### Exception #########################\n" +
    DirectoryHandling::getFileFileOfPath(exceptionName)+": "+message
    + "\nLocation: In Class '"+DirectoryHandling::getFileFileOfPath(fileName)+"' in function '"+ functionName
    + "' line " + std::to_string(codeLine)+"\n#############################################################\n\n") {
}
