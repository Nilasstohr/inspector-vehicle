//
// Created by robotcentral on 2/26/25.
//

#ifndef BATCHGRADIENTDECENT_H
#define BATCHGRADIENTDECENT_H

#include "Line.h"

class BatchGradientDecent {
public:
    explicit BatchGradientDecent(double sigma,double m,double b);
    void update(Line *line);

    void update(Line *line, int iterations);

    double getM() const;
    double getB() const;
private:
    double sigma;
    double gradientM;
    double gradientB;
    double m;
    double b;
    double xi;
    double yi;
    int N;
};



#endif //BATCHGRADIENTDECENT_H
