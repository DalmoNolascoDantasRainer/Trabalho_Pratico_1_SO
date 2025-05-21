#ifndef LISTA_H
#define LISTA_H

#include <stdio.h>
#include <stdlib.h>
#include "../ProcessoSimulado/ProcessoSimulado.h"

typedef struct celulaProcessoSimulado Celula;
struct celulaProcessoSimulado {
    ProcessoSimulado* processo;
    Celula* proximo;
};

typedef struct tipolista {
    Celula* inicio;
    Celula* fim;
} Lista;

Lista* criaLista();
int listaVazia(Lista* lista);
void insereTabela(Lista* lista, ProcessoSimulado* processo);
void removeTabela(Lista* lista, int PID);
ProcessoSimulado* buscaProcesso(Lista* lista, int PID);
int maiorPIDTabela(Lista* lista);
void imprimeTabela(Lista* lista);

#endif 