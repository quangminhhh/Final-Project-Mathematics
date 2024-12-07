#include "mainwindow.h"
#include "astar.h"
#include "dijkstra.h"
#include "bellman_ford.h"

#include <QFileDialog>
#include <QFile>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMessageBox>

#include <QHBoxLayout>
#include <QFormLayout>
#include <QGraphicsTextItem>
#include <QLineEdit>
#include <chrono>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), scene(new QGraphicsScene(this)), view(new QGraphicsView(scene, this)) {

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // Giao diện tìm đường
    QLabel *startNodeLabel = new QLabel("Select Start Node:");
    QLabel *goalNodeLabel = new QLabel("Select Goal Node:");
    QLabel *algorithmLabel = new QLabel("Choose Algorithm:");

    startComboBox = new QComboBox();
    goalComboBox = new QComboBox();
    algorithmComboBox = new QComboBox();
    algorithmComboBox->addItem("A* Algorithm");
    algorithmComboBox->addItem("Dijkstra Algorithm");
    algorithmComboBox->addItem("Bellman-Ford Algorithm");

    findRouteButton = new QPushButton("Find Optimal Route");

    QFormLayout *findRouteForm = new QFormLayout();
    findRouteForm->addRow(startNodeLabel, startComboBox);
    findRouteForm->addRow(goalNodeLabel, goalComboBox);
    findRouteForm->addRow(algorithmLabel, algorithmComboBox);
    findRouteForm->addRow(findRouteButton);

    // Giao diện thêm node mới
    QLabel *nodeNameLabel = new QLabel("Node Name:");
    QLabel *nodeXLabel = new QLabel("X Coordinate:");
    QLabel *nodeYLabel = new QLabel("Y Coordinate:");

    nodeNameInput = new QLineEdit();
    nodeXInput = new QLineEdit();
    nodeYInput = new QLineEdit();
    addNodeButton = new QPushButton("Add New Node");

    QFormLayout *addNodeForm = new QFormLayout();
    addNodeForm->addRow(nodeNameLabel, nodeNameInput);
    addNodeForm->addRow(nodeXLabel, nodeXInput);
    addNodeForm->addRow(nodeYLabel, nodeYInput);
    addNodeForm->addRow(addNodeButton);

    // Giao diện thêm liên kết giữa hai node
    QLabel *firstNodeLabel = new QLabel("Select First Node:");
    QLabel *secondNodeLabel = new QLabel("Select Second Node:");
    QLabel *linkDistanceLabel = new QLabel("Distance:");

    firstNodeComboBox = new QComboBox();
    secondNodeComboBox = new QComboBox();
    linkDistanceInput = new QLineEdit();
    addLinkButton = new QPushButton("Add or Update Link");

    exportButton = new QPushButton("Export Map");
    importButton = new QPushButton("Import Map");

    mainLayout->addWidget(exportButton);  // Sửa lại mainLayout
    mainLayout->addWidget(importButton);  // Sửa lại mainLayout

    connect(exportButton, &QPushButton::clicked, this, &MainWindow::exportMap);
    connect(importButton, &QPushButton::clicked, this, &MainWindow::importMap);



    QFormLayout *addLinkForm = new QFormLayout();
    addLinkForm->addRow(firstNodeLabel, firstNodeComboBox);
    addLinkForm->addRow(secondNodeLabel, secondNodeComboBox);
    addLinkForm->addRow(linkDistanceLabel, linkDistanceInput);
    addLinkForm->addRow(addLinkButton);

    // Thêm các layout vào giao diện chính
    mainLayout->addWidget(view);
    mainLayout->addLayout(findRouteForm);
    mainLayout->addLayout(addNodeForm);
    mainLayout->addLayout(addLinkForm);

    resultLabel = new QLabel("Result: ");
    mainLayout->addWidget(resultLabel);

    setCentralWidget(centralWidget);

    setupGraph();
    visualizeGraph();

    // Kết nối tín hiệu
    connect(findRouteButton, &QPushButton::clicked, this, &MainWindow::findOptimalRoute);
    connect(addNodeButton, &QPushButton::clicked, this, &MainWindow::addNode);
    connect(addLinkButton, &QPushButton::clicked, this, &MainWindow::addLink);
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

    graph.addEdge(0, 1, 3.6);
    graph.addEdge(0, 2, 5.1);
    graph.addEdge(1, 3, 4.1);
    graph.addEdge(2, 3, 3.0);
    graph.addEdge(2, 4, 3.5);
    graph.addEdge(3, 5, 3.2);
    graph.addEdge(4, 6, 3.0);
    graph.addEdge(5, 7, 3.0);
    graph.addEdge(6, 8, 3.5);
    graph.addEdge(7, 9, 3.2);
    graph.addEdge(8, 9, 3.3);
    graph.addEdge(5, 6, 4.0);

    for (const auto &node : graph.nodes) {
        startComboBox->addItem(QString::fromStdString(node.name));
        goalComboBox->addItem(QString::fromStdString(node.name));
        firstNodeComboBox->addItem(QString::fromStdString(node.name));
        secondNodeComboBox->addItem(QString::fromStdString(node.name));
    }
}
void MainWindow::drawAxes() {
    const int gridSize = 20;  // Kích thước giữa các mốc
    const int sceneSize = 600; // Kích thước cảnh đồ họa

    QPen axisPen(Qt::gray, 1, Qt::DashLine);

    // Vẽ trục X
    scene->addLine(-sceneSize, 0, sceneSize, 0, axisPen);  // Trục X
    for (int i = -sceneSize; i <= sceneSize; i += gridSize) {
        if (i == 0) continue;  // Bỏ qua gốc tọa độ
        scene->addLine(i, -5, i, 5, axisPen);  // Các mốc trên trục X
        QGraphicsTextItem *label = scene->addText(QString::number(i / 40));
        label->setPos(i, 5);
    }

    // Vẽ trục Y
    scene->addLine(0, -sceneSize, 0, sceneSize, axisPen);  // Trục Y
    for (int i = -sceneSize; i <= sceneSize; i += gridSize) {
        if (i == 0) continue;  // Bỏ qua gốc tọa độ
        scene->addLine(-5, -i, 5, -i, axisPen);  // Các mốc trên trục Y (đảo ngược giá trị y)
        QGraphicsTextItem *label = scene->addText(QString::number(i / 40));
        label->setPos(5, -i);
    }
}

