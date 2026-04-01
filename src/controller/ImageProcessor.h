#pragma once
#include <QImage>

// Classe de algoritmos puros sobre imagens
class ImageProcessor {
public:
    // Sem construtor público — não faz sentido instanciar
    ImageProcessor() = delete;

    // Recebe a imagem original + parâmetros, devolve nova imagem processada
    // Não modifica nada — "pure function" no sentido funcional
    static QImage applyBrightnessContrast(
        const QImage& source,
        int brightness,   // [-100, 100]
        int contrast      // [-100, 100]
    );

    // Rotaciona a imagem em múltiplos de 90 graus
    static QImage rotate(const QImage& source, int degrees);
};