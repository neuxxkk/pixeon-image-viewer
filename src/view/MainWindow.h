#pragma once
#include <QMainWindow>
#include <memory> // std::unique_ptr
#include "../model/ImageModel.h"
#include "../model/ImageCollection.h"
#include <QListWidget>
#include <QSlider>
#include <QLabel>
#include <QGroupBox>
#include <QDockWidget>

class ImageViewWidget;

class MainWindow : public QMainWindow { // QObject -> QWidget -> QMainWindow
    Q_OBJECT // set(CMAKE_AUTOMOC ON) gera código back para esse sinal

public:
    explicit MainWindow(QWidget* parent = nullptr); // explicita que mainwindow é elemento raiz sem pai
    ~MainWindow() override = default; // Boa prática destrutor automático

private slots:
    // Slots são métodos chamados em resposta a signals
    // Convenção Qt: prefixo "on" + nome do sinal
    void onOpenFile();
    void onSaveFile(); // opcional, não implementado
    void onBrightnessChanged(int value);
    void onContrastChanged(int value);
    void onZoomChanged(int value);
    void onRotationChanged(int value);
    void onImageSelected(int row);

private:
    void setupUi(); //Monta widgets e conections
    void setupMenus(); // Monta menu   
    void reprocessImage(); // Reprocessa a imagem atual com os ajustes do Model
    void updateControls(); // Habilita caso imagem carregada, desabilita caso contrário  

    ImageCollection  m_collection;  // valor, não ponteiro — ela é simples o suficiente
    QListWidget*     m_imageList{nullptr};

    ImageViewWidget*    m_imageView{nullptr}; // Widget para exibir a imagem
    
    // Controles da Barra Lateral
    QDockWidget*        m_dock{nullptr};     // Dock de Ajustes
    QDockWidget*        m_listDock{nullptr}; // Dock de Miniaturas
    
    QGroupBox*          m_groupBC{nullptr}; // Brilho/Contraste
    QGroupBox*          m_groupZoom{nullptr};
    QGroupBox*          m_groupRotate{nullptr};

    QSlider*            m_brightnessSlider{nullptr};
    QSlider*            m_contrastSlider{nullptr};
    QSlider*            m_zoomSlider{nullptr};
    QSlider*            m_rotationSlider{nullptr};

    QLabel*             m_brightnessLabel{nullptr};
    QLabel*             m_contrastLabel{nullptr};
    QLabel*             m_zoomLabel{nullptr};
    QLabel*             m_rotationLabel{nullptr};

    double factorZoom{1.0};
};