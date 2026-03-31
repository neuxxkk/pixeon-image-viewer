#pragma once
#include <QMainWindow>
#include <memory> // std::unique_ptr
#include "../model/ImageModel.h"

class ImageViewWidget;
class QSlider;
class QLabel;

class MainWindow : public QMainWindow { // QObject -> QWidget -> QMainWindow
    Q_OBJECT // set(CMAKE_AUTOMOC ON) gera código back para esse sinal

public:
    explicit MainWindow(QWidget* parent = nullptr); // explicita que mainwindow é elemento raiz sem pai
    ~MainWindow() override = default; // Boa prática destrutor automático

private slots:
    // Slots são métodos chamados em resposta a signals
    // Convenção Qt: prefixo "on" + nome do sinal
    void onOpenFile();
    void onBrightnessChanged(int value);
    void onContrastChanged(int value);

private:
    void setupUi(); //Monta widgets
    void setupMenus(); // Monta menu   
    void updateControls(); //habilita caso imagem carregada, desabilita caso contrário

    // Ponteiros inteligentes para gerenciamento automático de memória
    // MainWindow é dono do modelo e da view, então usamos unique_ptr
    std::unique_ptr<ImageModel> m_model; // Gerencia a imagem e ajustes

    ImageViewWidget*    m_imageView{nullptr}; // Widget para exibir a imagem
    QSlider*            m_brightnessSlider{nullptr}; // Slider para ajuste de brilho
    QSlider*            m_contrastSlider{nullptr}; // Slider para ajuste de contraste
    QLabel*             m_brightnessLabel{nullptr}; // Label para mostrar valor do brilho
    QLabel*             m_contrastLabel{nullptr}; // Label para mostrar valor do contraste

};