//
// Created by robot1 on 10/1/23.
//

#include "MatrixStackBase.h"

MatrixStackBase::MatrixStackBase(int capacity) : capacity(capacity) {}

int MatrixStackBase::size() {
    return stackNum;
}