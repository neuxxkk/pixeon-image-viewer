# Plano de Ação — Desafio Técnico Pixeon (C++ / Qt)
#### By: Vitor Neuenschwander

## Análise do Desafio

O que vai ser avaliado:

- **Clareza e documentação** — código legível, bem comentado, relatório técnico caprichado
- **C++ Moderno (14+)** — smart pointers, lambdas, range-based for, RAII
- **Qt** — conhecimento do framework e seus idiomas
- **Testes unitários** — mesmo que básicos, precisam existir
- **Eficiência e corretude** — algoritmos de imagem funcionando bem

---

## Decisões de Arquitetura

### Stack escolhida

| Camada | Escolha | Justificativa |
|---|---|---|
| Linguagem | C++17 | Padrão do mercado, suportado, traz `std::optional`, `if constexpr`, structured bindings |
| UI | Qt 6 | Requisito do teste |
| Build | CMake | Mais moderno que qmake, melhor suporte a IDEs e CI |
| Testes | Qt Test (QTest) | Já incluso no Qt, sem dependência extra |
| Imagens | QImage + algoritmos próprios | Evita dependência pesada; Qt já lida com BMP/PNG/JPEG nativamente |

### Formatos de imagem suportados (bônus 1b)
Suportar os três: **BMP, PNG e JPEG** — o Qt nativo resolve tudo via `QImage::load()`.

### Padrão arquitetural: MVC leve
```
pixeon-image-viewer/
├── CMakeLists.txt          ← raiz
├── src/
│   ├── CMakeLists.txt      ← target da aplicação
│   ├── main.cpp
│   ├── model/
│   ├── view/
│   └── controller/
└── tests/
    └── CMakeLists.txt      ← target dos testes

```

---

## Plano de Desenvolvimento — Fases

---

### ✅ Fase 0 — Setup do Projeto

- [X] Instalar Qt 6 + Qt Creator (ou usar VSCode com extensão Qt)
- [X] Criar projeto CMake com estrutura de pastas acima
- [ ] Configurar `CMakeLists.txt` com target principal + target de testes
- [ ] Inicializar repositório Git com `.gitignore` adequado para Qt/CMake
- [ ] Criar `README.md` inicial com instruções de build

---

### ✅ Fase 1 — Core: Abertura e Exibição de Imagem 

Este é o requisito mais importante segundo o enunciado: *"a imagem aberta deve ser apresentada na aplicação, essa imagem é parte mais importante dela"*.

- [ ] Implementar `ImageModel` para encapsular estado da imagem atual
- [ ] Implementar `MainWindow` com menu `File > Open` usando `QFileDialog`
- [ ] Implementar `ImageViewWidget` herdando de `QWidget` com `paintEvent` customizado
- [ ] Suportar BMP, PNG e JPEG no filtro do `QFileDialog`

```cpp
// model/ImageModel.h
#pragma once
#include <QImage>
#include <optional>
#include <QString>

class ImageModel {
public:
    bool loadFromFile(const QString& filePath);
    bool isLoaded() const { return m_original.has_value(); }

    const QImage& original() const { return m_original.value(); }
    const QImage& processed() const { return m_processed; }

    void setBrightness(int value);   // [-100, 100]
    void setContrast(int value);     // [-100, 100]
    int brightness() const { return m_brightness; }
    int contrast() const { return m_contrast; }

private:
    void applyAdjustments();

    std::optional<QImage> m_original;
    QImage m_processed;
    int m_brightness{0};
    int m_contrast{0};
};
```

```cpp
// Abertura de arquivo em MainWindow.cpp
void MainWindow::onOpenFile() {
    const QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("Abrir Imagem"),
        QDir::homePath(),
        tr("Imagens (*.bmp *.png *.jpg *.jpeg)")
    );

    if (filePath.isEmpty()) return;

    if (!m_model->loadFromFile(filePath)) {
        QMessageBox::warning(this, tr("Erro"), tr("Não foi possível abrir a imagem."));
        return;
    }

    // Habilita controles apenas com imagem carregada
    m_brightnessSlider->setEnabled(true);
    m_contrastSlider->setEnabled(true);
    m_imageView->update();
}
```

