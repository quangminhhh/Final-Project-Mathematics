#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QComboBox>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include "graph.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QGraphicsScene *scene;
    QGraphicsView *view;

    QComboBox *startComboBox;
    QComboBox *goalComboBox;
    QComboBox *algorithmComboBox;

    QLineEdit *nodeNameInput;
    QLineEdit *nodeXInput;
    QLineEdit *nodeYInput;
    QPushButton *addNodeButton;

    QComboBox *firstNodeComboBox;
    QComboBox *secondNodeComboBox;
    QLineEdit *linkDistanceInput;
    QPushButton *addLinkButton;

    QPushButton *findRouteButton;
    QPushButton *resetButton;
    QPushButton *exportButton; // Nút Export
    QPushButton *importButton; // Nút Import

    QLabel *resultLabel;
    Graph graph;

    void setupGraph();
    void visualizeGraph();
    void resetVisualization();
    void drawAxes();

    private slots:
        void findOptimalRoute();
    void addNode();
    void addLink();
    void exportMap(); // Hàm Export
    void importMap(); // Hàm Import
    void exportImage(const QString &filePath); // Hàm xuất ảnh
};

#endif // MAINWINDOW_H
