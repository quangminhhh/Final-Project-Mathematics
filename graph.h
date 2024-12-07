#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <string>
#include <unordered_map>
#include <tuple>

struct Node {
    std::string name;
    double x, y;

    Node(const std::string &n, double x_coord, double y_coord)
        : name(n), x(x_coord), y(y_coord) {}
};

struct Edge {
    int to;
    double distance;

    Edge(int t, double d) : to(t), distance(d) {}
};

class Graph {
public:
    std::vector<Node> nodes;
    std::unordered_map<int, std::vector<Edge>> adjList;

    void addNode(const std::string &name, double x, double y);
    void addEdge(int from, int to, double distance);
    bool isConnected(int from, int to);
};

#endif
