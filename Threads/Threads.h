#ifndef THREADS_H
#define THREADS_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include "../Modulos/ProcessoControle/ProcessoControle.h"
#include "../Modulos/ProcessoImpressao/ProcessoImpressao.h"
#include "../Modulos/ProcessoImpressao/ProcessoImpressaoRR.h"  // ADICIONADO: Include para Round Robin

typedef struct {
    char comando;                    // Comando a ser processado
    int comando_disponivel;          // Flag indicando se tem comando disponivel (0 = nao, 1 = sim)
    int comando_processado;          // Flag indicando se comando foi processado (0 = nao, 1 = sim)
    pthread_mutex_t mutex;           // Mutex para controle de acesso exclusivo ao buffer
    pthread_cond_t cond_comando;     // Variavel de condicao para sinalizar comando disponivel
    pthread_cond_t cond_processado;  // Variavel de condicao para sinalizar comando processado
} BufferComando;

typedef struct {
    GerenciadorProcessos *gerenciador;        // Gerenciador para Filas Múltiplas
    GerenciadorProcessosRR *gerenciadorRR;    // ADICIONADO: Gerenciador para Round Robin
    BufferComando *buffer;
    FILE *arquivo;
    int opcao;
    int opcaoEscalonamento;                   // ADICIONADO: Tipo de algoritmo (1=FM, 2=RR)
    int numCPUs;
    int sistema_ativo;
    sem_t sem_impressao;
    pthread_mutex_t mutex_gerenciador;
} DadosGlobais;

void* thread_controle(void* arg);
void* thread_gerenciador(void* arg);
void* thread_impressao(void* arg);

#endif