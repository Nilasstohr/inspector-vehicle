//
// Created by robot1 on 1/3/25.
//

#include "Lines.h"

Lines::Lines():lines(std::vector<Line>(50)),count(0) {}

void Lines::addLine(Line *line) {
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
