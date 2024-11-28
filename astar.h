#ifndef ASTAR_H
#define ASTAR_H

#include <vector>
#include <cmath>
#include <limits>
#include <queue>
#include "graph.h"

// cau truc a*
struct PriorityNode {
    int node;
    double f; // f = g + h

    bool operator<(const PriorityNode &other) const {
        return f > other.f;
    }
};

// heuristic (Euclidean distance)
double heuristic(const Node &a, const Node &b);

// cau truc thuat toan a*
bool AStar(const Graph &graph, int start, int goal, double alpha, double beta,
           std::vector<int> &path, double &totalDistance, double &totalCost);

#endif
