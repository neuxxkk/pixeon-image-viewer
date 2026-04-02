# Pixeon Image Viewer (Medical Edition)

Visualizador de imagens médicas desenvolvido para o desafio técnico da Pixeon. O projeto utiliza C++17 e Qt 6, seguindo a arquitetura MVC.

## Download

| Versão | Sistema Operacional | Arquivo |
|--------|---------------------|---------|
| v1.0   | Windows (x64)       | [PixeonImageViewer-v1.0-Windows.zip](https://github.com/neuxxkk/pixeon-image-viewer/raw/main/PixeonImageViewer-v1.0-Windows.zip) |
| v1.0   | —                   | [Relatório Técnico.pdf](https://github.com/neuxxkk/pixeon-image-viewer/raw/main/Relat%C3%B3rio%20T%C3%A9cnico.pdf) |

> **Instalação:** Extraia o arquivo `.zip` e execute `PixeonImageViewer.exe`. Nenhuma instalação adicional é necessária.

## Funcionalidades Core
- **Abertura de Imagens:** Suporte a BMP, PNG e JPEG.
- **Ajustes de Imagem:** Brilho e Contraste otimizados via Lookup Table (LUT).
- **Transformações:** Rotação por slider e Inversão de Cores (Filtro Negativo).
- **Visualização Interativa:** Zoom (scroll) e Pan (botão direito) suaves.
- **Ferramenta Médica (Caliper):** Régua de medição calibrada em milímetros (mm).
- **Múltiplas Imagens:** Gerenciamento de lista de imagens com miniaturas.

## Requisitos
- **Qt 6** (Core, Gui, Widgets, Test)
- **CMake 3.16+**
- **Compilador C++17** (GCC, Clang ou MSVC)

## Como Compilar (Linux/macOS)
```bash
mkdir build && cd build
cmake ..
cmake --build .
```

## Como Compilar (Windows - Command Prompt)
```cmd
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

## Como Executar os Testes
```bash
cd build
./tests/PixeonTests
```

## Atalhos de Teclado
- `Ctrl + O`: Abrir imagem
- `Ctrl + S`: Salvar imagem
- `Ctrl + R`: Resetar Zoom/Pan
- `Ctrl + I`: Inverter Cores
- `Ctrl + M`: Ativar/Desativar Régua de Medição

---
Desenvolvido por **Vitor Neuenschwander**
