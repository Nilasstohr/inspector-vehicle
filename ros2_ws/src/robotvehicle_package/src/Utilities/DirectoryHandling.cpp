//
// Created by robot1 on 12/11/24.
//

#include "DirectoryHandling.h"

std::string DirectoryHandling::currentWorkingDirectory() {
    //cout << "Current path is " << filesystem::current_path()<< endl;
    return std::filesystem::current_path();
}
