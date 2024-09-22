#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// definindo o tamanho maximo de simbolos, estados, fitas e transicoes
#define MAX_SIMBOLOS 30
#define MAX_ESTADOS 50
#define MAX_FITAS 100
#define MAX_TRANSICOES 100

// estrutura que define as transicoes da maquina de turing
typedef struct {
    char simbolo_atual;       // o simbolo atual que a maquina esta lendo
    char simbolo_gravar;      // o simbolo que deve ser gravado na fita
    char mover;               // direcao para mover a cabeca da fita ('D' = direita, 'E' = esquerda)
    int estado_chegada;       // o estado para o qual a maquina vai transitar
} Transicao;

// estrutura que define a maquina de turing
typedef struct {
    int qtd_estados;                // quantidade de estados na maquina
    int qtd_transicoes;             // quantidade de transicoes na maquina
    Transicao transicoes[MAX_ESTADOS][MAX_TRANSICOES];  // tabela de transicoes
    int estado_aceitacao;           // o estado de aceitacao da maquina
} MaquinaTuring;

// funcao para encontrar a transicao correta com base no estado atual e no simbolo que esta sendo lido
int procurar_transicao(MaquinaTuring* mt, int estado_atual, char simbolo_atual) {
    // percorre todas as transicoes e verifica se existe uma transicao para o simbolo atual
    for (int i = 0; i < mt->qtd_transicoes; i++) {
        if (mt->transicoes[estado_atual][i].simbolo_atual == simbolo_atual) {
            return i;  // retorna o indice da transicao encontrada
        }
    }
    return -1;  // retorna -1 se nao houver transicao
}

// funcao que processa a palavra na fita de entrada usando a maquina de turing
char* processar_palavra(MaquinaTuring* mt, char* fita) {
    int estado_atual = 1;  // comeca no estado inicial, que e o estado 1
    int posicao = 0;       // posicao inicial na fita
    bool estado_aceitacao = true;  // variavel para verificar se a palavra foi aceita
    char* palavra = strdup(fita);  // cria uma copia da fita para preservar o valor original

    // adiciona um simbolo de branco ao final da fita para evitar erros na leitura
    strcat(fita, "-");

    // enquanto a maquina nao alcancar o estado de aceitacao ou rejeicao
    while (estado_atual != mt->estado_aceitacao) {
        char simbolo = fita[posicao];  // pega o simbolo atual na posicao da fita
        int indice_transicao = procurar_transicao(mt, estado_atual, simbolo);  // procura a transicao com base no estado e simbolo

        // se nao encontrar transicao, a palavra sera rejeitada
        if (indice_transicao == -1) {
            estado_aceitacao = false;  // marca como rejeitada
            break;
        }

        Transicao transicao = mt->transicoes[estado_atual][indice_transicao];  // pega a transicao valida

        // debug: imprime o estado atual, simbolo lido e proxima transicao
        printf("Estado atual: %d, Simbolo: %c, Proximo estado: %d, Mover: %c\n", estado_atual, simbolo, transicao.estado_chegada, transicao.mover);

        // atualiza a fita com o simbolo que a transicao define
        fita[posicao] = transicao.simbolo_gravar;

        // move a cabeca de leitura da fita para direita ou esquerda
        if (transicao.mover == 'D') {
            posicao++;  // move para a direita
        } else if (transicao.mover == 'E') {
            posicao--;  // move para a esquerda
        }

        // atualiza o estado atual da maquina para o estado definido pela transicao
        estado_atual = transicao.estado_chegada;

        // se a cabeca de leitura sair dos limites da fita, a palavra sera rejeitada
        if (posicao < 0 || posicao >= MAX_FITAS) {
            estado_aceitacao = false;  // marca como rejeitada
            break;
        }
    }

    // verifica se a palavra foi aceita ou nao e retorna o resultado junto com a palavra original
    char* resultado = estado_aceitacao ? "Accept" : "Not Accept";
    static char resposta[MAX_FITAS + 20];  // buffer para armazenar a resposta
    snprintf(resposta, sizeof(resposta), "%s %s", palavra, resultado);  // formata a string com a resposta
    free(palavra);  // libera a memoria da palavra duplicada
    return resposta;  // retorna a resposta final
}

int main() {
    char alfabeto[MAX_SIMBOLOS];  // armazena o alfabeto da maquina
    MaquinaTuring mt;  // estrutura da maquina de turing
    char fitas[MAX_FITAS][MAX_FITAS];  // matriz para armazenar as fitas de entrada
    int qtd_fitas;  // quantidade de fitas de entrada

    // define o nome do arquivo de entrada como "input.txt"
    char* nome_arquivo = "input.txt";

    // abre o arquivo de entrada
    FILE* arquivo = fopen(nome_arquivo, "r");
    if (!arquivo) {
        printf("Nao foi possivel abrir o arquivo %s.\n", nome_arquivo);  // se nao conseguir abrir, exibe mensagem de erro
        return 1;  // retorna com erro
    }

    // leitura dos dados do arquivo de entrada
    fscanf(arquivo, "%s", alfabeto);  // le o alfabeto
    fscanf(arquivo, "%d", &mt.qtd_estados);  // le a quantidade de estados
    fscanf(arquivo, "%d", &mt.qtd_transicoes);  // le a quantidade de transicoes

    // le as transicoes da maquina e armazena na estrutura
    for (int i = 0; i < mt.qtd_transicoes; i++) {
        int estado_atual, estado_chegada;
        char simbolo_atual, simbolo_gravar, mover;

        // le cada transicao do arquivo
        fscanf(arquivo, "%d %c %c %c %d", &estado_atual, &simbolo_atual, &simbolo_gravar, &mover, &estado_chegada);

        // cria uma nova transicao e a armazena na matriz de transicoes
        Transicao t = {simbolo_atual, simbolo_gravar, mover, estado_chegada};
        mt.transicoes[estado_atual][i] = t;
    }

    // define o estado de aceitacao como o ultimo estado
    mt.estado_aceitacao = mt.qtd_estados;

    // le a quantidade de fitas de entrada
    fscanf(arquivo, "%d", &qtd_fitas);

    // le as fitas de entrada
    for (int i = 0; i < qtd_fitas; i++) {
        fscanf(arquivo, "%s", fitas[i]);  // armazena cada fita na matriz
    }

    // fecha o arquivo de entrada
    fclose(arquivo);

    // processa cada fita de entrada usando a maquina de turing
    printf("\n *********MAQUINA DE TURING********\n");
    printf("\nSaida:\n");
    for (int i = 0; i < qtd_fitas; i++) {
        printf("%d: %s\n", i + 1, processar_palavra(&mt, fitas[i]));  // exibe o resultado de cada fita
    }

    return 0;  // finaliza o programa
}