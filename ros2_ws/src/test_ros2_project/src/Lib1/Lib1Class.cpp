//
// Created by robot1 on 10/12/24.
//

#include "Lib1Class.h"

#include <iostream>
#include <ostream>

Lib1Class::Lib1Class() {

}

void Lib1Class::print() {
    auto * a = new Lib2Class();
    auto *b = new Lib1SubClass();
    a->print();
    b->print();
    std::cout << "TestPackageClass::print" << std::endl;
}
