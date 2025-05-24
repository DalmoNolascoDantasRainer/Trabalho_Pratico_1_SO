#include "Fila.h"

Fila* criaFila()
{
    Fila* fila = (Fila*) malloc(sizeof(Fila));
    
    fila->Frente = (Apontador) malloc(sizeof(CelulaPidTempo));
    fila->Tras = fila->Frente;
    fila->Frente->Prox = NULL;
    fila->Tamanho = 0;

    return fila;
} 

int filaEhVazia(Fila* Fila)
{   
    return (Fila->Tamanho == 0);
}

//Retorna 1 se todas as filas são vazias
int filasVazias(Fila** filas, int numFilas)
{
    for (int i = 0; i < numFilas; i++)
    {
        if (!filaEhVazia(filas[i]))
        {
            return 0;
        }
    }
    
    return 1;
}

void enfileirar(int pid, int tempoExecutado, Fila *Fila)
{
    if(Fila->Tamanho == 0)
    {
        Fila->Frente = (Apontador) malloc(sizeof(CelulaPidTempo)); 
        Fila->Tras = Fila->Frente; // Ajusta a atribuição de Fila->Tras para o primeiro elemento
        Fila->Tras->Prox = NULL;
        Fila->Frente->pidTempo = criaCelulaPidTempo(pid, tempoExecutado);
    }
    else
    {
        Fila->Tras->Prox = (Apontador) malloc(sizeof(CelulaPidTempo)); 
        Fila->Tras = Fila->Tras->Prox;
        Fila->Tras->Prox = NULL;
        Fila->Tras->pidTempo = criaCelulaPidTempo(pid, tempoExecutado);
    }
    Fila->Tamanho++;
}

PidTempo* desenfileirar(Fila* fila)
{
    if (filaEhVazia(fila)) {
        return NULL;
    }

    PidTempo* pidTempoRemovido = (PidTempo*) malloc(sizeof(PidTempo));

    pidTempoRemovido->pid = fila->Frente->pidTempo.pid;
    pidTempoRemovido->tempoExecutado = fila->Frente->pidTempo.tempoExecutado;
    CelulaPidTempo* celulaRemovida = fila->Frente;

    fila->Frente = fila->Frente->Prox;
    free(celulaRemovida);

    if (fila->Frente == NULL) {
        fila->Tras = NULL;
    }

    fila->Tamanho--;

    return pidTempoRemovido;
}

int desenfileirarPID(Fila* fila)
{
    if (filaEhVazia(fila)) {
        return -1;
    }

    int processoRemovido = fila->Frente->pidTempo.pid;
    CelulaPidTempo* celulaRemovida = fila->Frente;

    fila->Frente = fila->Frente->Prox;
    free(celulaRemovida);

    if (fila->Frente == NULL) {
        fila->Tras = NULL;
    }

    fila->Tamanho--;

    return processoRemovido;
}

int desenfileirarFilas(Fila** filas, int numFilas)
{
    int pidProcessoRemovido = -1, i = 0;

    while (pidProcessoRemovido == -1 && i < numFilas)
    {
        pidProcessoRemovido = desenfileirarPID(filas[i]);
        if (pidProcessoRemovido >= 0)
        {
            return pidProcessoRemovido;
        }else
        {
            i++;
        }
    }

    return pidProcessoRemovido;
}

PidTempo criaCelulaPidTempo(int PID, int tempoExecutado)
{
    PidTempo celula;
    celula.pid = PID;
    celula.tempoExecutado = tempoExecutado;
    return celula;
}

void imprimeFila(Fila *fila)
{
    if (filaEhVazia(fila))
    {
        printf("   Fila Vazia!\n");

    } else
    {
        CelulaPidTempo *celula = fila->Frente;
        while (celula != NULL) {
            if (celula->pidTempo.tempoExecutado == -1)
            {
                printf("\n   Pid: %d", celula->pidTempo.pid);
            }else
            {
                printf("\n   Pid: %d, Tempo bloqueado: %d", celula->pidTempo.pid, celula->pidTempo.tempoExecutado);
            }
            celula = celula->Prox;
        }

        putchar('\n');
    }
}

void imprimeFilas(Fila** filas, int numFilas)
{
    for (int i = 0; i < numFilas; i++)
    {
        Fila* fila = filas[i];

        printf("\nFila nº %d:", i);

        if (filaEhVazia(fila))
        {
            printf("\n   Fila Vazia!\n");

        } else
        {
            CelulaPidTempo *celula = fila->Frente;
            while (celula != NULL) {
                if (celula->pidTempo.tempoExecutado == -1)
                {
                    printf("\n   Pid: %d", celula->pidTempo.pid);
                }else
                {
                    printf("\n   Pid: %d, Tempo bloqueado: %d", celula->pidTempo.pid, celula->pidTempo.tempoExecutado);
                }
                
                celula = celula->Prox;
            }
            putchar('\n');
        }
    }
}