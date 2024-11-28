#include "dijkstra.h"
#include <queue>
#include <limits>
#include <algorithm>


struct DijkstraNode {
    int node;
    double distance;

    bool operator<(const DijkstraNode &other) const {
        return distance > other.distance;
    }
};

bool Dijkstra(const Graph &graph, int start, int goal,
              std::vector<int> &path, double &totalDistance, double &totalCost) {
    int n = graph.nodes.size();
    std::vector<double> dist(n, std::numeric_limits<double>::infinity());
    std::vector<double> cost(n, std::numeric_limits<double>::infinity());
    std::vector<int> cameFrom(n, -1);

    std::priority_queue<DijkstraNode> pq;
    dist[start] = 0.0;
    cost[start] = 0.0;
    pq.push({start, 0.0});

    while (!pq.empty()) {
        DijkstraNode current = pq.top();
        pq.pop();

        if (current.node == goal) {
            path.clear();
            int node = goal;
            while (node != -1) {
                path.push_back(node);
                node = cameFrom[node];
            }
            std::reverse(path.begin(), path.end());

            totalDistance = dist[goal];
            totalCost = cost[goal];
            return true;
        }

        for (const auto &edge : graph.adjList.at(current.node)) {
            int neighbor = edge.to;
            double tentativeDist = dist[current.node] + edge.distance;
            double tentativeCost = cost[current.node] + edge.cost;

            if (tentativeDist < dist[neighbor]) {
                dist[neighbor] = tentativeDist;
                cost[neighbor] = tentativeCost;
                cameFrom[neighbor] = current.node;
                pq.push({neighbor, dist[neighbor]});
            }
        }
    }

    return false; // mù đường
}
