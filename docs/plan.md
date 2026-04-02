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
- [X] Configurar `CMakeLists.txt` com target principal + target de testes
- [X] Inicializar repositório Git com `.gitignore` adequado para Qt/CMake
- [X] Criar `README.md` inicial com instruções de build

---

### ✅ Fase 1 — Core: Abertura e Exibição de Imagem 

Este é o requisito mais importante segundo o enunciado: *"a imagem aberta deve ser apresentada na aplicação, essa imagem é parte mais importante dela"*.

- [X] Implementar `ImageModel` para encapsular estado da imagem atual
- [X] Implementar `MainWindow` com menu `File > Open` usando `QFileDialog`
- [X] Implementar `ImageViewWidget` herdando de `QWidget` com `paintEvent` customizado
- [X] Suportar BMP, PNG e JPEG no filtro do `QFileDialog`

---

### ✅ Fase 2 — Algoritmos de Brilho e Contraste

O algoritmo deve rodar em CPU de forma eficiente pixel a pixel. Use uma **lookup table (LUT)** — é a técnica profissional e demonstra conhecimento de performance.

- [x] Implementar `ImageProcessor::applyBrightnessContrast()` com LUT
- [x] Conectar os dois `QSlider` ao modelo via signals/slots
- [x] Garantir que os sliders ficam desabilitados sem imagem aberta (req. 4a)

---

### ✅ Fase 3 — Bônus Estratégicos

#### Bônus 4c — Arrastar mouse para brilho/contraste (ALTO impacto, BAIXO custo)

- [x] Sobrescrever `mousePressEvent`, `mouseMoveEvent`, `mouseReleaseEvent` em `ImageViewWidget`
- [x] Movimento horizontal → brilho; vertical → contraste
- [x] Emitir sinais que atualizam os sliders

#### Bônus 6a/6b — Zoom e Pan (MÉDIO impacto, MÉDIO custo)

- [X] Adicionar `m_zoomFactor` e `m_panOffset` ao `ImageViewWidget`
- [X] Zoom com scroll do mouse (`wheelEvent`)
- [X] Pan com botão direito pressionado + arrastar

#### Bônus 5 — Múltiplas imagens (BAIXO custo com a arquitetura certa)

- [x] Usar `QListWidget` lateral com miniaturas
- [x] `ImageModel` vira uma coleção: `std::vector<ImageModel>`
- [x] Seleção na lista troca a imagem exibida

---

### ✅ Fase 4 — Testes Unitários

Não precisa ser extenso — precisa existir e ser relevante.

- [x] Testar `ImageProcessor::applyBrightnessContrast()` com casos extremos
- [x] Testar `ImageModel::loadFromFile()` com arquivo válido e inválido
- [x] Testar que brilho=0, contraste=0 retorna imagem idêntica à original

---

### ✅ Fase 5 — Relatório Técnico e Entrega

O relatório é **diferencial enorme** para vaga júnior. Demonstra maturidade profissional.

- [x] Escrever relatório em PDF ou Markdown cobrindo:
  - **Metodologia**: MVC, separação de responsabilidades, por que CMake
  - **Formato escolhido**: os três (BMP/PNG/JPEG) + justificativa
  - **Técnicas aplicadas**: LUT para performance, `std::optional` para estado nulo seguro, RAII
  - **Dificuldades encontradas**: honestidade sobre desafios técnicos
  - **Melhorias futuras**: processamento em thread separada, suporte a DICOM, histograma
  - **Autoavaliação**: o que foi bem, o que faria diferente
- [x] Garantir que o `README.md` tem instruções claras de build
- [x] Verificar que o repositório Git tem histórico de commits limpo e mensagens descritivas

---

## Checklist Final de Qualidade

- [x] Código compila sem warnings (fixados warnings de unused variables)
- [x] Nenhum `new` sem correspondente gerenciamento (uso de smart pointers e parent-child Qt)
- [x] Sliders desabilitados antes de abrir imagem ✓
- [x] Nenhum crash com imagem corrompida ou caminho inválido
- [x] Todos os testes passando (11/11 testes QTest)
- [x] `.gitignore` exclui pasta `build/`, binários, `.user` do Qt Creator
- [x] Relatório técnico e README completos
