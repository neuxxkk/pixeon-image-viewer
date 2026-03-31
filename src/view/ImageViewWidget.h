#pragma once
#include <QWidget>


class ImageModel;

class ImageViewWidget : public QWidget {
    Q_OBJECT;

public:
    explicit ImageViewWidget(ImageModel* model, QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override; // Sobrescreve o método

private:
    ImageModel* m_model;
};