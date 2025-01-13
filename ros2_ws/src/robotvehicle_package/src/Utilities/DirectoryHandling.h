//
// Created by robot1 on 12/11/24.
//

#ifndef DIRECTORYHANDLING_H
#define DIRECTORYHANDLING_H
#include <string>
#include <filesystem>
#include <iostream>

class DirectoryHandling {
public:
    static std::string currentWorkingDirectory();
    static std::string getFileFileOfPath(const std::string& path);
};



#endif //DIRECTORYHANDLING_H
