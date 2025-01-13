//
// Created by robot1 on 1/3/25.
//

#ifndef LINES_H
#define LINES_H

#include <vector>
#include "Utilities/ExceptionExceededMaxEntries.h"
#include "Line.h"

class Lines {
public:
    Lines();
    void addLine(Line *line);
    Line * getLine(int index);
    void reset();
    int size() const;
private:
    std::vector<Line> lines;
    int count;
};



#endif //LINES_H
