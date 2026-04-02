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
#include <QGroupBox>
#include <QAction>
#include <QKeySequence>
#include <QFileInfo>

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
    m_imageView = new ImageViewWidget(&m_collection, this);
    setCentralWidget(m_imageView);

    // == Dock ESQUERDO (lista de imagens) ==
    m_listDock      = new QDockWidget(tr("Imagens"), this);
    m_listDock->setObjectName("ListDock");
    auto* listContainer = new QWidget(m_listDock);
    auto* listLayout    = new QVBoxLayout(listContainer);
    
    m_imageList = new QListWidget(listContainer);
    m_imageList->setMinimumWidth(160);
    m_listDock->setMaximumWidth(120);

    m_imageList->setViewMode(QListWidget::IconMode);
    m_imageList->setIconSize(QSize(64, 64));
    m_imageList->setGridSize(QSize(100, 80));
    m_imageList->setResizeMode(QListWidget::Adjust);
    m_imageList->setMovement(QListWidget::Static);
    m_imageList->setWordWrap(true);
    m_imageList->setSelectionMode(QAbstractItemView::SingleSelection);

    listLayout->addWidget(m_imageList);
    listContainer->setLayout(listLayout);
    m_listDock->setWidget(listContainer);
    addDockWidget(Qt::LeftDockWidgetArea, m_listDock);

    connect(m_imageList, &QListWidget::currentRowChanged,
            this, &MainWindow::onImageSelected);

    // == Dock Widget (controles) ==
    m_dock = new QDockWidget(tr("Ajustes"), this);
    m_dock->setObjectName("MenuUI"); // Para persistência se necessário
    auto* container  = new QWidget(m_dock);
    auto* mainLayout = new QVBoxLayout(container);

    // -- Grupo: Brilho e Contraste --
    m_groupBC = new QGroupBox(tr("Brilho & Contraste"), container);
    auto* bcLayout = new QVBoxLayout(m_groupBC);

    m_brightnessLabel = new QLabel(tr("Brilho: 0"), m_groupBC);
    m_brightnessSlider = new QSlider(Qt::Horizontal, m_groupBC);
    m_brightnessSlider->setRange(-100, 100);
    m_brightnessSlider->setValue(0);
    
    m_contrastLabel = new QLabel(tr("Contraste: 0"), m_groupBC);
    m_contrastSlider = new QSlider(Qt::Horizontal, m_groupBC);
    m_contrastSlider->setRange(-100, 100);
    m_contrastSlider->setValue(0);

    bcLayout->addWidget(m_brightnessLabel);
    bcLayout->addWidget(m_brightnessSlider);
    bcLayout->addWidget(m_contrastLabel);
    bcLayout->addWidget(m_contrastSlider);
    mainLayout->addWidget(m_groupBC);

    // -- Grupo: Zoom --
    m_groupZoom = new QGroupBox(tr("Zoom"), container);
    auto* zoomLayout = new QVBoxLayout(m_groupZoom);
    
    m_zoomLabel = new QLabel(tr("Zoom: 100%"), m_groupZoom);
    m_zoomSlider = new QSlider(Qt::Horizontal, m_groupZoom);
    m_zoomSlider->setRange(5, 2000); // 5% a 2000%
    m_zoomSlider->setValue(100);

    zoomLayout->addWidget(m_zoomLabel);
    zoomLayout->addWidget(m_zoomSlider);
    mainLayout->addWidget(m_groupZoom);

    // -- Grupo: Rotação --
    m_groupRotate = new QGroupBox(tr("Rotação"), container);
    auto* rotateLayout = new QVBoxLayout(m_groupRotate);

    m_rotationLabel = new QLabel(tr("Rotação: 0°"), m_groupRotate);
    m_rotationSlider = new QSlider(Qt::Horizontal, m_groupRotate);
    m_rotationSlider->setRange(0, 360);
    m_rotationSlider->setValue(0);

    rotateLayout->addWidget(m_rotationLabel);
    rotateLayout->addWidget(m_rotationSlider);
    mainLayout->addWidget(m_groupRotate);

    mainLayout->addStretch();
    
    m_dock->setMinimumWidth(180);
    container->setLayout(mainLayout);
    m_dock->setWidget(container);
    addDockWidget(Qt::RightDockWidgetArea, m_dock);

    // == CONECTIONS ==
    connect(m_brightnessSlider, &QSlider::valueChanged, this, &MainWindow::onBrightnessChanged);
    connect(m_contrastSlider,   &QSlider::valueChanged, this, &MainWindow::onContrastChanged);
    connect(m_zoomSlider,       &QSlider::valueChanged, this, &MainWindow::onZoomChanged);
    connect(m_rotationSlider,   &QSlider::valueChanged, this, &MainWindow::onRotationChanged);

    connect(m_imageView, &ImageViewWidget::brightnessChangedByDrag, this, [this](int delta) {
        m_brightnessSlider->setValue(m_brightnessSlider->value() + delta);
    });

    connect(m_imageView, &ImageViewWidget::contrastChangedByDrag, this, [this](int delta) {
        m_contrastSlider->setValue(m_contrastSlider->value() - delta); // Invertido para naturalidade
    });

    connect(m_imageView, &ImageViewWidget::zoomChanged, this, [this](double factor) {
        m_zoomSlider->blockSignals(true);
        m_zoomSlider->setValue(static_cast<int>(factor * 100));
        m_zoomLabel->setText(tr("Zoom: %1%").arg(m_zoomSlider->value()));
        m_zoomSlider->blockSignals(false);
    });
}

