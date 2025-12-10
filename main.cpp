#include <iostream>
#include <conio.h>
#include <windows.h>
#include <ctime>
#include <vector>
#include <string>
#include <locale.h>
#include <fstream>
#include <ctime>
using namespace std;

// ====== VARIÁVEIS GLOBAIS ======
bool gameOver;    // Indica se o jogo acabou
bool modoDificil;  // Indica se o modo difícil está ativado
bool voltarMenuPrincipal = false; // indica se deve voltar ao menu principal
const int width = 20, height = 20;  // Define a largura e altura do "tabuleiro" do jogo
int x, y, frutaX, frutaY, pontos;   // x & y = posição da cabeça          frutaX & fruta Y = posição da fruta         pontos = contador de pontuação
enum eDirecao {STOP = 0, LEFT, RIGHT, UP, DOWN};     // Enumeração para controlar a direção do movimento da cobra
eDirecao dir;
int caudaX[100], caudaY[100];   // posição da cauda
int nCauda;     // comprimento da cauda
int velocidade;   // velocidade da snake
string nomeJogador;   // nome do jogador
// Cores ANSI
string verde = "\033[32m" ;   // cobra/cabeça
string branco = "\033[37m";    // bordas/espaco
string vermelho = "\033[1;31m"; // fruta
string amarelo = "\033[1;33m"; // nomes e outros textos
string reset = "\033[0m";      // reset

// ====== FUNÇÕES DE CONSOLA ======
// Reposiciona o cursor da consola para o canto superior esquerdo,
// reduzindo flicker ao redesenhar o jogo.
void limparEcra()
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = {0, 0};
    SetConsoleCursorPosition(hOut, pos);
}

// Função para esconder o cursor da consola
void esconderCursor()
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hOut, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hOut, &cursorInfo);
}

// Limpa o ecrã para Windows e Linux
void limparTela()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// ====== ESTRUTURA PARA PAINEL DE PONTUAÇÃO ======
struct Score
{
    string nome;  // Nome do jogador
    int pontos;    // Pontuação do jogador
    string modo; // "Normal" ou "Difícil"
};
vector<Score> painelPontuacao; // Vetor que armazenará múltiplos registros de Score

// ====== SETUP ======
void Setup()
{
    gameOver = false;    // O jogo ainda não terminou
    dir = STOP;               // A cobra não se move inicialmente
    // Define a posição inicial da cabeça da cobra
    x = width / 2;
    y = height / 2;
    // Sorteia a posição da fruta de forma aleatória dentro do tabuleiro
    frutaX = rand() % width;
    frutaY = rand() % height;
    pontos = 0; // Pontuação inicial do jogador
    nCauda = 0; // Inicializa o comprimento da cauda da cobra com zero

    // Inicializa as posições da cauda para zero
    for(int i = 0; i < 100; i++)
    {
        caudaX[i] = 0;
        caudaY[i] = 0;
    }
}

// ====== DESENHO DO TABULEIRO======
void Draw()
{
    limparEcra();         // Limpa o ecrã para redesenhar o jogo

    // Desenha a linha superior da "parede"
    for(int i = 0; i < width + 2; i++)
        cout << branco << "#";
    cout << endl;

    for(int i = 0; i < height; i++)    // Percorre todas as linhas do tabuleiro
    {
        for(int j = 0; j < width; j++)   // Percorre todas as colunas do tabuleiro
        {
            if(j == 0)
                cout << branco << "#";   // Desenha a parede esquerda

            if(i == y && j == x)
                cout << verde <<"O";   // Desenha a cabeça da cobra na posição atual
            else if(i == frutaY && j == frutaX)
                cout << vermelho << "+";  // Desenha a fruta na posição atual
            else
            {
                bool print = false;   // Flag para verificar se a posição é parte da cauda
                // Percorre todas as partes da cauda
                for(int k = 0; k < nCauda; k++)
                {
                    if(caudaX[k] == j && caudaY[k] == i)
                    {
                        cout << verde << "o";  // Desenha a cauda da cobra
                        print = true;
                        break;
                    }
                }
                if(!print)
                    cout << " ";  // Desenha espaço vazio se não houver cabeça, fruta ou cauda
            }

            if(j == width - 1)
                cout << branco << "#";  // Desenha a parede direita no final da linha
        }
        cout << endl;  // Vai para a próxima linha do tabuleiro
    }

// Desenha a linha inferior da parede
    for(int i = 0; i < width + 2; i++)
        cout << branco << "#";
    cout << endl;

// Mostra pontuação, nome do jogador e modo de jogo
    cout << amarelo << " Pontos: " << reset << pontos << endl;
    cout << amarelo << " Jogador: " << reset << nomeJogador << endl;
    cout << amarelo << " Modo de Jogo: " << reset << (modoDificil ? "Difícil" : "Normal") << endl;
// Instrução para sair do jogo
    cout << "\n Pressione " << amarelo << "X " << reset << "para sair do jogo e voltar ao menu.\n" << endl;
}

