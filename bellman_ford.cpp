#include "bellman_ford.h"
#include <limits>
#include <algorithm>

bool BellmanFord(const Graph &graph, int start, int goal,
                 std::vector<int> &path, double &totalDistance) {
    int n = graph.nodes.size();
    std::vector<double> dist(n, std::numeric_limits<double>::infinity());
    std::vector<int> cameFrom(n, -1);

    dist[start] = 0.0;

    for (int i = 0; i < n - 1; ++i) {
        for (const auto &[from, edges] : graph.adjList) {
            for (const auto &edge : edges) {
                int to = edge.to;
                double weight = edge.distance;
                if (dist[from] + weight < dist[to]) {
                    dist[to] = dist[from] + weight;
                    cameFrom[to] = from;
                }
            }
        }
    }
    for (const auto &[from, edges] : graph.adjList) {
        for (const auto &edge : edges) {
            int to = edge.to;
            double weight = edge.distance;
            if (dist[from] + weight < dist[to]) {
                return false;
            }
        }
    }

    if (dist[goal] == std::numeric_limits<double>::infinity()) {
        return false;
    }

    path.clear();
    for (int at = goal; at != -1; at = cameFrom[at]) {
        path.push_back(at);
    }
    std::reverse(path.begin(), path.end());

    totalDistance = dist[goal];
    return true;
}
