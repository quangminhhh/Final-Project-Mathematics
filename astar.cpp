#include "astar.h"

double heuristic(const Node &a, const Node &b) {
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

bool AStar(const Graph &graph, int start, int goal,
           std::vector<int> &path, double &totalDistance) {
    int n = graph.nodes.size();
    std::vector<double> g(n, std::numeric_limits<double>::infinity());
    std::vector<int> cameFrom(n, -1);

    std::priority_queue<PriorityNode> openSet;
    g[start] = 0.0;
    double h = heuristic(graph.nodes[start], graph.nodes[goal]);
    openSet.push({start, g[start] + h});

    while (!openSet.empty()) {
        PriorityNode current = openSet.top();
        openSet.pop();

        if (current.node == goal) {
            path.clear();
            int node = goal;
            while (node != -1) {
                path.push_back(node);
                node = cameFrom[node];
            }
            std::reverse(path.begin(), path.end());

            totalDistance = g[goal];
            return true;
        }

        for (const auto &edge : graph.adjList.at(current.node)) {
            int neighbor = edge.to;
            double tentative_g = g[current.node] + edge.distance;

            if (tentative_g < g[neighbor]) {
                cameFrom[neighbor] = current.node;
                g[neighbor] = tentative_g;
                openSet.push({neighbor, tentative_g + heuristic(graph.nodes[neighbor], graph.nodes[goal])});
            }
        }
    }

    return false;
}
