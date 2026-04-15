//
// Created by robotcentral on 4/17/26.
//

#include <algorithm>
#include <cmath>
#include "AStar2.h"


std::vector<std::pair<int, int>> AStar2::update(const MatrixXd & grid,
                                                const std::pair<int, int> &start, const std::pair<int, int> &goal) {

    const unsigned int rows = grid.rows();
    const unsigned int cols = grid.cols();

    // Reserve a sensible fraction — A* rarely visits the entire grid
    const size_t expected = std::min(rows * cols, (unsigned int)10000);

    std::vector<Node> container;
    container.reserve(expected);
    std::priority_queue open(Compare{}, std::move(container));

    std::unordered_map<int,double> gScore;
    std::unordered_map<int,int>    cameFrom;
    std::unordered_set<int>        closedSet;
    gScore.reserve(expected);
    cameFrom.reserve(expected);
    closedSet.reserve(expected);

    Node startNode{0, 0, 0.0f, heuristic(start.first, start.second, goal.first, goal.second)};
    open.push(startNode);
    gScore[key(start.first, start.second, cols)] = 0.0;

    // {row_offset, col_offset, move_cost}
    // straight = 1.0,  diagonal = √2 ≈ 1.414
    const struct { int dr, dc; double cost; } directions[] = {
        {-1,  0, 1.0},              // N
        { 1,  0, 1.0},              // S
        { 0, -1, 1.0},              // W
        { 0,  1, 1.0},              // E
        {-1, -1, M_SQRT2},          // NW
        {-1,  1, M_SQRT2},          // NE
        { 1, -1, M_SQRT2},          // SW
        { 1,  1, M_SQRT2},          // SE
    };

    while(!open.empty()){
        const Node parent = open.top();
        open.pop();

        int parentKey = key(parent.x, parent.y, cols);
        if (closedSet.count(parentKey) > 0)
            continue; // already settled

        closedSet.insert(parentKey);
        if(parent.x == goal.first && parent.y == goal.second){
            return reconstructPath(parent, cameFrom, cols, start);
        }

        for(const auto& dir : directions){
            const int nx = parent.x + dir.dr;
            const int ny = parent.y + dir.dc;

            if(nx >= 0 && ny >= 0 && nx < rows && ny < cols && grid.coeff(nx,ny) != 1){
                const double tentative_gScore = parent.g + dir.cost;
                int neighborKey = key(nx, ny, cols);

                if(!(gScore.count(neighborKey)>0) || tentative_gScore < gScore[neighborKey]){
                    gScore[neighborKey] = tentative_gScore;
                    cameFrom[neighborKey] = key(parent.x, parent.y, cols);

                    const double hScore = heuristic(nx, ny, goal.first, goal.second);
                    open.push(Node{nx, ny, tentative_gScore, hScore});
                }
            }
        }
    }

    return {}; // No path found
}

std::vector<std::pair<int,int>> AStar2::reconstructPath(Node destinationNode,std::unordered_map<int,int> & cameFrom, const int cols, const std::pair<int,int> &start) {
    std::vector<std::pair<int,int>> path;
    int currentKey = key(destinationNode.x, destinationNode.y, cols);
    while(cameFrom.count(currentKey)>0) {
        path.emplace_back(destinationNode.x, destinationNode.y);
        currentKey = cameFrom[currentKey];
        destinationNode.x = currentKey / cols;
        destinationNode.y = currentKey % cols;
    }
    path.emplace_back(start.first, start.second);
    std::reverse(path.begin(), path.end());
    //std::cout << "Path found: ";
    //for(const auto& p : path) {
    //    std::cout << "{" << p.first << ", " << p.second << "}, ";
   //}
    return path;
}


double AStar2::heuristic(const int x1, const int y1, const int x2, const int y2) {
    // Octile distance — admissible heuristic for 8-connected grids
    // diagonals cost √2, straights cost 1
    const double dx = std::abs(x1 - x2);
    const double dy = std::abs(y1 - y2);
    return (dx + dy) + (M_SQRT2 - 2.0) * std::min(dx, dy);
}
