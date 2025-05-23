#ifndef LISTA_H
#define LISTA_H

#include <stdio.h>
#include <stdlib.h>
#include "../ProcessoSimulado/ProcessoSimulado.h"

typedef struct CelulaProcessoSimulado *CelulaPtr;
typedef struct CelulaProcessoSimulado {
    ProcessoSimulado* processo;
    CelulaPtr Prox;
} CelulaProcessoSimulado;

typedef struct Lista {
    CelulaPtr Primeiro, Ultimo;
} Lista;

Lista* criaLista();
int listaEhVazia(Lista* lista);

#endif 