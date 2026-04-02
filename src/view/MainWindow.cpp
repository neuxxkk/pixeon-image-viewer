#include "MainWindow.h"
#include "ImageViewWidget.h"
#include "../controller/ImageProcessor.h"
#include "../model/ImageCollection.h"
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QSlider>
#include <QLabel>
#include <QDockWidget>
#include <QVBoxLayout>
#include <QWidget>
#include <QInputDialog>


MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
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
    m_imageView = new ImageViewWidget(&m_collection, this); // Passa ponteiro
    setCentralWidget(m_imageView);

    // == Dock ESQUERDO (lista de imagens) ==
    auto* listDock      = new QDockWidget(tr("Imagens"), this);
    auto* listContainer = new QWidget(listDock);
    auto* listLayout    = new QVBoxLayout(listContainer);
    
    m_imageList = new QListWidget(listContainer);
    m_imageList->setMinimumWidth(160);
    listDock->setMaximumWidth(120);

    // === ESSENCIAL para mostrar miniaturas ===
    // Modo grade: ícone em cima, texto embaixo
    m_imageList->setViewMode(QListWidget::IconMode);
    m_imageList->setIconSize(QSize(64, 64));      // tamanho do ícone
    m_imageList->setGridSize(QSize(100, 80));       // aumenta altura pra caber 2 linhas de texto

    m_imageList->setResizeMode(QListWidget::Adjust); // reorganiza ao redimensionar
    m_imageList->setMovement(QListWidget::Static);   // itens não arrastáveis

    m_imageList->setWordWrap(true);          // já tens isso
    m_imageList->setSelectionMode(QAbstractItemView::SingleSelection);

    listLayout->addWidget(m_imageList);
    listContainer->setLayout(listLayout);
    listDock->setWidget(listContainer);

    // Encaixa à ESQUERDA
    addDockWidget(Qt::LeftDockWidgetArea, listDock);

    // Conecta seleção da lista ao slot
    connect(m_imageList, &QListWidget::currentRowChanged,
            this, &MainWindow::onImageSelected);

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
    
    dock->setMinimumWidth(120);
    

    container->setLayout(layout);
    dock->setWidget(container);
    addDockWidget(Qt::RightDockWidgetArea, dock); // Adiciona o dock à direita da janela

    // == CONECTIONS (signals -> slots) ==
    // QSlider::valueChanged(int) é o sinal emitido quando o valor do slider muda
    connect(m_brightnessSlider, &QSlider::valueChanged, this, &MainWindow::onBrightnessChanged);
    //       ^emissor            ^sinal                 ^receptor        ^slot

    connect(m_contrastSlider,   &QSlider::valueChanged, this, &MainWindow::onContrastChanged);

    connect(m_imageView, &ImageViewWidget::brightnessChangedByDrag,
            this, [this](int delta) { // lambda calcula novo brightness e chama slot via setValue()
                const int novoValor = m_brightnessSlider->value() + delta;
                m_brightnessSlider->setValue(novoValor);
            });

    connect(m_imageView, &ImageViewWidget::contrastChangedByDrag,
        this, [this](int delta) {
            const int novoValor = m_contrastSlider->value() + delta;
            m_contrastSlider->setValue(novoValor);
        });
}

