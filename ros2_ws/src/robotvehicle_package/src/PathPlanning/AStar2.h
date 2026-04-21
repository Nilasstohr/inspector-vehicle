//
// Created by robotcentral on 4/17/26.
//

#ifndef ASTAR2_H
#define ASTAR2_H
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <utility>
#include "Utilities/MatrixHelper.h"

struct Node {
    int x,y;
    double g,h;
    [[nodiscard]] double f() const{return g+h;}
};

class AStar2 {
public:
    static std::vector<std::pair<int, int>> update(
        const MatrixXd & gridMap, const std::pair<int, int> & start, const std::pair<int, int> & goal);

    static std::vector<std::pair<int,int>> reconstructPath(
        Node destinationNode,std::unordered_map<int,int> & cameFrom,int cols, const std::pair<int,int> &start);
private:
    struct Compare {
        bool operator() (const Node & n1, const Node & n2) const {
            return n1.f() > n2.f(); // min-heap: lowest f() popped first
        }
    };
    static int key(int x, int y, int cols) { return x * cols + y; }
    static double heuristic(int x1, int y1, int x2, int y2);


};



#endif //ASTAR2_H
