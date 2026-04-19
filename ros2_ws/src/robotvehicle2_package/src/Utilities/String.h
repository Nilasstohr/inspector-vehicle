//
// Created by robotcentral on 2/24/25.
//

#ifndef STRING_H
#define STRING_H
#include <string>
#include <vector>
#include <sstream>


class String {
public:
    static void split(std::vector<std::string> &result, const std::string &str, char delimiter);
};



#endif //STRING_H
