#include "ImageViewWidget.h"
#include "../model/ImageModel.h"
#include "../model/ImageCollection.h"
#include <QPainter>
#include <QPaintEvent>
#include <algorithm>
#include <cmath>

ImageViewWidget::ImageViewWidget(ImageCollection* collection, QWidget* parent) : QWidget(parent), m_collection(collection)  
{
    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::black);
    setPalette(pal);
    setAutoFillBackground(true);
}

void ImageViewWidget::paintEvent(QPaintEvent*) {
    QPainter painter(this);  
    auto* model = m_collection->current();
    if (!model || !model->isLoaded()) return;

    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    const QImage& img = model->processed();
    const QSizeF  widgetSize = size();
    const QPointF center(widgetSize.width()  / 2.0, widgetSize.height() / 2.0);

    // Salva a transformação atual para desenhar os overlays depois
    painter.save();
    painter.translate(center + m_panOffset);
    painter.scale(m_zoomFactor, m_zoomFactor);

    const QPointF imgTopLeft(-img.width()  / 2.0, -img.height() / 2.0);
    painter.drawImage(imgTopLeft, img);
    painter.restore();

    // Desenha a medição (Caliper) se estiver ativa
    if (m_isMeasuring || (!m_measureStart.isNull() && !m_measureEnd.isNull())) {
        painter.setRenderHint(QPainter::Antialiasing);
        
        // Converte coordenadas de IMAGEM para WIDGET para desenhar o overlay fixo na imagem
        // (Isso garante que a linha "grude" na imagem ao dar pan/zoom)
        auto mapToWidget = [&](const QPointF& imgPos) {
            return center + m_panOffset + imgPos * m_zoomFactor; // aplica zoom e pan para converter de coordenadas de imagem para widget
        };

        QPointF p1 = mapToWidget(m_measureStart);
        QPointF p2 = mapToWidget(m_measureEnd);

        // Desenha linha
        QPen pen(Qt::yellow, 2, Qt::DashLine);
        painter.setPen(pen);
        painter.drawLine(p1, p2);

        // Desenha "terminais" da régua
        painter.setPen(QPen(Qt::yellow, 2));
        painter.drawEllipse(p1, 3, 3);
        painter.drawEllipse(p2, 3, 3);

        // Calcula distância (simulando calibração médica: 0.1mm por pixel)
        const double pixelDist = std::sqrt(std::pow(m_measureEnd.x() - m_measureStart.x(), 2) + 
                                           std::pow(m_measureEnd.y() - m_measureStart.y(), 2));
        const double mmDist = pixelDist * 0.1; // Simulação de calibração DICOM

        QString text = QString("%1 mm").arg(mmDist, 0, 'f', 2);
        
        // Desenha fundo do texto para legibilidade
        painter.setBrush(Qt::black);
        painter.setPen(Qt::NoPen);
        QRectF textRect = painter.fontMetrics().boundingRect(text);
        textRect.moveCenter((p1 + p2) / 2.0);
        textRect.adjust(-4, -2, 4, 2);
        painter.drawRoundedRect(textRect, 4, 4);

        painter.setPen(Qt::yellow);
        painter.drawText(textRect, Qt::AlignCenter, text);
    }
}

QPointF ImageViewWidget::mapToImage(const QPointF& widgetPos) const {
    const QPointF center(width() / 2.0, height() / 2.0);
    // Operação inversa do paintEvent:
    // (widgetPos - center - panOffset) / zoom
    return (widgetPos - center - m_panOffset) / m_zoomFactor;
}

void ImageViewWidget::setMeasurementMode(bool enabled) {
    m_measurementMode = enabled;
    if (enabled) {
        setCursor(Qt::CrossCursor);
    } else {
        setCursor(Qt::ArrowCursor);
        m_measureStart = QPointF();
        m_measureEnd = QPointF();
        m_isMeasuring = false;
        update();
    }
}

void ImageViewWidget::mousePressEvent(QMouseEvent* event) {
    auto* model = m_collection->current();
    if (!model || !model->isLoaded()) return;

    if (m_measurementMode && event->button() == Qt::LeftButton) {
        m_measureStart = mapToImage(event->position());
        m_measureEnd = m_measureStart;
        m_isMeasuring = true;
        m_dragMode = DragMode::Measuring;
        update();
    } 
    else if (event->button() == Qt::LeftButton) {
        m_lastMousePos = event->pos();
        m_dragMode = DragMode::AdjustingImage;
    } else if (event->button() == Qt::RightButton) {
        m_lastMousePos = event->pos();
        m_dragMode = DragMode::Panning;
        setCursor(Qt::ClosedHandCursor);
    }
}

void ImageViewWidget::mouseMoveEvent(QMouseEvent* event) {
    auto* model = m_collection->current();
    if (!model || !model->isLoaded()) return;

    if (m_dragMode == DragMode::Measuring) {
        m_measureEnd = mapToImage(event->position());
        update();
    }
    else if (m_dragMode == DragMode::AdjustingImage) {
        const QPoint delta = event->pos() - m_lastMousePos;
        m_lastMousePos = event->pos();
        emit brightnessChangedByDrag(delta.x());
        emit contrastChangedByDrag(-delta.y());
    }
    else if (m_dragMode == DragMode::Panning) {
        const QPoint delta = event->pos() - m_lastMousePos;
        m_lastMousePos = event->pos();
        m_panOffset += QPointF(delta);
        update();
    }
}

void ImageViewWidget::mouseReleaseEvent(QMouseEvent* event) {
    if (m_dragMode == DragMode::Measuring) {
        m_isMeasuring = false;
        m_dragMode = DragMode::None;
        update();
    }
    else if (event->button() == Qt::LeftButton) {
        m_dragMode = DragMode::None;
    }
    else if (event->button() == Qt::RightButton) {
        m_dragMode = DragMode::None;
        if (!m_measurementMode) setCursor(Qt::ArrowCursor);
        else setCursor(Qt::CrossCursor);
    }
}

void ImageViewWidget::wheelEvent(QWheelEvent* event) {
    auto* model = m_collection->current();
    if (!model || !model->isLoaded()) return;

    const double factor = 1.0 + (event->angleDelta().y() / m_zoomSensitivity);
    const QPointF mouseWidget = event->position();
    const QPointF center(width() / 2.0, height() / 2.0);

    const double oldZoom = m_zoomFactor;
    m_zoomFactor = std::clamp(m_zoomFactor * factor, 0.05, 20.0);
    const double actualFactor = m_zoomFactor / oldZoom;

    m_panOffset = mouseWidget - center - actualFactor * (mouseWidget - center - m_panOffset);

    update(); 
    event->accept();
}

void ImageViewWidget::resetView() {
    m_zoomFactor = 1.0;
    m_panOffset  = {0.0, 0.0};
    update();
}