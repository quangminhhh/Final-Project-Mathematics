#include "graph.h"

void Graph::addNode(const std::string &name, double x, double y) {
    nodes.emplace_back(name, x, y);
}

void Graph::addEdge(int from, int to, double distance) {
    // Kiểm tra nếu liên kết đã tồn tại
    for (auto &edge : adjList[from]) {
        if (edge.to == to) {
            // Nếu liên kết tồn tại, cập nhật trọng số
            edge.distance = distance;
            for (auto &reverseEdge : adjList[to]) {
                if (reverseEdge.to == from) {
                    reverseEdge.distance = distance; // Cập nhật cả hướng ngược lại
                    return;
                }
            }
        }
    }

    // Nếu liên kết chưa tồn tại, thêm mới
    adjList[from].emplace_back(to, distance);
    adjList[to].emplace_back(from, distance);
}

bool Graph::isConnected(int from, int to) {
    if (adjList.find(from) == adjList.end()) {
        return false;
    }

    for (const auto &edge : adjList[from]) {
        if (edge.to == to) {
            return true; // Hai node đã được kết nối
        }
    }

    return false;
}
