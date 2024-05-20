//
// Created by robot1 on 5/14/24.
//

#include <iostream>
#include "NodeQueue.h"

NodeQueue::NodeQueue() {
    reset();
}

void NodeQueue::reset() {
    currentNode=0;
    lastNode=0;
}

void NodeQueue::enQueue(PathPoint *point, PathPoint *path, int pathSize, const MatrixXd *graph, MatrixXd *visited) {
    //std::cout << "last node: " << lastNode << endl;
    nodeQueue[lastNode++].update(point,path,pathSize,graph,visited);
}

Node *NodeQueue::pop() {
    return &nodeQueue[currentNode++];
}

bool NodeQueue::isEmpty() {
    return currentNode>=lastNode;
}