---

### ✅ Fase 2 — Algoritmos de Brilho e Contraste (2–3h)

O algoritmo deve rodar em CPU de forma eficiente pixel a pixel. Use uma **lookup table (LUT)** — é a técnica profissional e demonstra conhecimento de performance.

- [ ] Implementar `ImageProcessor::applyBrightnessContrast()` com LUT
- [ ] Conectar os dois `QSlider` ao modelo via signals/slots
- [ ] Garantir que os sliders ficam desabilitados sem imagem aberta (req. 4a)

```cpp
// controller/ImageProcessor.cpp
QImage ImageProcessor::applyBrightnessContrast(
    const QImage& source, int brightness, int contrast)
{
    // Monta LUT de 256 entradas — O(1) por pixel
    std::array<uint8_t, 256> lut;

    // Contraste: fator no intervalo [0.0, 2.0]
    const double contrastFactor = (contrast + 100.0) / 100.0;

    for (int i = 0; i < 256; ++i) {
        // 1. Aplica contraste em torno do ponto médio (128)
        double val = contrastFactor * (i - 128.0) + 128.0;
        // 2. Aplica brilho (deslocamento linear)
        val += brightness;
        // 3. Clamp para [0, 255]
        lut[i] = static_cast<uint8_t>(std::clamp(val, 0.0, 255.0));
    }

    // Aplica a LUT em cada pixel
    QImage result = source.convertToFormat(QImage::Format_RGB32);
    const int w = result.width();
    const int h = result.height();

    for (int y = 0; y < h; ++y) {
        QRgb* line = reinterpret_cast<QRgb*>(result.scanLine(y));
        for (int x = 0; x < w; ++x) {
            const QRgb px = line[x];
            line[x] = qRgb(lut[qRed(px)], lut[qGreen(px)], lut[qBlue(px)]);
        }
    }
    return result;
}
```

---

### ✅ Fase 3 — Bônus Estratégicos (3–4h)

Escolha os bônus com melhor custo-benefício. Recomendo esta ordem:

#### Bônus 4c — Arrastar mouse para brilho/contraste (ALTO impacto, BAIXO custo)

- [ ] Sobrescrever `mousePressEvent`, `mouseMoveEvent`, `mouseReleaseEvent` em `ImageViewWidget`
- [ ] Movimento horizontal → brilho; vertical → contraste
- [ ] Emitir sinais que atualizam os sliders

```cpp
// view/ImageViewWidget.cpp
void ImageViewWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_dragStartPos = event->pos();
        m_dragging = true;
    }
}

void ImageViewWidget::mouseMoveEvent(QMouseEvent* event) {
    if (!m_dragging) return;

    const QPoint delta = event->pos() - m_dragStartPos;
    m_dragStartPos = event->pos();

    // Sensibilidade: 1px = 1 unidade de ajuste
    emit brightnessChangedByDrag(delta.x());
    emit contrastChangedByDrag(-delta.y()); // invertido: arrasta pra cima = mais contraste
}
```

#### Bônus 6a/6b — Zoom e Pan (MÉDIO impacto, MÉDIO custo)

- [ ] Adicionar `m_zoomFactor` e `m_panOffset` ao `ImageViewWidget`
- [ ] Zoom com scroll do mouse (`wheelEvent`)
- [ ] Pan com botão direito pressionado + arrastar

```cpp
void ImageViewWidget::wheelEvent(QWheelEvent* event) {
    const double delta = event->angleDelta().y() / 120.0;
    m_zoomFactor = std::clamp(m_zoomFactor * std::pow(1.15, delta), 0.1, 10.0);
    update();
}

void ImageViewWidget::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.translate(m_panOffset);
    painter.scale(m_zoomFactor, m_zoomFactor);
    painter.drawImage(0, 0, m_model->processed());
}
```

#### Bônus 5 — Múltiplas imagens (BAIXO custo com a arquitetura certa)

