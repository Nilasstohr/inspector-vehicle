//
// Created by robot1 on 10/1/23.
//

#ifndef ROBOTVEHICLE_PACKAGE_LINESTACK_H
#define ROBOTVEHICLE_PACKAGE_LINESTACK_H


#include "Line.h"
#include "../../Utilities/MatrixStackBase.h"

class LineStack: private MatrixStackBase {
public:
    LineStack(int capacity);
    void   add(Line * line);
    void   add(double alfa,double r);
    double getAlfaByIndex(int j) const;
    double getRByIndex(int j) const;
private:
    MatrixXd lineStack;
};


#endif //ROBOTVEHICLE_PACKAGE_LINESTACK_H
