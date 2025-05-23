#ifndef PROCESSO_SIMULADO_H
#define PROCESSO_SIMULADO_H

#include <stdlib.h>
#include "Instrucao.h"

// Enum para os estados do processo
typedef enum {
    BLOQUEADO,
    EXECUCAO,
    PRONTO
} Estado;

typedef struct ProcessoSimulado{
    int pid;                           // ID do proceso
    int pid_pai;                       // ID do proceso pai
    int* pc;                           // Posicao da proxima istrucao que sera lida no vetor de instrucoes)
    int* vetorVariaveis;               // Vetor que guarda as variaveis do processo
    int prioridade;                    // Prioridade do processo  (0 a 3, 0 e a menor prioridade, 3 e a maior)
    Estado estadoProcesso;             // Estado do processo podendo ser um dos 3 valores do enum
    int tempoInicio;                   // Tempo em que o processo começou
    int tempoCPU;                      // Tempo que o processo ja passou executando
    Instrucao** conjuntoInstrucoes;    // Vetor com as instrucoes do processo
} ProcessoSimulado;

ProcessoSimulado* criaProcessoInit(int tempoSistema); // Cria o processo inicial (init)
ProcessoSimulado* copiaProcesso(ProcessoSimulado processoPai, int tempoAtualSistema, int novoPid); // Cria uma copia de um processo pai (como se fosse um fork)
void copiaVariaveis(int* vetorVariaveisBase, int* vetorVariaveisNovo, int tamanho);
void copiaConjuntoInstrucoes(Instrucao** vetorNovo, Instrucao* vetorBase);
int numeroVariaveis(Instrucao* conjuntoInstrucoes); // Conta quantas variaveis sao necessarias com base nas instrucoes
void imprimeProcesso(ProcessoSimulado processo, int opcao);
void imprimeEstadoProcessoSimulado(Estado estadoProcesso);
void imprimeVariaveis(int* vetorVariaveis, int tamanho);


/// // Funcoes para acessar os atributos do processo atual  VE SE VAI DEIXAR /////
int pidProcessoAtual(); // Retorna o PID do processo atual
int pidProcessoPai(); // Retorna o PID do processo pai
int* pcProcessoAtual(); // Retorna o ponteiro para o PC do processo atual
int* vetorVariaveisProcessoAtual(); // Retorna o vetor de variaveis do processo atual
int prioridadeProcessoAtual(); // Retorna a prioridade do processo atual
Estado estadoProcessoAtual(); // Retorna o estado do processo atual
int tempoInicioProcessoAtual(); // Retorna o tempo de inicio do processo atual
int tempoCPUProcessoAtual(); // Retorna o tempo de CPU do processo atual
Instrucao** conjuntoInstrucoesProcessoAtual(); // Retorna o vetor de instrucoes do processo atual

#endif 