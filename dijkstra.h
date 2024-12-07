#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include <vector>
#include "graph.h"

bool Dijkstra(const Graph &graph, int start, int goal,
              std::vector<int> &path, double &totalDistance);

#endif
