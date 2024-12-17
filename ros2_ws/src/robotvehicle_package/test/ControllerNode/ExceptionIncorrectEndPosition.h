//
// Created by robot1 on 12/16/24.
//

#ifndef EXCEPTIONINCORRECTENDPOSITION_H
#define EXCEPTIONINCORRECTENDPOSITION_H
#include <stdexcept>


class ExceptionIncorrectEndPosition: public std::runtime_error{
public:
    ExceptionIncorrectEndPosition(double current, double expected, double tolerance);

    virtual char const* what() const throw();
private:
    double current;
    double expected;
    double tolerance;
};



#endif //EXCEPTIONINCORRECTENDPOSITION_H