// ====== INPUT DE TECLAS ======
void Input()
{
    if(_kbhit()) // verifica se uma tecla foi pressionada
    {
        switch(_getch()) // lê a tecla pressionada sem precisar de ENTER
        {
        case 'a':
            dir = LEFT;   // Muda a direção da cobra para a esquerda
            break;
        case 'd':
            dir = RIGHT;  // Muda a direção da cobra para a direita
            break;
        case 'w':
            dir = UP;       // Muda a direção da cobra para cima
            break;
        case 's':
            dir = DOWN;   // Muda a direção da cobra para baixo
            break;
        case 'x':
            gameOver = true;           // termina o loop do jogo
            voltarMenuPrincipal = true; // sinaliza que deve ir para o menu principal
            break;
        }
    }
}

// ====== LÓGICA DO JOGO ======
void Logic()
{
    // Guarda a posição atual da cabeça da cobra
    int prevX = x;
    int prevY = y;

    // Atualiza a posição da cabeça da cobra consoante a direção
    switch(dir)
    {
    case LEFT:
        x--;          // Move a cabeça para a esquerda
        break;
    case RIGHT:
        x++;       // Move a cabeça para a direita
        break;
    case UP:
        y--;        // Move a cabeça para cima
        break;
    case DOWN:
        y++;     // Move a cabeça para baixo
        break;
    default:
        break;
    }

    // Verifica colisão com as paredes do tabuleiro
    if(x >= width || x < 0 || y >= height || y < 0)
        gameOver = true;

    // Verifica colisão da cabeça com a própria cauda
    for(int i = 0; i < nCauda; i++)
        if(caudaX[i] == x && caudaY[i] == y)
            gameOver = true;

    // Atualiza as posições da cauda
    int prev2X, prev2Y;
    for(int i = 0; i < nCauda; i++)
    {
        // Guarda a posição atual da parte da cauda
        prev2X = caudaX[i];
        prev2Y = caudaY[i];
        // Move a parte da cauda para a posição anterior
        caudaX[i] = prevX;
        caudaY[i] = prevY;
        // Atualiza prevX/Y para a próxima iteração
        prevX = prev2X;
        prevY = prev2Y;
    }

    // Verifica se a cabeça comeu a fruta
    if(x == frutaX && y == frutaY)
    {
        pontos += 10;          // Incrementa a pontuação
        // Adiciona nova parte à cauda
        caudaX[nCauda] = prevX;
        caudaY[nCauda] = prevY;
        nCauda++;    // Aumenta o tamanho da cauda

        // Se estiver no modo difícil, aumenta a velocidade
        if(modoDificil && velocidade > 50)
            velocidade -= 10;

        // Gera nova posição para a fruta, evitando colisão com a cobra
        bool frutaValida = false;
        while(!frutaValida)
        {
            frutaX = rand() % width;    // Nova posição X aleatória
            frutaY = rand() % height;    // Nova posição Y aleatória
            frutaValida = true;

            // Verifica se a nova posição coincide com a cabeça
            if(frutaX == x && frutaY == y)
                frutaValida = false;

            // Verifica se a nova posição coincide com a cauda
            for(int i = 0; i < nCauda; i++)
                if(caudaX[i] == frutaX && caudaY[i] == frutaY)
                    frutaValida = false;
        }
    }
}

