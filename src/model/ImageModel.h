# pragma once
#include <QImage>
#include <optional>
#include <QString>

class ImageModel {
public:
    // Setter e getter para carregar imagem, brilho e contraste
    bool loadImage(const QString& filePath); // & = pass by reference

    bool isLoaded() const {return m_original.has_value();}

    const QImage& original() const {return m_original.value();}
    const QImage& processed() const {return m_processed;}   

    void setBrightness(int value);
    void setContrast(int value);
    int brightness() const {return m_brightness;}
    int contrast() const {return m_contrast;}

private:    
    void applyAdjustments(); // Recalcula a imagem processada com base na original, brilho e contraste

    std::optional<QImage> m_original; // std::optional para lidar com ausência de imagem
    QImage m_processed; // Imagem ajustada

    int m_brightness{0}; // Brilho padrão
    int m_contrast {0}; // Contraste padrão
};