void MainWindow::visualizeGraph() {
    scene->clear();
    drawAxes(); // Vẽ trục tọa độ

    for (const auto &node : graph.nodes) {
        scene->addEllipse(node.x * 40, -node.y * 40, 20, 20); // Đảo ngược giá trị y
        QGraphicsTextItem *label = scene->addText(QString::fromStdString(node.name));
        label->setPos(node.x * 40 + 5, -node.y * 40 - 20); // Đảo ngược giá trị y
    }

    for (const auto &[from, edges] : graph.adjList) {
        for (const auto &edge : edges) {
            if (from < edge.to) {
                scene->addLine(
                    graph.nodes[from].x * 40 + 10, -graph.nodes[from].y * 40 + 10, // Đảo ngược giá trị y
                    graph.nodes[edge.to].x * 40 + 10, -graph.nodes[edge.to].y * 40 + 10, // Đảo ngược giá trị y
                    QPen(Qt::black)
                );

                double midX = ((graph.nodes[from].x + graph.nodes[edge.to].x) / 2.0) * 40.0;
                double midY = -((graph.nodes[from].y + graph.nodes[edge.to].y) / 2.0) * 40.0; // Đảo ngược giá trị y
                scene->addText(QString("D: %1").arg(edge.distance))->setPos(midX, midY);
            }
        }
    }
}