// ====== PAINEL DE PONTUAÇÃO COM DOWNLOAD TIPO DIÁRIO ======
void mostrarPainelPontuacao()
{
    while(true)
    {
        limparTela();       // Limpa a consola antes de mostrar o painel
        cout << amarelo << " *** PAINEL DE PONTUACAO ***\n" << reset << endl;

        // Verifica se existem jogadores registados
        if(painelPontuacao.empty())
        {
            cout << " Nenhum jogador registrado ainda!\n"<<endl;
        }
        else
        {
            // Mostra cada jogador, a sua pontuação e o modo de jogo
            for(size_t i = 0; i < painelPontuacao.size(); i++)
            {
                cout <<" "<< amarelo << i+1 << ". " << reset
                     << painelPontuacao[i].nome
                     << amarelo << " - " << reset
                     << painelPontuacao[i].pontos
                     << " pontos " << amarelo << "- " << reset
                     << painelPontuacao[i].modo << endl;
            }
        }

        // Menu de opções
        cout << amarelo << "\n * Escolha uma opção *" << reset << endl;
        cout << amarelo << " 1 -" << reset << " Voltar ao Menu Principal\n";
        cout << amarelo << " 2 -" << reset << " Guardar pontuações em ficheiro\n";

        char tecla = _getch();  // Captura tecla imediatamente
        if(tecla == '1')
            return;  // Sai da função e volta ao menu
        else if(tecla == '2')
        {
            ofstream ficheiro("pontuacoes.txt", ios::app);  // Abre em modo append
            if(ficheiro.is_open())
            {
                // Obter data e hora atual
                time_t now = time(0);
                tm *ltm = localtime(&now);
                ficheiro << "\n===== Registo em "
                         << 1900 + ltm->tm_year << "/"
                         << 1 + ltm->tm_mon << "/"
                         << ltm->tm_mday << " "
                         << ltm->tm_hour << ":"
                         << ltm->tm_min << ":"
                         << ltm->tm_sec
                         << " =====\n";

                // Guardar pontuações atuais
                for(size_t i = 0; i < painelPontuacao.size(); i++)
                {
                    ficheiro << i+1 << ". "
                             << painelPontuacao[i].nome
                             << " - " << painelPontuacao[i].pontos
                             << " pontos - " << painelPontuacao[i].modo
                             << endl;
                }
                ficheiro.close();
                cout << "\n Pontuações guardadas em 'pontuacoes.txt'.";
                 cout << "\n - Pressione qualquer tecla para voltar ao menu.\n";
                _getch();
            }
            else
            {
                cout << "\n Erro ao abrir o ficheiro.";
                _getch();
            }
        }
        else
        {
            cout << "\n Opção inválida!";
            _getch();
        }
    }
}
// ====== FUNÇÃO PARA MOSTRAR INSTRUÇÕES======
void mostrarInstrucoes()
{
    limparTela();
    cout << amarelo <<  "*** INSTRUÇÕES ***\n" << reset << endl;
    cout << " Use " << amarelo << "W A S D " << reset << "para mover a Snake.\n";
    cout << " Coma a fruta " << amarelo << "+ " << reset << "para crescer.\n";
    cout << " Não bata nas paredes nem na cauda.\n";
    cout << " Modo de jogo " << amarelo << "Normal " << reset << "mantém a velocidade ao comer a fruta.\n";
    cout << " Modo de jogo " << amarelo << "Difícil " << reset << "aumenta a velocidade ao comer a fruta.\n";
    cout << " Pressione " << amarelo << "X " << reset << "para sair do jogo e voltar ao menu.\n" << endl;
    cout << " - Pressione qualquer tecla para voltar ao menu.\n";

    _getch(); // captura tecla para voltar ao menu
}

// ===== FUNÇÃO MENU PÓS JOGO ========
int menuPosJogo()
{
    while(true)
    {
        limparTela();
        cout << amarelo << "\n * Escolha uma opção *" << reset << endl;
        cout << amarelo << " 1 -" << reset << " Jogar novamente\n";
        cout << amarelo << " 2 -" << reset << " Voltar ao Menu Principal\n";
        cout << amarelo << " 3 -" << reset << " Sair do Jogo/Encerrar\n";


        char tecla = _getch(); // lê tecla imediatamente sem precisar de ENTER
        cout << tecla << endl; // mostra a tecla pressionada

        if(tecla == '1')
            return 1;  // Jogar novamente
        else if(tecla == '2')
            return 2;  // Voltar ao menu principal
        else if(tecla == '3')
            return 3;  // Sair do jogo
        else
        {
            cout << " Opção inválida!\n";
            Sleep(1000);
        }
    }
}

