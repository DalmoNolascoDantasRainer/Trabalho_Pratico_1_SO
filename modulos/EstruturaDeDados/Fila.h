#ifndef FILA_H
#define FILA_H

#include <stdio.h>
#include <stdlib.h>

//"Apontador" eh um ponteiro para uma estrutura do tipo "CelulaProcesso"
typedef struct CelulaPidTempo* Apontador;

typedef struct PidTempo {
    int pid; // "pid" (identificador de processo)
    int tempoExecutado; // "tempoExecutado" (tempo que o processo ja executou).
} PidTempo;

typedef struct CelulaPidTempo {
    PidTempo pidTempo;
    Apontador Prox;
} CelulaPidTempo;

typedef struct Fila {
    Apontador Frente, Tras;
    int Tamanho;
} Fila;

Fila* criaFila();

int filaEhVazia(Fila* Fila);
int filasVazias(Fila** filas, int numFilas); // Verifica se todas as filas em um conjunto de filas estao vazias
void enfileirar(int pid, int tempoExecutado, Fila *Fila);
PidTempo* desenfileirar(Fila* fila); // Remove e retorna o primeiro elemento da fila
int desenfileirarPID(Fila* fila); // Remove o primeiro elemento da fila de filas e retorna o Processo
int desenfileirarFilas(Fila** filas, int numFilas); // Esvazia todas as filas da fila
PidTempo criaCelulaPidTempo(int PID, int tempoExecutado);
void imprimeFila(Fila *fila);
void imprimeFilas(Fila** filas, int numFilas);

#endif 
