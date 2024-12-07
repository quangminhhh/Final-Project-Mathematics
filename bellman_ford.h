#ifndef BELLMAN_FORD_H
#define BELLMAN_FORD_H

#include <vector>
#include "graph.h"

bool BellmanFord(const Graph &graph, int start, int goal,
                 std::vector<int> &path, double &totalDistance);

#endif // BELLMAN_FORD_H