void MainWindow::setupMenus() {
    auto* fileMenu = menuBar()->addMenu(tr("&Arquivo")); // & define atalho Alt+A

    QAction* openAction = fileMenu->addAction(tr("&Abrir..."), this, &MainWindow::onOpenFile); // adiciona ação ao menu e conecta ao slot
    openAction->setShortcut(QKeySequence::Open); // Atalho Ctrl+O
    openAction->setStatusTip(tr("Abrir uma imagem BMP, PNG ou JPEG"));

    fileMenu->addSeparator();

    auto* saveAction = fileMenu->addAction(tr("&Salvar Como..."), this, &MainWindow::onSaveFile);
    saveAction->setShortcut(QKeySequence::Save); // Atalho Ctrl+S
    saveAction->setStatusTip(tr("Salvar a imagem atual"));

    fileMenu->addSeparator();

    auto* exitAction = fileMenu->addAction(tr("Sair"), this, &QMainWindow::close);
    exitAction->setShortcut(QKeySequence::Quit); // Atalho Ctrl+Q
    exitAction->setStatusTip(tr("Fechar o aplicativo"));

    fileMenu->addSeparator();

    fileMenu->addAction(tr("Sobre"), this, []() {
        QMessageBox::information(
            nullptr,
            tr("Sobre o Pixeon Image Viewer"),
            tr("Pixeon Image Viewer v1.0\n"
               "Desenvolvido por Vitor Neuenschwander\n"
               "Usando Qt, C++17 e muito café!")
        );
    });


    auto* toolMenu = menuBar()->addMenu(tr("&Ferramentas"));

    toolMenu->addAction(tr("Rotacionar 90° Esquerda"), this, [this]() {
        auto* model = m_collection.current();
        if (model) {
            model->setRotation((model->rotation() - 90) % 360);
            reprocessImage();
        }
    })->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_L));

    toolMenu->addAction(tr("Rotacionar 90° Direita"), this, [this]() {
        auto* model = m_collection.current();
        if (model) {
            model->setRotation((model->rotation() + 90) % 360);
            reprocessImage();
        }
    })->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_D));

    toolMenu->addSeparator();

    toolMenu->addAction(tr("Inverter Cores (Negativo)"), this, [this]() {
        auto* model = m_collection.current();
        if (model) {
            model->setInverted(!model->isInverted());
            reprocessImage();
        }
    })->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_I));

    auto* measureAction = toolMenu->addAction(tr("Régua de Medição"), this, [this](bool checked) {
        m_imageView->setMeasurementMode(checked);
    });
    measureAction->setCheckable(true);
    measureAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_M));

    toolMenu->addSeparator();

    auto* resetViewAction = toolMenu->addAction(tr("Resetar Zoom/Pan"), m_imageView, &ImageViewWidget::resetView);
    resetViewAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_R)); // Atalho Ctrl+R
    resetViewAction->setStatusTip(tr("Reseta o zoom e pan para a posição inicial"));

    toolMenu->addSeparator();

    toolMenu->addAction(tr("Ajustar Sensibilidade do Zoom"), this, [this]() {
        bool ok;
        factorZoom = QInputDialog::getDouble(
            nullptr,
            tr("Ajustar Sensibilidade do Zoom"),
            tr("Fator de Sensibilidade (ex: 1.0 para normal, >1.0 para mais sensível, <1.0 para menos sensível):"),
            factorZoom,
            0.1, 10.0, 1, &ok
        );
        if (ok) {
            m_imageView->setZoomSensitivity(factorZoom);
            QMessageBox::information(nullptr, tr("Sensibilidade Ajustada"), tr("Nova sensibilidade de zoom: %1").arg(factorZoom));
        }
    });
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

    const auto index = m_collection.addImage(filePath);
    if (!index) {
        QMessageBox::warning(
            this,
            tr("Erro ao abrir"),
            tr("Não foi possível abrir o arquivo:\n%1").arg(filePath)
        );
        return;
    }

    // Gera miniatura 64x64 da imagem recém-carregada
    const QImage thumb = m_collection.current()->original()
                            .scaled(64, 64,
                                    Qt::KeepAspectRatio,
                                    Qt::SmoothTransformation);

    // Cria o item com ícone (miniatura) e texto (nome do arquivo)
    auto* item = new QListWidgetItem(
        QIcon(QPixmap::fromImage(thumb)),          // ícone = miniatura
        QFileInfo(filePath).fileName()             // texto = nome do arquivo
    );


    // Adiciona item no QListWidget
    m_imageList->addItem(item);
    m_imageList->setCurrentItem(item);

    //reseta sliders para 0
    m_brightnessSlider->blockSignals(true); // Evita chamar onBrightnessChanged
    m_contrastSlider->blockSignals(true); // Evita chamar onContrastChanged
    m_brightnessSlider->setValue(0);
    m_contrastSlider->setValue(0);
    m_brightnessSlider->blockSignals(false); // Reativa sinais
    m_contrastSlider->blockSignals(false); // Reativa sinais

    m_brightnessLabel->setText(tr("Brilho: 0"));
    m_contrastLabel->setText(tr("Contraste: 0"));

    m_imageView->resetView();

    updateControls(); // Habilita sliders

    m_imageView->update(); // Redesenha a imagem
    setWindowTitle(
        QString("Pixeon Image Viewer — %1")
            .arg(QFileInfo(filePath).fileName()) // mostra só o nome do arquivo
    );

}

