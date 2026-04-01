#include "ImageModel.h"

bool ImageModel::loadImage(const QString& filePath) {
    QImage img;
    if (!img.load(filePath)) return false;

    // Normaliza para RGB32 ao carregar
    // O Controller sempre receberá imagens nesse formato
    m_original  = img.convertToFormat(QImage::Format_RGB32);
    m_processed = m_original.value();
    m_filePath  = filePath;

    // Reseta parâmetros — nova imagem começa neutra
    m_brightness = 0;
    m_contrast   = 0;

    return true;
}

void ImageModel::clear() {
    m_original.reset();   // esvazia o optional
    m_processed = QImage();
    m_filePath.clear();
    m_brightness = 0;
    m_contrast   = 0;
}