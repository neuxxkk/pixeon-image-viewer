#include "ImageViewWidget.h"
#include "../model/ImageModel.h"
#include "../model/ImageCollection.h"
#include <QPainter>
#include <QPaintEvent>
#include <algorithm>

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

    painter.translate(center + m_panOffset);
    painter.scale(m_zoomFactor, m_zoomFactor);

    const QPointF imgTopLeft(-img.width()  / 2.0, -img.height() / 2.0);
    painter.drawImage(imgTopLeft, img);
}

void ImageViewWidget::mousePressEvent(QMouseEvent* event) {
    auto* model = m_collection->current();
    if (!model || !model->isLoaded()) return;

    if (event->button() == Qt::LeftButton) {
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

    if (m_dragMode == DragMode::AdjustingImage) {
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
    auto* model = m_collection->current();
    if (!model || !model->isLoaded()) return;

    if (event->button() == Qt::LeftButton) {
        m_dragMode = DragMode::None;
    }
    else if (event->button() == Qt::RightButton) {
        m_dragMode = DragMode::None;
        setCursor(Qt::ArrowCursor);
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