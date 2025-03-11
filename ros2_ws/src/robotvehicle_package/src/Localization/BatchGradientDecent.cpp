//
// Created by robotcentral on 2/26/25.
//

#include "BatchGradientDecent.h"

#include <iostream>


BatchGradientDecent::BatchGradientDecent(double sigma, double m, double b):
sigma(sigma),
gradientM(0),
gradientB(0),
m(m),
b(b),
xi(0),
yi(0),
N(0)
{}

void BatchGradientDecent::update(Line *line) {
    gradientM=0;
    gradientB=0;
    N = line->getPointCount()-1;
    int iterations = 1;
    for(int i=0;i<iterations;++i) {
        gradientB=0;
        gradientM=0;
        for(int j=0;j<N;j++) {
            xi = line->getPoint(j)->getX();
            yi = line->getPoint(j)->getY();
            gradientM+= xi*(yi-(m*xi + b));
            gradientB+= (yi-(m*xi + b));
        }
        m = m -sigma*1/N*gradientM;
        b = b -sigma*1/N*gradientB;
    }
}

void BatchGradientDecent::update(Line *line,int iterations) {
    double sumM=0;
    double sumB=0;
    N = line->getPointCount()-1;
    for(int i=0;i<iterations;++i) {
        sumB=0;
        sumM=0;
        for(int j=0;j<N;j++) {
            xi = line->getPoint(j)->getX();
            yi = line->getPoint(j)->getY();
            sumM+= xi*(yi-(m*xi + b));
            sumB+= (yi-(m*xi + b));
        }
        gradientM = -(2.0/N)*sumM;
        gradientB = -(2.0/N)*sumB;
        m -= sigma*gradientM;
        b -= sigma*gradientB;
        //std::cout << m << " " << b << std::endl;
    }
}


double BatchGradientDecent::getM() const {
    return m;
}

double BatchGradientDecent::getB() const {
    return b;
}


