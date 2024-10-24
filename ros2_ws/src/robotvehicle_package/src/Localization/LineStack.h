//
// Created by robot1 on 10/1/23.
//

#ifndef ROBOTVEHICLE_PACKAGE_LINESTACK_H
#define ROBOTVEHICLE_PACKAGE_LINESTACK_H


#include "Line.h"
#include "Utilities/MatrixStackBase.h"

class LineStack: public MatrixStackBase {
public:
    LineStack(int capacity);
    void   add(Line * line);
    void   add(double alfa,double r);
    void   add(const MatrixXd* line);
    double getAlfaByIndex(int j) const;
    double getRByIndex(int j) const;
    void setByIndex(int j,double alfa,double r);
    void getByIndex(int j,double &alfa,double &r);
    const MatrixXd  *getLineByIndex(int j);
    const MatrixXd getLines();
    void printLines(char * name);
    MatrixXd unstackLines();

    const MatrixXd getLinesUnstacked();

private:
    MatrixXd line;
    void setROffSetByIndex(int j,double ROffSet);
};


#endif //ROBOTVEHICLE_PACKAGE_LINESTACK_H