void MainWindow::resetVisualization() {
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

    QString result;

    // Lấy thuật toán được chọn
    QString selectedAlgorithm = algorithmComboBox->currentText();

    if (selectedAlgorithm == "A* Algorithm") {
        std::vector<int> pathAStar;
        double totalDistanceAStar = 0.0;

        auto startAStar = std::chrono::high_resolution_clock::now();
        bool foundAStar = AStar(graph, startIndex, goalIndex, pathAStar, totalDistanceAStar);
        auto endAStar = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> durationAStar = endAStar - startAStar;

        if (foundAStar) {
            result += "A* Algorithm:\nRoute: ";
            for (size_t i = 0; i < pathAStar.size(); ++i) {
                result += QString::fromStdString(graph.nodes[pathAStar[i]].name);
                if (i != pathAStar.size() - 1)
                    result += " -> ";
            }
            result += QString("\nDistance: %1, Time: %2 ms")
                      .arg(totalDistanceAStar).arg(durationAStar.count() * 1000);

            QPen pathPen(Qt::red, 3);
            for (size_t i = 0; i < pathAStar.size() - 1; ++i) {
                int from = pathAStar[i];
                int to = pathAStar[i + 1];
                scene->addLine(
                    graph.nodes[from].x * 40 + 10, -graph.nodes[from].y * 40 + 10, // Đảo ngược y
                    graph.nodes[to].x * 40 + 10, -graph.nodes[to].y * 40 + 10,     // Đảo ngược y
                    pathPen
                );
            }
        } else {
            result += "A* Algorithm: No path found.";
        }
    } else if (selectedAlgorithm == "Dijkstra Algorithm") {
        std::vector<int> pathDijkstra;
        double totalDistanceDijkstra = 0.0;

        auto startDijkstra = std::chrono::high_resolution_clock::now();
        bool foundDijkstra = Dijkstra(graph, startIndex, goalIndex, pathDijkstra, totalDistanceDijkstra);
        auto endDijkstra = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> durationDijkstra = endDijkstra - startDijkstra;

        if (foundDijkstra) {
            result += "Dijkstra Algorithm:\nRoute: ";
            for (size_t i = 0; i < pathDijkstra.size(); ++i) {
                result += QString::fromStdString(graph.nodes[pathDijkstra[i]].name);
                if (i != pathDijkstra.size() - 1)
                    result += " -> ";
            }
            result += QString("\nDistance: %1, Time: %2 ms")
                      .arg(totalDistanceDijkstra).arg(durationDijkstra.count() * 1000);

            QPen optimalPathPen(Qt::blue, 3);
            for (size_t i = 0; i < pathDijkstra.size() - 1; ++i) {
                int from = pathDijkstra[i];
                int to = pathDijkstra[i + 1];
                scene->addLine(
                    graph.nodes[from].x * 40 + 10, -graph.nodes[from].y * 40 + 10, // Đảo ngược y
                    graph.nodes[to].x * 40 + 10, -graph.nodes[to].y * 40 + 10,     // Đảo ngược y
                    optimalPathPen
                );
            }
        } else {
            result += "Dijkstra Algorithm: No path found.";
        }
    } else if (selectedAlgorithm == "Bellman-Ford Algorithm") {
        std::vector<int> pathBellmanFord;
        double totalDistanceBellmanFord = 0.0;

        auto startBellmanFord = std::chrono::high_resolution_clock::now();
        bool foundBellmanFord = BellmanFord(graph, startIndex, goalIndex, pathBellmanFord, totalDistanceBellmanFord);
        auto endBellmanFord = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> durationBellmanFord = endBellmanFord - startBellmanFord;

        if (foundBellmanFord) {
            result += "Bellman-Ford Algorithm:\nRoute: ";
            for (size_t i = 0; i < pathBellmanFord.size(); ++i) {
                result += QString::fromStdString(graph.nodes[pathBellmanFord[i]].name);
                if (i != pathBellmanFord.size() - 1)
                    result += " -> ";
            }
            result += QString("\nDistance: %1, Time: %2 ms")
                      .arg(totalDistanceBellmanFord).arg(durationBellmanFord.count() * 1000);

            QPen pathPen(Qt::green, 3);
            for (size_t i = 0; i < pathBellmanFord.size() - 1; ++i) {
                int from = pathBellmanFord[i];
                int to = pathBellmanFord[i + 1];
                scene->addLine(
                    graph.nodes[from].x * 40 + 10, -graph.nodes[from].y * 40 + 10, // Đảo ngược y
                    graph.nodes[to].x * 40 + 10, -graph.nodes[to].y * 40 + 10,     // Đảo ngược y
                    pathPen
                );
            }
        } else {
            result += "Bellman-Ford Algorithm: No path found.";
        }
    }

    resultLabel->setText(result);
}


