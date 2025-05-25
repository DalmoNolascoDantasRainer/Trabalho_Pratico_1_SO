#include "Fila.h"

// Funcao que cria uma fila vazia
Fila* criaFila(){
    Fila* fila = (Fila*) malloc(sizeof(Fila)); 
    
    fila->Frente = (Apontador) malloc(sizeof(ElementoPidTempo));
    fila->Tras = fila->Frente; // No inicio frente e tras estao na mesma celula (para evitar erros de memoria depois)
    fila->Frente->Prox = NULL;
    fila->Tamanho = 0;

    return fila;
} 

// Funcao que verifica se uma fila e vazia
int filaEhVazia(Fila* Fila) {   
    return (Fila->Tamanho == 0); 
}

//Retorna 1 se todas as filas sao vazias
int filasVazias(Fila** filas, int numFilas) {
    for (int i = 0; i < numFilas; i++){
        if (!filaEhVazia(filas[i])){
            return 0;
        }
    }
    
    return 1;
}

// Funcao que adiciona um elemento em uma fila vazia
void enfileirar(int pid, int tempoExecutado, Fila *Fila) {
    if(Fila->Tamanho == 0) {
        Fila->Frente = (Apontador) malloc(sizeof(ElementoPidTempo)); 
        Fila->Tras = Fila->Frente;  // Define que frente e tras apontam para o mesmo elemento (unico elemento)
        Fila->Tras->Prox = NULL;
        Fila->Frente->pidTempo = criaCelulaPidTempo(pid, tempoExecutado);
    }
    else{
        Fila->Tras->Prox = (Apontador) malloc(sizeof(ElementoPidTempo)); // Liga a celula na fila
        Fila->Tras = Fila->Tras->Prox; // Nova "ultima" celula
        Fila->Tras->Prox = NULL;
        Fila->Tras->pidTempo = criaCelulaPidTempo(pid, tempoExecutado);
    }
    Fila->Tamanho++;
}

// Funcao que remove um elemento em uma fila 
PidStatus* desenfileirar(Fila* fila) {
    if (filaEhVazia(fila)) {
        return NULL;
    }

    // Guarda o elemento que sera removido da fila
    PidStatus* pidTempoRemovido = (PidStatus*) malloc(sizeof(PidStatus));
    
    pidTempoRemovido->pid = fila->Frente->pidTempo.pid;
    pidTempoRemovido->tempoExecutado = fila->Frente->pidTempo.tempoExecutado;
    ElementoPidTempo* celulaRemovida = fila->Frente;

    fila->Frente = fila->Frente->Prox;
    free(celulaRemovida);

    // Para evitar possiveis erros 
    if (fila->Frente == NULL) {
        fila->Tras = NULL;
    }

    fila->Tamanho--;

    return pidTempoRemovido;
}

// Funcao que remove um elemento em uma fila e retorna o PID 
int desenfileirarPID(Fila* fila) {
    if (filaEhVazia(fila)) {
        return -1;
    }

    int PidRemovido = fila->Frente->pidTempo.pid;
    ElementoPidTempo* celulaRemovida = fila->Frente;

    fila->Frente = fila->Frente->Prox;
    free(celulaRemovida);

    if (fila->Frente == NULL) {
        fila->Tras = NULL;
    }

    fila->Tamanho--;

    return PidRemovido;
}


// Funcao que retorna o pid da primeira fila nao vazia
int desenfileirarFilas(Fila** filas, int numFilas) {
    int pidProcessoRemovido = -1;
    int i = 0;

    while (pidProcessoRemovido == -1 && i < numFilas) {
        pidProcessoRemovido = desenfileirarPID(filas[i]);
        if (pidProcessoRemovido >= 0){ // Nao e vazia
            return pidProcessoRemovido;
        }else{
            i++;
        }
    }

    return pidProcessoRemovido;
}

// Funcao que criar uma celula pid tempo
PidStatus criaCelulaPidTempo(int PID, int tempoExecutado){
    PidStatus celula;
    celula.pid = PID;
    celula.tempoExecutado = tempoExecutado;
    return celula;
}

// Funcao que realiza a impressao da fila
void imprimeFila(Fila *fila) {
    if (filaEhVazia(fila)){
        printf("   Fila Vazia!\n");

    } else{
        ElementoPidTempo *celula = fila->Frente;
        while (celula != NULL) {
            if (celula->pidTempo.tempoExecutado == -1){
                printf("\n   Pid: %d", celula->pidTempo.pid);
            } else{
                printf("\n   Pid: %d, Tempo bloqueado: %d", celula->pidTempo.pid, celula->pidTempo.tempoExecutado);
            }
            celula = celula->Prox;
        }

        putchar('\n');
    }
}

void imprimeFilas(Fila** filas, int numFilas) {
    for (int i = 0; i < numFilas; i++){
        Fila* fila = filas[i];

        printf("\nFila nº %d:", i);
        imprimeFila(fila);
    }
}