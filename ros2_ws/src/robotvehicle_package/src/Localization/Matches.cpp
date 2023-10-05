//
// Created by robot1 on 10/5/23.
//

#include "Matches.h"

Matches::Matches(int capacity) {
    this->z    = new LineStack(capacity);
    this->zEst = new LineStack(capacity);
    this->hStack = new HStack(capacity);
}
