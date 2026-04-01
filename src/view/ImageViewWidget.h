#pragma once
#include <QWidget>

class ImageCollection;  // <- se já migrou para coleção

class ImageViewWidget : public QWidget {
    Q_OBJECT

public:
    explicit ImageViewWidget(ImageCollection* collection, QWidget* parent = nullptr);
    void resetView();
    void setZoomSensitivity(double factor) { m_zoomSensitivity = 1200.0 / factor; }
    void setMeasurementMode(bool enabled);
    bool isMeasurementMode() const { return m_measurementMode; }

signals:
    void brightnessChangedByDrag(int delta);
    void contrastChangedByDrag(int delta);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    enum class DragMode { None, Panning, AdjustingImage, Measuring };

    ImageCollection* m_collection;

    double   m_zoomFactor{1.0};
    QPointF  m_panOffset{0.0, 0.0};
    double   m_zoomSensitivity{1200.0};

    QPoint   m_lastMousePos;
    DragMode m_dragMode{DragMode::None};

    bool     m_measurementMode{false};
    QPointF  m_measureStart;
    QPointF  m_measureEnd;
    bool     m_isMeasuring{false};

    // Converte posição do widget para posição na imagem (levando em conta zoom/pan)
    QPointF mapToImage(const QPointF& widgetPos) const;
};