Desafio Pixeon - Analista Desenvolvedor C++
Este teste tem como objetivo avaliar suas habilidades em programação C++, utilizando Qt e ferramentas C++.
É esperado que você:
1. Desenvolva uma Aplicação, conforme os requisitos descritos posteriormente;
2. Disponibilize o código fonte da aplicação, juntamente com o arquivo de projeto (qmake ou cmake), em um
repositório GIT;
3. Entregue um executável Windows, com as dependências necessárias;
4. Entregue um Relatório Técnico, descrevendo metodologia utilizada, dificuldades encontradas, técnicas
aplicadas, possíveis melhorias futuras e sua avaliação do resultado.
Requisitos da aplicação
Você deve desenvolver uma aplicação que permita abrir arquivos de imagem e fazer algumas manipulações sobre
ela. A aplicação deve:
1. Ser desenvolvida em C++ Moderno (14+)
2. Utilizar Qt para interface gráfica
3. Pode ser utilizada qualquer biblioteca/algoritmo Boost e STL.
4. Pode ser utilizado qualquer utilitário que o QT forneça em suas bibliotecas.
Requisitos:
1. Deve ser possível abrir um dos seguintes tipos de imagem: BMP, PNG ou JPEG
a. O formato escolhido deve ser descrito no relatório.
b. Bônus: Suportar mais de um dos formatos.
2. Para abrir essa imagem, um diálogo de abertura de arquivo deve ser exibido, para a escolha do local e do
arquivo a ser aberto.
a. Pode ser usado o do Qt (QFileDialog).
3. A imagem aberta deve ser apresentada na aplicação, essa imagem é parte mais importante dela.
4. Deve haver dois controles, onde possa ser alterado o contraste e o brilho da imagem.
a. O comando para isso deve ficar ativo apenas se a imagem estiver aberta.
b. Utilizar dois QSlider, um para alterar o contraste e outro para o brilho.
c. Bônus: Ao se clicar e arrastar o mouse sobre a imagem:
i. Mover na horizontal: Altera o Brilho
ii. Mover na vertical: Altera o Contraste
iii. Os valores devem atualizar os QSliders.
5. Bônus: Possibilitar que mais de uma imagem seja aberta ao mesmo tempo, com uma listagem para seleção.
6. Bônus: Implemente mais funções sobre a imagem:
a. Zoom (escala)
b. Pan (deslocamento)
c. Rotação
d. Cria um controle para seleção de funções
7. Bônus: Surpreenda-nos
Avaliação:
Seu código-fonte será avaliado por:
1. Clareza e documentação
2. Adequação às práticas modernas de desenvolvimento C++
3. Testes unitários
4. Eficiência e corretude