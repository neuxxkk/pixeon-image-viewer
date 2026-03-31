#include "ImageViewWidget.h"
#include "../model/ImageModel.h"
#include <QPainter>
#include <QPaintEvent>

ImageViewWidget::ImageViewWidget(ImageModel* model, QWidget* parent)
    : QWidget(parent)   // inicializa a classe base
    , m_model(model)    // inicializa o ponteiro (lista de inicialização — C++ idiomático)
{
    // Fundo preto quando não há imagem
    // QPalette controla cores dos elementos do widget
    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::black);
    setPalette(pal);
    setAutoFillBackground(true);  // Qt preenche o fundo com a cor acima
}

void ImageViewWidget::paintEvent(QPaintEvent* /*event*/) {
    // QPainter é o objeto de desenho do Qt
    // Sempre criado no stack dentro do paintEvent — nunca guarde como membro
    QPainter painter(this);

    // Se não há imagem, não há nada para desenhar
    if (!m_model->isLoaded()) return;

    // SmoothPixmapTransform: usa interpolação bilinear ao escalar
    // Sem isso, zoom mostra pixels "quadrados" (nearest neighbor)
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    const QImage& img = m_model->processed();

    // Escala a imagem para caber no widget mantendo proporção (aspect ratio)
    // Qt::KeepAspectRatio: nunca distorce
    // Qt::SmoothTransformation: interpolação de qualidade
    const QSize  widgetSize = size();
    const QImage scaled = img.scaled(widgetSize,
                                     Qt::KeepAspectRatio,
                                     Qt::SmoothTransformation);

    // Centraliza a imagem no widget
        // (widget pode ser maior que a imagem escalada)
    const int x = (widgetSize.width()  - scaled.width())  / 2;
    const int y = (widgetSize.height() - scaled.height()) / 2;

    painter.drawImage(x, y, scaled);
}