#include "MainWindow.h"
#include "ImageViewWidget.h"
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QSlider>
#include <QLabel>
#include <QDockWidget>
#include <QVBoxLayout>
#include <QWidget>


MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_model(std::make_unique<ImageModel>()) // Cria o modelo
{
    setupUi();
    setupMenus();
    updateControls(); // Sliders desabilitados até imagem ser carregada
    resize(1024, 768);
    setWindowTitle("Pixeon Image Viewer");
}

// == SETUPS voids ==

void MainWindow::setupUi() {
    // == Central Widget (a imagem) ==
    // setCentralWidget é o método do QMainWindow para definir o widget principal
    m_imageView = new ImageViewWidget(m_model.get(), this); // Passa ponteiro
    setCentralWidget(m_imageView);

    // == Dock Widget (controles) ==
    auto* dock       = new QDockWidget(tr("Ajustes"), this); // QDockWidget é um painel que pode ser acoplado ou flutuante
    auto* container  = new QWidget(dock); // Container para organizar os controles dentro do dock
    auto* layout     = new QVBoxLayout(container); // Layout vertical para empilhar os controles

    // -- Brilho --
    m_brightnessLabel = new QLabel(tr("Brilho: 0"), container);
    m_brightnessSlider = new QSlider(Qt::Horizontal, container);
    m_brightnessSlider->setRange(-100, 100); // Brilho de -100 a +100
    m_brightnessSlider->setValue(0); // Brilho inicial 0 
    m_brightnessSlider->setTickPosition(QSlider::TicksBelow);
    m_brightnessSlider->setTickInterval(25);

    // -- Contraste --
    m_contrastLabel = new QLabel(tr("Contraste: 0"), container);
    m_contrastSlider = new QSlider(Qt::Horizontal, container);
    m_contrastSlider->setRange(-100, 100); // Contraste de -100 a +100
    m_contrastSlider->setValue(0); // Contraste inicial 0
    m_contrastSlider->setTickPosition(QSlider::TicksBelow);
    m_contrastSlider->setTickInterval(25);

    // Adiciona widgets ao layout
    layout->addWidget(m_brightnessLabel);
    layout->addWidget(m_brightnessSlider);
    layout->addSpacing(12); // Espaço entre os controles
    layout->addWidget(m_contrastLabel);
    layout->addWidget(m_contrastSlider);
    layout->addStretch(); // Empurra os controles para o topo

    container->setLayout(layout);
    dock->setWidget(container);
    addDockWidget(Qt::RightDockWidgetArea, dock); // Adiciona o dock à direita da janela

    // == CONECTIONS (signals -> slots) ==
    // QSlider::valueChanged(int) é o sinal emitido quando o valor do slider muda
    connect(m_brightnessSlider, &QSlider::valueChanged, this, &MainWindow::onBrightnessChanged);
    //       ^emissor            ^sinal(tipo-safe)      ^receptor        ^slot

    connect(m_contrastSlider,   &QSlider::valueChanged, this, &MainWindow::onContrastChanged);
}

void MainWindow::setupMenus() {
    // Menu "Arquivo"
    auto* fileMenu = menuBar()->addMenu(tr("&Arquivo")); // & define atalho Alt+A

    // Ação "Abrir"
    QAction* openAction = fileMenu->addAction(tr("&Abrir..."), this, &MainWindow::onOpenFile); // Conecta ao slot onOpenFile
    openAction->setShortcut(QKeySequence::Open); // Atalho Ctrl+O
    openAction->setStatusTip(tr("Abrir uma imagem BMP, PNG ou JPEG"));

    /*// Lambda como slot: para ações simples, evita criar slot separado
    connect(openAction, &QAction::triggered,
            this, &MainWindow::onOpenFile);*/

    fileMenu->addSeparator(); // Linha separadora

    // Ação "Sair"
    auto* exitAction = fileMenu->addAction(tr("Sair"), this, &QMainWindow::close);
    exitAction->setShortcut(QKeySequence::Quit); // Atalho Ctrl+Q
    exitAction->setStatusTip(tr("Fechar o aplicativo"));

    connect(exitAction, &QAction::triggered,
        this, &QWidget::close);
}

// == SLOTS ==
void MainWindow::onOpenFile() {
    const QString filePath = QFileDialog::getOpenFileName(
        this, 
        tr("Abrir Imagem"), 
        QDir::homePath(),
        tr("Imagens (*.bmp *.png *.jpg *.jpeg);;"
           "BMP (*.bmp);;"
           "PNG (*.png);;"
           "JPEG (*.jpg *.jpeg)")
    );

    if (filePath.isEmpty()) { return; }

    if (!m_model->loadImage(filePath)) {
        QMessageBox::warning(
            this,
            tr("Erro ao abrir"),
            tr("Não foi possível abrir o arquivo:\n%1").arg(filePath)
        );        
        return;
    }

    //reseta sliders para 0
    m_brightnessSlider->blockSignals(true); // Evita chamar onBrightnessChanged
    m_contrastSlider->blockSignals(true); // Evita chamar onContrastChanged
    m_brightnessSlider->setValue(0);
    m_contrastSlider->setValue(0);
    m_brightnessSlider->blockSignals(false); // Reativa sinais
    m_contrastSlider->blockSignals(false); // Reativa sinais

    m_brightnessLabel->setText(tr("Brilho: 0"));
    m_contrastLabel->setText(tr("Contraste: 0"));

    updateControls(); // Habilita sliders

    m_imageView->update(); // Redesenha a imagem
    setWindowTitle(
        QString("Pixeon Image Viewer — %1")
            .arg(QFileInfo(filePath).fileName()) // mostra só o nome do arquivo
    );

}

void MainWindow::onBrightnessChanged(int value) {
    m_model->setBrightness(value); // Atualiza o modelo
    m_brightnessLabel->setText(tr("Brilho: %1").arg(value)); // Atualiza o texto do label
    m_imageView->update(); // Redesenha a imagem
}

void MainWindow::onContrastChanged(int value) {
    m_model->setContrast(value); // Atualiza o modelo
    m_contrastLabel->setText(tr("Contraste: %1").arg(value)); // Atualiza o texto do label
    m_imageView->update(); // Redesenha a imagem
}

void MainWindow::updateControls() {
    const bool hasImage = m_model->isLoaded();
    m_brightnessSlider->setEnabled(hasImage);
    m_contrastSlider->setEnabled(hasImage);
}