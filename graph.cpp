#include "graph.h"

void Graph::addNode(const std::string &name, double x, double y) {
    nodes.emplace_back(name, x, y);
}

void Graph::addEdge(int from, int to, double distance, double cost) {
    adjList[from].emplace_back(to, distance, cost);
    adjList[to].emplace_back(from, distance, cost);
}
