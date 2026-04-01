# Relatório Técnico — Pixeon Image Viewer

## 1. Metodologia Utilizada
O desenvolvimento seguiu o padrão de arquitetura **MVC (Model-View-Controller)** para garantir a separação de responsabilidades:
- **Model (`ImageModel`, `ImageCollection`):** Gerencia os dados da imagem (original, processada) e os parâmetros de estado (brilho, contraste, rotação).
- **View (`MainWindow`, `ImageViewWidget`):** Responsável pela interface gráfica e exibição da imagem com suporte a zoom e pan via `QPainter`.
- **Controller (`ImageProcessor`):** Contém algoritmos puros de processamento de imagem, isolados da lógica de interface.

O fluxo de trabalho foi incremental, começando pelo carregamento básico e evoluindo para filtros, manipulação interativa e suporte a múltiplas imagens.

## 2. Técnicas Aplicadas
- **Lookup Table (LUT):** Implementada para otimizar o ajuste de brilho e contraste, reduzindo a complexidade de milhões de operações de ponto flutuante para apenas 256 cálculos pré-computados por aplicação.
- **Matrizes de Transformação (`QTransform`):** Utilizadas para rotação e escala de forma eficiente.
- **Coordenadas de View/Pan:** O cálculo de zoom foi implementado para manter o ponto sob o cursor fixo, utilizando deltas de deslocamento proporcional.
- **Smart Pointers e Optional:** Uso de `std::unique_ptr` e `std::optional` para gestão segura de memória e estados de ausência de dados.

## 3. Dificuldades Encontradas
- **Cálculo de Pan no Zoom:** Ajustar o `m_panOffset` para que a imagem não "saltasse" ao atingir os limites de zoom (`std::clamp`) exigiu o uso do fator de zoom real em vez do delta do scroll.
- **Sincronização de Estado:** Garantir que todos os componentes da UI (sliders, labels, lista de imagens) refletissem o estado do Model ao alternar entre múltiplas imagens abertas.

## 4. Possíveis Melhorias Futuras
- **Processamento em GPU:** Migrar filtros pesados para shaders (OpenGL/Vulkan) via `QOpenGLWidget`.
- **Undo/Redo:** Implementar o padrão *Command* para permitir desfazer alterações.
- **Suporte a DICOM:** Integrar bibliotecas como GDCM ou DCMTK para visualização de imagens médicas reais.
- **Filtros Adicionais:** Implementar filtros de convolução (Blur, Sharpen) e histograma em tempo real.

## 5. Avaliação do Resultado
O projeto cumpre todos os requisitos mandatórios e bônus propostos. A performance de processamento é excelente devido ao uso de LUTs e acesso direto à memória (`scanLine`). A interface é intuitiva, permitindo ajustes rápidos tanto por sliders quanto por interação direta com o mouse. O sistema de testes cobre as funcionalidades críticas, garantindo a robustez do motor de processamento.
