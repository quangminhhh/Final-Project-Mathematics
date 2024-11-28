#include "astar.h"

// heuristic Euclidean
double heuristic(const Node &a, const Node &b) {
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

// thuat toan
bool AStar(const Graph &graph, int start, int goal, double alpha, double beta,
           std::vector<int> &path, double &totalDistance, double &totalCost) {
    int n = graph.nodes.size();
    std::vector<double> g(n, std::numeric_limits<double>::infinity());
    std::vector<double> c(n, std::numeric_limits<double>::infinity());
    std::vector<int> cameFrom(n, -1);

    std::priority_queue<PriorityNode> openSet;
    g[start] = 0.0;
    c[start] = 0.0;
    double h = heuristic(graph.nodes[start], graph.nodes[goal]);
    openSet.push({start, alpha * g[start] + beta * c[start] + h});

    while (!openSet.empty()) {
        PriorityNode current = openSet.top();
        openSet.pop();

        if (current.node == goal) {
            // Tái tạo đường đi
            path.clear();
            int node = goal;
            while (node != -1) {
                path.push_back(node);
                node = cameFrom[node];
            }
            std::reverse(path.begin(), path.end());

            totalDistance = g[goal];
            totalCost = c[goal];
            return true;
        }

        for (const auto &edge : graph.adjList.at(current.node)) {
            int neighbor = edge.to;
            double tentative_g = g[current.node] + edge.distance;
            double tentative_c = c[current.node] + edge.cost;

            double h = heuristic(graph.nodes[neighbor], graph.nodes[goal]);
            double tentative_f = alpha * tentative_g + beta * tentative_c + h;

            if (tentative_g < g[neighbor] || tentative_c < c[neighbor]) {
                cameFrom[neighbor] = current.node;
                g[neighbor] = tentative_g;
                c[neighbor] = tentative_c;
                openSet.push({neighbor, tentative_f});
            }
        }
    }

    return false; // mù đường
}
