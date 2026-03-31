#include "ImageModel.h"
#include <algorithm> // std::clamp

bool ImageModel::loadImage(const QString& filePath) {
    QImage img;

    // BPM, PNG, JPEG, etc. - Qt suporta muitos formatos
    if (!img.load(filePath)) {
        return false; // Falha ao carregar a imagem
    }

    // Converte para Format_RGB32 imediatamente
    // Motivo: garante que TODO pixel tem 32 bits (R, G, B + 8 bits ignorados)
    // Sem isso, imagens PNG com transparência (ARGB) ou paletas (Indexed8)
    // causariam comportamento diferente nos nossos algoritmos
    m_original = img.convertToFormat(QImage::Format_RGB32);

    m_brightness = 0; // Reseta brilho
    m_contrast = 0; // Reseta contraste

    m_processed = m_original.value(); // Inicialmente, processada é igual à original

    return true;
}

void ImageModel::setBrightness(int value) {
    m_brightness = std::clamp(value, -100, 100); // Limita entre -100 e 100
    applyAdjustments(); // Recalcula a imagem processada
}

void ImageModel::setContrast(int value) {
    m_contrast = std::clamp(value, -100, 100); // Limita entre -100 e 100
    applyAdjustments(); // Recalcula a imagem processada
}

/**
 * @brief Aplica ajustes de brilho e contraste à imagem carregada.
 *
 * Este método utiliza uma Look-Up Table (LUT) para calcular e aplicar os ajustes de brilho e contraste
 * em cada valor possível de pixel (0-255) para os canais R, G, B e A. O fator de contraste é calculado
 * com base no valor de m_contrast, variando de 0.0 (imagem totalmente cinza) até 2.0 (contraste dobrado).
 * O brilho é ajustado somando m_brightness ao valor do pixel. Os valores resultantes são limitados ao
 * intervalo [0, 255] e convertidos para uint8_t usando static_cast.
 *
 * @note O uso de static_cast<uint8_t> garante que o valor final seja convertido explicitamente para o tipo
 *       de dado esperado pela LUT (uint8_t), evitando possíveis warnings ou comportamentos indesejados
 *       devido à conversão implícita de double para uint8_t.
 */
void ImageModel::applyAdjustments() {
    if (!isLoaded()) {
        return; 
    }

    // LUT - Look-Up Table para brilho e contraste
    std::array<uint8_t, 256> lut;

    // Calcula o fator de contraste
    // -100 -> 0.0 (sem contraste - imagem cinza)
    // 0 -> 1.0 (contraste normal - imagem original)
    // +100 -> 2.0 (contraste dobrado - realça diferenças)
    double contrastFactor = (100.0 + m_contrast) / 100.0;

    // Preenche a LUT com base no brilho e contraste
    // Isso é em cada possivel 255 valores de pixel (0-255) Tanto R, G, B, A
    for (int i = 0; i < 256; ++i) {

        double val = contrastFactor * (i - 128) + 128; // Aplica contraste

        val += m_brightness; // Aplica brilho

        // Limita o valor entre 0 e 255
        lut[i] = static_cast<uint8_t>(std::clamp(val, 0.0, 255.0)); 
    }

    m_processed = m_original.value(); // Começa com a imagem original

    const int width = m_processed.width();
    const int height = m_processed.height();

    for (int y = 0; y < height; ++y) {

        // QRgb: typedef para uint32_t, onde cada byte representa um canal (R, G, B, A)
        QRgb* line = reinterpret_cast<QRgb*>(m_processed.scanLine(y)); // ptr para a primeira linha

        for (int x = 0; x < width; x++) {
            const QRgb pixel = line[x]; // Pixel original

            line[x] = qRgb(
                lut[qRed(pixel)],   // Ajusta o canal R usando a LUT
                lut[qGreen(pixel)], // Ajusta o canal G usando a LUT
                lut[qBlue(pixel)]  // Ajusta o canal B usando a LUT
            );
        };
    }

}