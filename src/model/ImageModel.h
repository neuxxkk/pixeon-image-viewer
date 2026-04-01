#pragma once
#include <QImage>
#include <QString>
#include <optional>

class ImageModel {
public:
    // Carregamento
    bool loadImage(const QString& filePath);
    void clear();

    // Consultas de estado
    bool isLoaded() const { return m_original.has_value(); }
    QString filePath() const { return m_filePath; }

    // Acesso às imagens
    // original(): a imagem como saiu do disco, nunca modificada
    // processed(): resultado após aplicar brilho/contraste
    const QImage& original()  const { return m_original.value(); }
    const QImage& processed() const { return m_processed; }

    // Escrita de processed() — chamado pelo Controller após processar
    // O Model não sabe como a imagem foi processada, só armazena
    void setProcessed(const QImage& img) { m_processed = img; }

    // getters & setters
    int  brightness() const { return m_brightness; }
    int  contrast()   const { return m_contrast; }
    int  rotation()   const { return m_rotation; }
    bool isInverted() const { return m_inverted; }
    void setBrightness(int value) { m_brightness = value; }
    void setContrast(int value)   { m_contrast   = value; }
    void setRotation(int value)   { m_rotation   = value; }
    void setInverted(bool value)  { m_inverted   = value; }

private:
    std::optional<QImage> m_original;
    QImage   m_processed;
    QString  m_filePath;
    int      m_brightness{0};
    int      m_contrast{0};
    int      m_rotation{0};
    bool     m_inverted{false};
};