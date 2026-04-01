#include "ImageProcessor.h"
#include <array>
#include <algorithm>  // std::clamp
#include <cmath>      // std::pow (usado futuramente em gamma)

QImage ImageProcessor::applyBrightnessContrast(
    const QImage& source,
    int brightness,
    int contrast)
{
    // Garante que não processamos imagem inválida
    if (source.isNull()) return source;

    // === MONTA A LOOKUP TABLE ===
    //
    // Uma LUT (Lookup Table) é uma tabela de pré-computação.
    // Em vez de calcular a fórmula para cada um dos milhões de pixels,
    // calculamos apenas 256 vezes (os possíveis valores 0..255 de cada canal)
    // e para cada pixel fazemos só uma consulta à tabela — O(1).
    //
    // Exemplo com imagem 1920x1080 (Full HD):
    //   Sem LUT: 1920 * 1080 * 3 canais = ~6.2 milhões de operações float
    //   Com LUT: 256 operações float + ~6.2 milhões de consultas a array
    //   Consulta a array é ~10x mais rápida que operação float
    std::array<uint8_t, 256> lut;

    // Transforma contraste de [-100, 100] para fator multiplicativo [0.0, 2.0]
    //   -100 → 0.0  (achata tudo para cinza médio)
    //      0 → 1.0  (sem alteração)
    //   +100 → 2.0  (dobra o contraste)
    const double contrastFactor = (contrast + 100.0) / 100.0;

    for (int i = 0; i < 256; ++i) {
        // Etapa 1 — Contraste
        // Centraliza em 128, aplica fator, volta ao range original
        // Com fator > 1: valores escuros ficam mais escuros, claros mais claros
        // Com fator < 1: valores convergem para o cinza médio (128)
        double val = contrastFactor * (i - 128.0) + 128.0;

        // Etapa 2 — Brilho
        // Deslocamento linear puro: soma o valor diretamente
        // +brightness clareia, -brightness escurece
        val += static_cast<double>(brightness);

        // Etapa 3 — Clamp
        // OBRIGATÓRIO: sem isso, static_cast<uint8_t> de valor negativo
        // ou > 255 causa undefined behavior (UB) em C++
        lut[i] = static_cast<uint8_t>(std::clamp(val, 0.0, 255.0));
    }

    // === APLICA A LUT PIXEL A PIXEL ===
    //
    // Copiamos a source para não modificar o original
    // (o Model guarda o original intacto para futuras reprocessagens)
    QImage result = source; // QImage usa copy-on-write — cópia é barata

    const int width  = result.width();
    const int height = result.height();

    for (int y = 0; y < height; ++y) {
        // scanLine() retorna ponteiro para o início da linha y na memória
        // É acesso direto ao buffer interno da imagem — sem overhead de pixel()
        //
        // Por que reinterpret_cast<QRgb*>?
        // scanLine() retorna uchar* (byte cru)
        // QRgb é uint32_t — 4 bytes por pixel no formato RGB32
        // O cast diz: "trate essa memória como array de QRgb"
        // É seguro aqui porque garantimos Format_RGB32 no loadFromFile()
        QRgb* line = reinterpret_cast<QRgb*>(result.scanLine(y));

        for (int x = 0; x < width; ++x) {
            const QRgb pixel = line[x];

            // qRed/qGreen/qBlue extraem cada canal como int (0..255)
            // lut[] aplica a transformação pré-calculada
            // qRgb() remonta o pixel com os canais transformados
            line[x] = qRgb(
                lut[qRed(pixel)],
                lut[qGreen(pixel)],
                lut[qBlue(pixel)]
            );
        }
    }

    return result; // NRVO: o compilador elimina a cópia de retorno
}