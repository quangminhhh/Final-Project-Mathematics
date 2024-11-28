#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include "graph.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QGraphicsScene *scene;
    QGraphicsView *view;

    QComboBox *startComboBox;
    QComboBox *goalComboBox;

    QPushButton *findRouteButton;
    QPushButton *resetButton;

    QLabel *resultLabel;

    Graph graph;

    void setupGraph();
    void visualizeGraph();
    void resetVisualization();
    void findOptimalRoute();
};

#endif // MAINWINDOW_H
