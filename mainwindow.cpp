#include "mainwindow.h"
#include "astar.h"
#include "dijkstra.h"

#include <QHBoxLayout>
#include <QGraphicsTextItem>
#include <chrono>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), scene(new QGraphicsScene(this)), view(new QGraphicsView(scene, this)) {

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    startComboBox = new QComboBox();
    goalComboBox = new QComboBox();

    findRouteButton = new QPushButton("Find Route");

    resultLabel = new QLabel("Result: ");

    QHBoxLayout *controlsLayout = new QHBoxLayout();
    controlsLayout->addWidget(startComboBox);
    controlsLayout->addWidget(goalComboBox);
    controlsLayout->addWidget(findRouteButton);

    layout->addWidget(view);
    layout->addLayout(controlsLayout);
    layout->addWidget(resultLabel);

    setCentralWidget(centralWidget);

    setupGraph();
    visualizeGraph();

    connect(findRouteButton, &QPushButton::clicked, this, &MainWindow::findOptimalRoute);
}

MainWindow::~MainWindow() {}

void MainWindow::setupGraph() {
    graph.addNode("A", 0, 0);
    graph.addNode("B", 2, 3);
    graph.addNode("C", 5, 1);
    graph.addNode("D", 6, 4);
    graph.addNode("E", 8, 0);
    graph.addNode("F", 9, 3);
    graph.addNode("G", 11, 1);
    graph.addNode("H", 12, 4);
    graph.addNode("I", 14, 0);
    graph.addNode("J", 15, 3);

    graph.addEdge(0, 1, 3.6, 2.0);
    graph.addEdge(0, 2, 5.1, 3.0);
    graph.addEdge(1, 3, 4.1, 2.5);
    graph.addEdge(2, 3, 3.0, 2.2);
    graph.addEdge(2, 4, 3.5, 2.8);
    graph.addEdge(3, 5, 3.2, 2.4);
    graph.addEdge(4, 6, 3.0, 2.1);
    graph.addEdge(5, 7, 3.0, 2.3);
    graph.addEdge(6, 8, 3.5, 2.7);
    graph.addEdge(7, 9, 3.2, 2.5);
    graph.addEdge(8, 9, 3.3, 2.6);
    graph.addEdge(5, 6, 4.0, 3.0);

    for (const auto &node : graph.nodes) {
        startComboBox->addItem(QString::fromStdString(node.name));
        goalComboBox->addItem(QString::fromStdString(node.name));
    }
}

void MainWindow::visualizeGraph() {
    for (const auto &node : graph.nodes) {
        scene->addEllipse(node.x * 40, node.y * 40, 20, 20);
        QGraphicsTextItem *label = scene->addText(QString::fromStdString(node.name));
        label->setPos(node.x * 40 + 5, node.y * 40 - 20);
    }

    for (const auto &[from, edges] : graph.adjList) {
        for (const auto &edge : edges) {
            if (from < edge.to) {
                scene->addLine(
                    graph.nodes[from].x * 40 + 10, graph.nodes[from].y * 40 + 10,
                    graph.nodes[edge.to].x * 40 + 10, graph.nodes[edge.to].y * 40 + 10
                );

                double midX = ((graph.nodes[from].x + graph.nodes[edge.to].x) / 2.0) * 40.0;
                double midY = ((graph.nodes[from].y + graph.nodes[edge.to].y) / 2.0) * 40.0;
                scene->addText(QString("D: %1, C: %2")
                               .arg(edge.distance).arg(edge.cost))->setPos(midX, midY);
            }
        }
    }
}

void MainWindow::resetVisualization() {
    scene->clear();
    visualizeGraph();
    resultLabel->setText("Result: ");
}

void MainWindow::findOptimalRoute() {
    resetVisualization();
    int startIndex = startComboBox->currentIndex();
    int goalIndex = goalComboBox->currentIndex();

    if (startIndex == goalIndex) {
        resultLabel->setText("Start and goal nodes cannot be the same.");
        return;
    }

    // Các biến lưu kết quả
    std::vector<int> pathAStar, pathDijkstra;
    double totalDistanceAStar = 0.0, totalCostAStar = 0.0;
    double totalDistanceDijkstra = 0.0, totalCostDijkstra = 0.0;

    // Alpha và Beta (trọng số cho thuật toán A*)
    double alpha = 1.0, beta = 1.0;

    // **1. Chạy thuật toán A***
    auto startAStar = std::chrono::high_resolution_clock::now();
    bool foundAStar = AStar(graph, startIndex, goalIndex, alpha, beta,
                            pathAStar, totalDistanceAStar, totalCostAStar);
    auto endAStar = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> durationAStar = endAStar - startAStar;

    // **2. Chạy thuật toán Dijkstra**
    auto startDijkstra = std::chrono::high_resolution_clock::now();
    bool foundDijkstra = Dijkstra(graph, startIndex, goalIndex,
                                  pathDijkstra, totalDistanceDijkstra, totalCostDijkstra);
    auto endDijkstra = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> durationDijkstra = endDijkstra - startDijkstra;

    // **3. Hiển thị kết quả**
    QString result;

    // Kết quả từ thuật toán A*
    if (foundAStar) {
        result += "A* Algorithm:\nRoute: ";
        for (size_t i = 0; i < pathAStar.size(); ++i) {
            result += QString::fromStdString(graph.nodes[pathAStar[i]].name);
            if (i != pathAStar.size() - 1)
                result += " -> ";
        }
        result += QString("\nDistance: %1, Cost: %2, Time: %3 ms\n\n")
                  .arg(totalDistanceAStar).arg(totalCostAStar).arg(durationAStar.count() * 1000);
    } else {
        result += "A* Algorithm: No path found.\n\n";
    }

    // Kết quả từ thuật toán Dijkstra
    if (foundDijkstra) {
        result += "Dijkstra Algorithm:\nRoute: ";
        for (size_t i = 0; i < pathDijkstra.size(); ++i) {
            result += QString::fromStdString(graph.nodes[pathDijkstra[i]].name);
            if (i != pathDijkstra.size() - 1)
                result += " -> ";
        }
        result += QString("\nDistance: %1, Cost: %2, Time: %3 ms")
                  .arg(totalDistanceDijkstra).arg(totalCostDijkstra).arg(durationDijkstra.count() * 1000);

        // **4. Tô màu đường đi của thuật toán Dijkstra**
        QPen optimalPathPen(Qt::blue, 3); // Đường màu xanh, độ dày 3
        for (size_t i = 0; i < pathDijkstra.size() - 1; ++i) {
            int from = pathDijkstra[i];
            int to = pathDijkstra[i + 1];
            scene->addLine(
                graph.nodes[from].x * 40 + 10, graph.nodes[from].y * 40 + 10,
                graph.nodes[to].x * 40 + 10, graph.nodes[to].y * 40 + 10,
                optimalPathPen
            );
        }
    } else {
        result += "Dijkstra Algorithm: No path found.";
    }

    // Cập nhật kết quả lên giao diện
    resultLabel->setText(result);
}