void MainWindow::addNode() {
    QString nodeName = nodeNameInput->text();
    QString xText = nodeXInput->text();
    QString yText = nodeYInput->text();

    if (nodeName.isEmpty() || xText.isEmpty() || yText.isEmpty()) {
        resultLabel->setText("Please fill all fields to add a new node.");
        return;
    }

    // Kiểm tra tọa độ hợp lệ
    bool okX, okY;
    double x = xText.toDouble(&okX);
    double y = yText.toDouble(&okY);
    if (!okX || !okY) {
        resultLabel->setText("Invalid coordinates.");
        return;
    }

    // Kiểm tra trùng tên node
    for (const auto &node : graph.nodes) {
        if (node.name == nodeName.toStdString()) {
            resultLabel->setText("Node name already exists.");
            return;
        }
    }

    // Thêm node mới
    graph.addNode(nodeName.toStdString(), x, y);
    startComboBox->addItem(nodeName);
    goalComboBox->addItem(nodeName);
    firstNodeComboBox->addItem(nodeName);
    secondNodeComboBox->addItem(nodeName);

    // Cập nhật hiển thị
    resetVisualization();
    resultLabel->setText("Node added successfully.");
}
void MainWindow::addLink() {
    QString firstNode = firstNodeComboBox->currentText();
    QString secondNode = secondNodeComboBox->currentText();
    QString distanceText = linkDistanceInput->text();

    if (firstNode.isEmpty() || secondNode.isEmpty() || distanceText.isEmpty()) {
        resultLabel->setText("Please fill all fields to add a link.");
        return;
    }

    if (firstNode == secondNode) {
        resultLabel->setText("Cannot link a node to itself.");
        return;
    }

    bool ok;
    double distance = distanceText.toDouble(&ok);
    if (!ok || distance <= 0) {
        resultLabel->setText("Invalid distance value.");
        return;
    }

    // Tìm index của hai node
    int firstIndex = -1, secondIndex = -1;
    for (int i = 0; i < graph.nodes.size(); ++i) {
        if (graph.nodes[i].name == firstNode.toStdString()) {
            firstIndex = i;
        }
        if (graph.nodes[i].name == secondNode.toStdString()) {
            secondIndex = i;
        }
    }

    if (firstIndex == -1 || secondIndex == -1) {
        resultLabel->setText("Error: Node not found.");
        return;
    }

    // Thêm hoặc cập nhật liên kết giữa hai node
    graph.addEdge(firstIndex, secondIndex, distance);

    resultLabel->setText("Link added or updated successfully.");

    // Cập nhật hiển thị
    resetVisualization();
}

