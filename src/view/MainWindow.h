#pragma once
#include <QMainWindow>

class MainWindow : public QMainWindow { // QObject -> QWidget -> QMainWindow
    Q_OBJECT // set(CMAKE_AUTOMOC ON) gera código back para esse sinal

public:
    explicit MainWindow(QWidget* parent = nullptr); // explicita que mainwindow é elemento raiz sem pai
    ~MainWindow() override = default; // Boa prática destrutor automático
};