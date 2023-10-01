//
// Created by robot1 on 10/1/23.
//

#include "HStack.h"

HStack::HStack(int capacity) : MatrixStackBase(capacity) {
    hStack = MatrixXd(this->capacity*2,3);
}

void HStack::add(Eigen::MatrixXd *H) {

}