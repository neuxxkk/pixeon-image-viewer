#pragma once
#include "ImageModel.h"
#include <vector>
#include <memory>
#include <optional>

class ImageCollection {
public:
    // Carrega nova imagem e adiciona à coleção
    // Retorna o índice da imagem adicionada, ou nullopt se falhou
    std::optional<int> addImage(const QString& filePath);

    void removeImage(int index);
    void selectImage(int index);

    bool hasImages() const { return !m_images.empty(); }
    int  currentIndex() const { return m_currentIndex; }
    int  count() const { return static_cast<int>(m_images.size()); }

    // Retorna o model ativo — nullptr se não há imagens
    ImageModel* current();
    const ImageModel* current() const;

    // Para popular o QListWidget
    QString nameAt(int index) const;

private:
    std::vector<std::unique_ptr<ImageModel>> m_images;
    int m_currentIndex{-1};  // -1 = nenhuma selecionada
};