// ====== FUNÇÃO PARA INICIAR O JOGO ======
void jogar(bool dificil, string nomeExistente = "")
{
    modoDificil = dificil;                           // Define se o jogo será em modo difícil ou normal
    velocidade = (dificil ? 100 : 150);       // Ajusta a velocidade da cobra conforme o modo

    // Se já houver um nome existente, usa-o, caso contrário, pede ao jogador
    if(nomeExistente.empty())
    {
        do
        {
            limparTela();               // limpa a tela antes de pedir o nome
            cout << "\n Digite o seu nome: ";
            getline(cin, nomeJogador);       // Lê o nome do jogador

            // Remove espaços extras no início e no fim do nome (para garantir que não pessionam ENTER sem querer)
            nomeJogador.erase(0, nomeJogador.find_first_not_of(" \t\n\r\f\v"));
            nomeJogador.erase(nomeJogador.find_last_not_of(" \t\n\r\f\v") + 1);

        }
        while(nomeJogador.empty());   // Repete enquanto o nome for vazio
    }
    else
    {
        nomeJogador = nomeExistente; // Utiliza o nome existente (para o jogador que quer continuar a jogar)
    }

    bool repetirJogo = true;   // Controla se o jogador quer jogar novamente
    while(repetirJogo)
    {
        limparTela();          // Limpa a tela antes de reiniciar o jogo
        Setup();                // Inicializa o jogo (cobra, fruta, cauda, etc.)
        while(!gameOver)     // Loop principal do jogo
        {
            Draw();                                 // Desenha o tabuleiro, cobra, fruta e pontuação
            Input();                                // Captura a tecla pressionada pelo jogador
            Logic();                                // Atualiza a lógica do jogo (movimento, colisões, pontuação)
            Sleep(velocidade);              // Controla a velocidade do jogo
        }

        cout << " Fim de jogo! Pontos: " << pontos << endl;    // Mostra a pontuação final

        // Guarda a pontuação do jogador no painel
        Score novoScore;
        novoScore.nome = nomeJogador;
        novoScore.pontos = pontos;
        novoScore.modo = (modoDificil ? "Difícil" : "Normal");
        painelPontuacao.push_back(novoScore);

        // Menu pós-jogo com 3 opções
        int escolha = menuPosJogo();
        switch(escolha)
        {
        case 1: // Jogar novamente com o mesmo nome e modo
            gameOver = false;          // Reinicia o jogo
            repetirJogo = true;         // Mantém o loop de repetição
            break;
        case 2: // Voltar ao menu principal
            repetirJogo = false;        // Sai do loop de repetição
            gameOver = false;          // Reseta o estado do jogo
            return;
        case 3: // Sair do jogo
            exit(0);                            // Termina o programa
        }
    }
}

// ====== MENU PRINCIPAL ======
void menu()
{
    while(true)            // Loop infinito para manter o menu ativo até o jogador sair
    {
        limparTela();      // Limpa o ecrã antes de mostrar o menu
        // Cabeçalho do menu
        cout << amarelo << " *** SNAKE GAME ***\n" << reset << endl;
        cout << amarelo << " *Escolha uma opção*\n" << reset;
        // Opções do menu
        cout << amarelo << " 1. " << reset << "Jogo - Normal\n";         // Inicia jogo normal
        cout << amarelo << " 2. " << reset << "Jogo - Difícil\n";         // Inicia jogo difícil
        cout << amarelo << " 3. " << reset << "Painel de Pontuação\n";    // Mostra o painel de pontuação
        cout << amarelo << " 4. " << reset << "Instruções\n";             // Mostra instruções do jogo
        cout << amarelo << " 5. " << reset << "Sair\n";                   // Sai do jogo

        char tecla = _getch();           // Captura imediatamente a tecla pressionada, sem precisar de ENTER
        cout << tecla << endl;           // Mostra a tecla pressionada

        // Processa a escolha do jogador
        switch(tecla)
        {
        case '1':
            jogar(false);                       // Inicia jogo normal
            break;
        case '2':
            jogar(true);                         // Inicia jogo difícil
            break;
        case '3':
            mostrarPainelPontuacao();    // Mostra o painel de pontuação
            break;
        case '4':
            mostrarInstrucoes();             // Mostra as instruções
            break;
        case '5':
            cout << " Encerrando o jogo...\n";
            exit(0);                                  // Termina o programa
        default:                                     // Caso a tecla seja inválida
            cout << " Opção inválida!\n";
            Sleep(1000);                         // Espera 1 segundo antes de voltar ao menu
            break;
        }
    }
}

// ====== MAIN ======
int main()
{
    setlocale(LC_ALL, "");
    srand(time(NULL));
    esconderCursor();
    menu();
    return 0;
}
