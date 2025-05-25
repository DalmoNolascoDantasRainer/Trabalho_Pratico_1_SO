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


typedef struct {
    char comando;
    int comando_disponivel;
    int comando_processado;
    pthread_mutex_t mutex;
    pthread_cond_t cond_comando;
    pthread_cond_t cond_processado;
} BufferComando;

typedef struct {
    GerenciadorProcessos *gerenciador;
    BufferComando *buffer;
    FILE *arquivo;
    int opcao;
    int numCPUs;
    int sistema_ativo;
    sem_t sem_impressao;
    pthread_mutex_t mutex_gerenciador;
} DadosGlobais;

void* thread_controle(void* arg);
void* thread_gerenciador(void* arg);
void* thread_impressao(void* arg);

#endif