- [ ] Usar `QListWidget` lateral com miniaturas
- [ ] `ImageModel` vira uma coleção: `std::vector<ImageModel>`
- [ ] Seleção na lista troca a imagem exibida

---

### ✅ Fase 4 — Testes Unitários (1–2h)

Não precisa ser extenso — precisa existir e ser relevante.

- [ ] Testar `ImageProcessor::applyBrightnessContrast()` com casos extremos
- [ ] Testar `ImageModel::loadFromFile()` com arquivo válido e inválido
- [ ] Testar que brilho=0, contraste=0 retorna imagem idêntica à original

```cpp
// tests/tst_imageprocessor.cpp
#include <QtTest>
#include "../src/controller/ImageProcessor.h"

class TestImageProcessor : public QObject {
    Q_OBJECT

private slots:
    void testNeutralAdjustment() {
        QImage img(10, 10, QImage::Format_RGB32);
        img.fill(QColor(100, 150, 200));

        const QImage result = ImageProcessor::applyBrightnessContrast(img, 0, 0);

        // brightness=0, contrast=0 não deve alterar os pixels
        QCOMPARE(result.pixel(5, 5), img.pixel(5, 5));
    }

    void testBrightnessClamp() {
        QImage img(10, 10, QImage::Format_RGB32);
        img.fill(QColor(250, 250, 250));

        const QImage result = ImageProcessor::applyBrightnessContrast(img, 100, 0);
        const QColor px(result.pixel(0, 0));

        // Valores devem ser clamped em 255, nunca overflow
        QCOMPARE(px.red(), 255);
        QCOMPARE(px.green(), 255);
        QCOMPARE(px.blue(), 255);
    }
};

QTEST_MAIN(TestImageProcessor)
#include "tst_imageprocessor.moc"
```

---

### ✅ Fase 5 — Relatório Técnico e Entrega (1–2h)

O relatório é **diferencial enorme** para vaga júnior. Demonstra maturidade profissional.

- [ ] Escrever relatório em PDF ou Markdown cobrindo:
  - **Metodologia**: MVC, separação de responsabilidades, por que CMake
  - **Formato escolhido**: os três (BMP/PNG/JPEG) + justificativa
  - **Técnicas aplicadas**: LUT para performance, `std::optional` para estado nulo seguro, RAII
  - **Dificuldades encontradas**: seja honesto — recrutadores valorizam isso
  - **Melhorias futuras**: processamento em thread separada (`QtConcurrent`), suporte a DICOM (contexto Pixeon!), histograma, filtros convolucionais
  - **Autoavaliação**: o que foi bem, o que faria diferente
- [ ] Executar `windeployqt` para empacotar o `.exe` com as DLLs do Qt
- [ ] Garantir que o `README.md` tem instruções claras de build
- [ ] Verificar que o repositório Git tem histórico de commits limpo e mensagens descritivas

---

## Checklist Final de Qualidade

Antes de entregar, passe por essa lista:

- [ ] Código compila sem warnings em nível `-Wall`
- [ ] Nenhum `new` sem correspondente gerenciamento (prefira `std::make_unique`, `QPointer`, parent-child Qt)
- [ ] Sliders desabilitados antes de abrir imagem ✓
- [ ] Nenhum crash com imagem corrompida ou caminho inválido
- [ ] Todos os testes passando (`ctest` ou executável de testes)
- [ ] `.gitignore` exclui pasta `build/`, binários, `.user` do Qt Creator
- [ ] Executável Windows funciona em máquina limpa (sem Qt instalado)

---

## Dica Final — O que realmente vai te destacar

Na Pixeon, que lida com imagens médicas (DICOM, PACS), demonstrar que você **pensa em qualidade de imagem e performance** é ouro. No relatório, mencione explicitamente:

> *"Em um contexto de imagens médicas como DICOM, o processamento pixel a pixel via LUT poderia ser expandido para suportar imagens de 16 bits e metadados DICOM, garantindo fidelidade diagnóstica."*

Essa frase mostra que você entendeu o negócio da empresa — e isso vale mais do que qualquer bônus de código.