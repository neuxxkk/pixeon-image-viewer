#include "ImageProcessor.h"
#include <array>
#include <algorithm>

QImage ImageProcessor::applyBrightnessContrast(const QImage& source, int brightness, int contrast) {
    if (source.isNull()) return source;

    std::array<uint8_t, 256> lut;
    const double contrastFactor = (contrast + 100.0) / 100.0;

    for (int i = 0; i < 256; ++i) {
        double val = contrastFactor * (i - 128.0) + 128.0;
        val += static_cast<double>(brightness);
        lut[i] = static_cast<uint8_t>(std::clamp(val, 0.0, 255.0));
    }

    QImage result = source;
    const int width  = result.width();
    const int height = result.height();

    for (int y = 0; y < height; ++y) {
        QRgb* line = reinterpret_cast<QRgb*>(result.scanLine(y));
        for (int x = 0; x < width; ++x) {
            const QRgb pixel = line[x];
            line[x] = qRgb(
                lut[qRed(pixel)],
                lut[qGreen(pixel)],
                lut[qBlue(pixel)]
            );
        }
    }

    return result;
}

QImage ImageProcessor::rotate(const QImage& source, int degrees) {
    if (source.isNull() || degrees % 360 == 0) return source;

    QTransform transform;
    transform.rotate(degrees);
    return source.transformed(transform, Qt::SmoothTransformation);
}

QImage ImageProcessor::invert(const QImage& source) {
    if (source.isNull()) return source;
    QImage result = source;
    result.invertPixels(QImage::InvertRgb);
    return result;
}