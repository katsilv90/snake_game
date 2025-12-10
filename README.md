Jogo Snake em C++


Jogo clássico da Snake desenvolvido em C++ para consola Windows com cores ANSI. O objetivo é controlar a cobra, comer as frutas e evitar bater nas paredes ou na própria cauda.

Funcionalidades

Modos de jogo:

Normal

Difícil (velocidade aumenta ao comer frutas)

Painel de pontuação com registo de múltiplos jogadores

Guardar pontuações em ficheiro (pontuacoes.txt)

Menu de instruções e menu pós-jogo

Controlo completo com as teclas:

W = cima

A = esquerda

S = baixo

D = direita

X = sair do jogo ou voltar ao menu

Como Executar

Certificar que tens um compilador C++ compatível com Windows (Visual Studio, MinGW, etc.).

Clonar ou descarregar o repositório.

Compilar o código:

g++ main.cpp -o snake.exe


Executar o programa:

./snake.exe


Navegar pelo menu principal para jogar, ver pontuações ou instruções.

Estrutura do Código

Variáveis globais: posição da cobra, fruta, pontuação e velocidade

Funções principais:

Setup() → inicializa o jogo

Draw() → desenha o tabuleiro, cobra e frutas

Input() → captura teclas do jogador

Logic() → lógica do jogo (movimento, colisões e pontuação)

jogar() → inicia o loop principal do jogo

menu() → menu principal

mostrarPainelPontuacao() → exibe e guarda pontuações

mostrarInstrucoes() → mostra instruções do jogo

Tecnologias e Bibliotecas

Linguagem: C++

Consola Windows (<conio.h>, <windows.h>)

Manipulação de cores ANSI

Arquivos para guardar pontuações (<fstream>)

Observações

Funciona melhor em consola Windows

A velocidade da cobra é ajustável dependendo do modo de jogo

Permite registar múltiplos jogadores e guardar resultados em ficheiro
