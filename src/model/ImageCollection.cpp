#include "ImageCollection.h"
#include <QFileInfo>

std::optional<int> ImageCollection::addImage(const QString& filePath) {
    auto model = std::make_unique<ImageModel>();

    if (!model->loadImage(filePath))
        return std::nullopt;  // falhou — não adiciona nada

    m_images.push_back(std::move(model));  // move, não copia

    // Seleciona automaticamente a imagem recém-adicionada
    m_currentIndex = static_cast<int>(m_images.size()) - 1;

    return m_currentIndex;
}

void ImageCollection::removeImage(int index) {
    if (index < 0 || index >= count()) return;

    m_images.erase(m_images.begin() + index);

    // Ajusta o índice selecionado após remoção
    if (m_images.empty()) {
        m_currentIndex = -1;
    } else {
        // Garante que o índice continua válido
        m_currentIndex = std::min(m_currentIndex, count() - 1);
    }
}

void ImageCollection::selectImage(int index) {
    if (index >= 0 && index < count())
        m_currentIndex = index;
}

ImageModel* ImageCollection::current() {
    if (m_currentIndex < 0) return nullptr;
    return m_images[m_currentIndex].get();
}

const ImageModel* ImageCollection::current() const {
    if (m_currentIndex < 0) return nullptr;
    return m_images[m_currentIndex].get();
}

QString ImageCollection::nameAt(int index) const {
    if (index < 0 || index >= count()) return {};
    return QFileInfo(m_images[index]->filePath()).fileName();
}