void MainWindow::exportMap() {
    QString jsonFilePath = QFileDialog::getSaveFileName(this, "Export Map Data", "", "JSON Files (*.json)");
    if (jsonFilePath.isEmpty()) {
        return;
    }

    // Lưu file JSON
    QJsonObject jsonObject;
    QJsonArray nodesArray;
    QJsonArray edgesArray;

    // Ghi danh sách node
    for (const auto &node : graph.nodes) {
        QJsonObject nodeObject;
        nodeObject["name"] = QString::fromStdString(node.name);
        nodeObject["x"] = node.x;
        nodeObject["y"] = node.y;
        nodesArray.append(nodeObject);
    }

    // Ghi danh sách liên kết
    for (const auto &[from, edges] : graph.adjList) {
        for (const auto &edge : edges) {
            QJsonObject edgeObject;
            edgeObject["from"] = QString::fromStdString(graph.nodes[from].name);
            edgeObject["to"] = QString::fromStdString(graph.nodes[edge.to].name);
            edgeObject["distance"] = edge.distance;
            edgesArray.append(edgeObject);
        }
    }

    jsonObject["nodes"] = nodesArray;
    jsonObject["edges"] = edgesArray;

    QJsonDocument jsonDoc(jsonObject);
    QFile jsonFile(jsonFilePath);
    if (jsonFile.open(QIODevice::WriteOnly)) {
        jsonFile.write(jsonDoc.toJson());
        jsonFile.close();
    }

    // Xuất file ảnh
    QString imageFilePath = jsonFilePath;
    imageFilePath.replace(".json", ".png"); // Đổi phần mở rộng thành .png
    exportImage(imageFilePath);

    QMessageBox::information(this, "Export Successful", "Map data and image have been exported successfully!");
}
void MainWindow::exportImage(const QString &filePath) {
    QImage image(scene->sceneRect().size().toSize(), QImage::Format_ARGB32);
    image.fill(Qt::white); // Nền trắng
    QPainter painter(&image);
    scene->render(&painter);
    image.save(filePath);
}
void MainWindow::importMap() {
    QString jsonFilePath = QFileDialog::getOpenFileName(this, "Import Map Data", "", "JSON Files (*.json)");
    if (jsonFilePath.isEmpty()) {
        return;
    }

    QFile jsonFile(jsonFilePath);
    if (!jsonFile.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "Import Failed", "Failed to open the file!");
        return;
    }

    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonFile.readAll());
    jsonFile.close();

    if (!jsonDoc.isObject()) {
        QMessageBox::warning(this, "Import Failed", "Invalid file format!");
        return;
    }

    QJsonObject jsonObject = jsonDoc.object();

    // Xóa bản đồ hiện tại
    graph.nodes.clear();
    graph.adjList.clear();
    startComboBox->clear();
    goalComboBox->clear();
    firstNodeComboBox->clear();
    secondNodeComboBox->clear();

    // Đọc danh sách node
    QJsonArray nodesArray = jsonObject["nodes"].toArray();
    for (const QJsonValue &nodeValue : nodesArray) {
        QJsonObject nodeObject = nodeValue.toObject();
        QString name = nodeObject["name"].toString();
        double x = nodeObject["x"].toDouble();
        double y = nodeObject["y"].toDouble();
        graph.addNode(name.toStdString(), x, y);
        startComboBox->addItem(name);
        goalComboBox->addItem(name);
        firstNodeComboBox->addItem(name);
        secondNodeComboBox->addItem(name);
    }

    // Đọc danh sách liên kết
    QJsonArray edgesArray = jsonObject["edges"].toArray();
    for (const QJsonValue &edgeValue : edgesArray) {
        QJsonObject edgeObject = edgeValue.toObject();
        QString from = edgeObject["from"].toString();
        QString to = edgeObject["to"].toString();
        double distance = edgeObject["distance"].toDouble();

        int fromIndex = -1, toIndex = -1;
        for (int i = 0; i < graph.nodes.size(); ++i) {
            if (graph.nodes[i].name == from.toStdString()) {
                fromIndex = i;
            }
            if (graph.nodes[i].name == to.toStdString()) {
                toIndex = i;
            }
        }

        if (fromIndex != -1 && toIndex != -1) {
            graph.addEdge(fromIndex, toIndex, distance);
        }
    }

    resetVisualization();
    QMessageBox::information(this, "Import Successful", "Map data has been imported successfully!");
}




