//
// Created by robot1 on 10/12/24.
//

#include "Lib1Class.h"
#include "Lib2Class.h"

#include <iostream>
#include <ostream>

Lib1Class::Lib1Class() {

}

void Lib1Class::print() {
    Lib2Class * a = new Lib2Class();
    a->print();
    std::cout << "TestPackageClass::print" << std::endl;
}
