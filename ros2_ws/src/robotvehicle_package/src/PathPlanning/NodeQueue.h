//
// Created by robot1 on 5/14/24.
//

#ifndef ROBOTVEHICLE_PACKAGE_NODEQUEUE_H
#define ROBOTVEHICLE_PACKAGE_NODEQUEUE_H


#include "Node.h"

class NodeQueue {
public:
    NodeQueue();
    void enQueue();
    bool isEmpty();
    Node * pop();
private:
    int currentNode;
    int lastNode;
    Node nodeQueue[10000];
};


#endif //ROBOTVEHICLE_PACKAGE_NODEQUEUE_H