void MainWindow::onSaveFile() {
    auto* model = m_collection.current();
    if (!model || !model->isLoaded()) {
        QMessageBox::information(this, tr("Nenhuma imagem"), tr("Abra uma imagem antes de salvar."));
        return;
    }

    const QString filePath = QFileDialog::getSaveFileName(
        this,
        tr("Salvar Imagem"),
        QDir::homePath(),
        tr("PNG (*.png);;JPEG (*.jpg *.jpeg);;BMP (*.bmp)")
    );

    if (filePath.isEmpty()) { return; }

    QString savePath = filePath;
    if (QFileInfo(savePath).suffix().isEmpty()) {
        savePath += ".png";
    }

    if (!m_collection.current()->processed().save(savePath)) {
        QMessageBox::warning(
            this,
            tr("Erro ao salvar"),
            tr("Não foi possível salvar o arquivo:\n%1").arg(savePath)
        );
    }
}

void MainWindow::onImageSelected(int row) {
    m_collection.selectImage(row);

    // Sincroniza os sliders com os valores da imagem selecionada
    auto* model = m_collection.current();
    if (!model) return;

    m_brightnessSlider->blockSignals(true);
    m_contrastSlider->blockSignals(true);
    m_brightnessSlider->setValue(model->brightness());
    m_contrastSlider->setValue(model->contrast());
    m_brightnessSlider->blockSignals(false);
    m_contrastSlider->blockSignals(false);

    // Sincroniza labels
    m_brightnessLabel->setText(tr("Brilho: %1").arg(model->brightness()));
    m_contrastLabel->setText(tr("Contraste: %1").arg(model->contrast()));

    m_imageView->update();
}


// Extrai a lógica de reprocessamento para evitar repetição
// nos slots de brightness e contrast
void MainWindow::reprocessImage() {
    auto* model = m_collection.current();
    if (!model || !model->isLoaded()) return;

    // Controller recebe dados do Model, processa, devolve resultado
    // O Model armazena — ninguém quebra a separação de responsabilidades
    QImage result = ImageProcessor::applyBrightnessContrast(
        model->original(),     // sempre parte do original, nunca do processed
        model->brightness(),   // parâmetro atual do model
        model->contrast()
    );

    if (model->rotation() != 0) {
        result = ImageProcessor::rotate(result, model->rotation());
    }

    if (model->isInverted()) {
        result = ImageProcessor::invert(result);
    }

    model->setProcessed(result);
    m_imageView->update(); // avisa o Qt para chamar paintEvent
}

// Slots simplificados — só atualizam o Model e delegam
void MainWindow::onBrightnessChanged(int value) {
    m_collection.current()->setBrightness(value);
    m_brightnessLabel->setText(tr("Brilho: %1").arg(value));
    reprocessImage();
}

void MainWindow::onContrastChanged(int value) {
    m_collection.current()->setContrast(value);
    m_contrastLabel->setText(tr("Contraste: %1").arg(value));
    reprocessImage();
}

void MainWindow::updateControls() {
    auto* model = m_collection.current();
    const bool hasImage = model && model->isLoaded();
    m_brightnessSlider->setEnabled(hasImage);
    m_contrastSlider->setEnabled(hasImage);
}