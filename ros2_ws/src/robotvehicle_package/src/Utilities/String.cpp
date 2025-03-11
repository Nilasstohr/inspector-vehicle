//
// Created by robotcentral on 2/24/25.
//

#include "String.h"

void String::split(std::vector<std::string> &result, const std::string &str, char delimiter) {
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        result.push_back(token);
    }
}
