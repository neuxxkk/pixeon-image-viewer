# Relatório Técnico — Pixeon Image Viewer (Medical Edition)

## 1. Metodologia Utilizada
O desenvolvimento seguiu o padrão de arquitetura **MVC (Model-View-Controller)** para garantir a separação de responsabilidades:
- **Model (`ImageModel`, `ImageCollection`):** Gerencia os dados da imagem (original, processada) e os parâmetros de estado (brilho, contraste, rotação, inversão).
- **View (`MainWindow`, `ImageViewWidget`):** Responsável pela interface gráfica, exibição da imagem com suporte a zoom/pan e ferramentas de anotação/medição.
- **Controller (`ImageProcessor`):** Contém algoritmos puros de processamento de imagem, isolados da lógica de interface.

## 2. Técnicas Aplicadas
- **Lookup Table (LUT):** Técnica profissional de otimização de imagem. Em vez de recalcular a fórmula de brilho/contraste para cada um dos milhões de pixels, calculamos apenas 256 vezes e mapeamos os valores. Isso reduz drasticamente o consumo de CPU.
- **Caliper Tool (Régua Médica):** Implementação de medição linear com mapeamento inverso de coordenadas. A régua permanece fixa na anatomia da imagem independentemente do nível de zoom ou deslocamento (pan).
- **Calibração Simulada:** A régua exibe valores em **milímetros (mm)**, simulando uma calibração DICOM real de 0.1mm por pixel, essencial para diagnóstico clínico.
- **Inversão de Polaridade (Negativo):** Filtro de inversão de RGB (`invertPixels`) para simular visualização de filmes radiográficos tradicionais.

## 3. Dificuldades Encontradas
- **Implementação da LUT:** A maior dificuldade técnica foi garantir que a LUT lidasse corretamente com a separação de canais RGB sem afetar a performance. O desafio foi mapear a transformação de contraste (que é não-linear e centralizada em 128) junto com o deslocamento de brilho, evitando "estouro" de valores (overflow) através do uso rigoroso de `std::clamp`. Além disso, o acesso direto à memória via `scanLine()` exigiu um cuidado extra com o alinhamento de bytes para evitar corrupção de cor.
- **Gestão de Coordenadas:** Converter o clique do mouse (coordenada do widget) para a posição real na imagem original (coordenada lógica) após rotações e escalas foi um desafio de álgebra linear que exigiu a inversão das matrizes de transformação do `QPainter`.

## 4. Possíveis Melhorias Futuras
- **Processamento em GPU:** Migrar filtros pesados para shaders (OpenGL/Vulkan) via `QOpenGLWidget`.
- **Suporte Nativo a DICOM:** Integrar a biblioteca DCMTK para ler metadados reais de calibração (Pixel Spacing) das imagens médicas.
- **Janelamento (Windowing):** Implementar controles de *Level/Window* específicos para imagens de 12-bit ou 16-bit, comuns em Tomografia.

## 5. Avaliação do Resultado
O projeto evoluiu de um simples visualizador para uma ferramenta de suporte ao diagnóstico médico. A inclusão da régua calibrada em mm e do filtro negativo demonstra uma compreensão clara das necessidades de um sistema PACS (Picture Archiving and Communication System). A arquitetura robusta permite a adição fácil de novos filtros ou ferramentas de medição sem comprometer a performance.
