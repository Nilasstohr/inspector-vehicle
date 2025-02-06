//
// Created by robot1 on 1/3/25.
//

#ifndef LINES_H
#define LINES_H

#include <vector>
#include "Utilities/ExceptionExceededMaxEntries.h"
#include "Line.h"
#include "Configurations.h"

class Lines {
public:
    Lines();
    void addLine(Line *line);
    Line * getLine(int index);
    void reset();
    int size() const;
    std::string *toString();
    Lines * toGlobalReferenceFrame(const Pose *pose);

private:
    std::vector<Line> lines;
    std::string lineAsString;
    int count;
};



#endif //LINES_H