void MainWindow::setupMenus() {
    // Menu Arquivo
    auto* fileMenu = menuBar()->addMenu(tr("&Arquivo"));
    
    auto* openAct = fileMenu->addAction(tr("&Abrir..."), this, &MainWindow::onOpenFile);
    openAct->setShortcut(QKeySequence::Open);

    auto* saveAct = fileMenu->addAction(tr("&Salvar Como..."), this, &MainWindow::onSaveFile);
    saveAct->setShortcut(QKeySequence::Save);

    fileMenu->addSeparator();
    
    auto* exitAct = fileMenu->addAction(tr("Sair"), this, &QMainWindow::close);
    exitAct->setShortcut(QKeySequence::Quit);

    // Menu Ferramentas
    auto* toolMenu = menuBar()->addMenu(tr("&Ferramentas"));
    
    auto* actBC = toolMenu->addAction(tr("Mostrar Brilho/Contraste"));
    actBC->setCheckable(true);
    actBC->setChecked(true);
    connect(actBC, &QAction::toggled, m_groupBC, &QGroupBox::setVisible);

    auto* actZoom = toolMenu->addAction(tr("Mostrar Zoom"));
    actZoom->setCheckable(true);
    actZoom->setChecked(true);
    connect(actZoom, &QAction::toggled, m_groupZoom, &QGroupBox::setVisible);

    auto* actRotate = toolMenu->addAction(tr("Mostrar Rotação"));
    actRotate->setCheckable(true);
    actRotate->setChecked(true);
    connect(actRotate, &QAction::toggled, m_groupRotate, &QGroupBox::setVisible);

    toolMenu->addSeparator();

    auto* invertAct = toolMenu->addAction(tr("Inverter Cores"), this, [this]() {
        auto* model = m_collection.current();
        if (model) {
            model->setInverted(!model->isInverted());
            reprocessImage();
        }
    });
    invertAct->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_I));

    auto* measureAction = toolMenu->addAction(tr("Régua de Medição"), this, [this](bool checked) {
        m_imageView->setMeasurementMode(checked);
    });
    measureAction->setCheckable(true);
    measureAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_M));

    toolMenu->addSeparator();
    
    auto* resetAct = toolMenu->addAction(tr("Resetar Visualização"), m_imageView, &ImageViewWidget::resetView);
    resetAct->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_R));

    // Menu Exibir (Para reabrir os painéis)
    auto* viewMenu = menuBar()->addMenu(tr("&Exibir"));
    
    QAction* showListAction = m_listDock->toggleViewAction();
    showListAction->setText(tr("Lista de Imagens"));
    viewMenu->addAction(showListAction);
    
    QAction* showAdjustAction = m_dock->toggleViewAction();
    showAdjustAction->setText(tr("Painel de Ajustes"));
    viewMenu->addAction(showAdjustAction);
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
    m_rotationSlider->blockSignals(true);
    m_brightnessSlider->setValue(model->brightness());
    m_contrastSlider->setValue(model->contrast());
    m_rotationSlider->setValue(model->rotation());
    m_brightnessSlider->blockSignals(false);
    m_contrastSlider->blockSignals(false);
    m_rotationSlider->blockSignals(false);

    // Sincroniza labels
    m_brightnessLabel->setText(tr("Brilho: %1").arg(model->brightness()));
    m_contrastLabel->setText(tr("Contraste: %1").arg(model->contrast()));
    m_rotationLabel->setText(tr("Rotação: %1°").arg(model->rotation()));

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

void MainWindow::onZoomChanged(int value) {
    m_zoomLabel->setText(tr("Zoom: %1%").arg(value));
    m_imageView->setZoom(value / 100.0);
}

void MainWindow::onRotationChanged(int value) {
    m_rotationLabel->setText(tr("Rotação: %1°").arg(value));
    auto* model = m_collection.current();
    if (model) {
        model->setRotation(value);
        reprocessImage();
    }
}

void MainWindow::updateControls() {
    auto* model = m_collection.current();
    const bool hasImage = model && model->isLoaded();
    m_brightnessSlider->setEnabled(hasImage);
    m_contrastSlider->setEnabled(hasImage);
    m_zoomSlider->setEnabled(hasImage);
    m_rotationSlider->setEnabled(hasImage);
}