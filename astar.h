#ifndef ASTAR_H
#define ASTAR_H

#include <vector>
#include <cmath>
#include <limits>
#include <queue>
#include "graph.h"

struct PriorityNode {
    int node;
    double f;

    bool operator<(const PriorityNode &other) const {
        return f > other.f;
    }
};

double heuristic(const Node &a, const Node &b);

bool AStar(const Graph &graph, int start, int goal,
           std::vector<int> &path, double &totalDistance);

#endif
