//
// Created by robot1 on 1/3/25.
//

#include "Lines.h"

#include <iostream>

#define MAX_NUMBER_OF_LINES 50

Lines::Lines():lines(std::vector<Line>(MAX_NUMBER_OF_LINES)),count(0) {}

void Lines::addLine(Line *line) {
    if(count>=MAX_NUMBER_OF_LINES) {
        throw ExceptionExceededMaxEntries(MAX_NUMBER_OF_LINES,"lines",
            __FILE__,__func__,__LINE__);
    }
    lines.at(count).reset();
    lines.at(count) = *line;
    count++;
}

Line * Lines::getLine(int index) {
    return &lines.at(index);
}

void Lines::reset() {
    count = 0;
}

int Lines::size() const {
    return count;
}
