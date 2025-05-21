#include "Fila.h" 

// Funcao para criar e inicializar uma nova fila
Fila* criaFila(){

    // Aloca memoria para a estrutura da fila
    Fila* fila = (Fila*) malloc(sizeof(Fila));
    fila->Frente = (Apontador) malloc(sizeof(CelulaPidTempo)); // Celula cabeca

    
    fila->Tras = fila->Frente; // Inicializa o ponteiro Tras apontando para a mesma celula que Frente, as duas apontam para a celula cabeca
    
    fila->Frente->Prox = NULL;  // Define que a celula inicial n possui proximo elemento, pq a fila ta vazia

    fila->Tamanho = 0; 
    return fila; // Retorna o ponteiro para a nova fila criada
} 

// Funcao para verificar se uma fila esta vazia
int filaEhVazia(Fila* Fila){   
    return (Fila->Frente == Fila->Tras); // Retorna 1 se o tamanho da fila for 0 (fila esta vazia), caso contrario retorna 0
}

// Funcao para verificar se todas as filas de um conjunto estao vazias
int filasVazias(Fila** filas, int numFilas){
    for (int i = 0; i < numFilas; i++){
        if (!filaEhVazia(filas[i])){
            return 0; // Se encontrar uma fila nao vazia, retorna 0
        }
    }
    return 1;  // Se todas as filas estiverem vazias, retorna 1
}


// Funcao para adicionar um elemento na fila
void enfileirar(int pid, int tempoExecutado, Fila *Fila){
    Fila->Tras->Prox = (Apontador) malloc(sizeof(CelulaPidTempo)); // Liga a nova celula na fila
    Fila->Tras = Fila->Tras->Prox; // Atualiza o ponteiro Tras para a nova celula, ela passa a ser a ultima 
    Fila->Tras->Prox = NULL; // A ultima celula n possui proximo elemento
    Fila->Tras->pidTempo = criaCelulaPidTempo(pid, tempoExecutado);
    
    Fila->Tamanho++;
}

// Funcao para remover o primeiro elemento da fila e retornar seus dados
PidTempo* desenfileirar(Fila* fila) {
    if (filaEhVazia(fila)) {
        return NULL;
    }
    
    // Guarda o ponteiro para a celula cabeca que sera removida
    CelulaPidTempo* celulaRemovida = fila->Frente;
    
    // Avanca o ponteiro Frente para a proxima celula, ou seja, a celula real 
    fila->Frente = fila->Frente->Prox;
    
    PidTempo* pidTempoRemovido = (PidTempo*) malloc(sizeof(PidTempo));
    
    // Copia os dados do item que agora esta na frente (oq sera removido, pq sera a nova celula cabeca)
    pidTempoRemovido->pid = fila->Frente->pidTempo.pid;
    pidTempoRemovido->tempoExecutado = fila->Frente->pidTempo.tempoExecutado;
    

    // Se a celula removida era a ultima, atualiza o ponteiro Tras /////TENTAR TIRAR ISSO DEPOIS
    if (fila->Frente->Prox == NULL) {
        fila->Tras = fila->Frente;
    }

    // Libera a antiga celula cabeca
    free(celulaRemovida);
    
    fila->Tamanho--;
    return pidTempoRemovido;
}


// Funcao para remover o primeiro elemento da fila e retornar apenas o PID.
int desenfileirarPID(Fila* fila){
    if (filaEhVazia(fila)){
        return -1;
    }
    int processoRemovido = fila->Frente->pidTempo.pid; // Armazena o PID do elemento da frente da fila

    CelulaPidTempo* celulaRemovida = fila->Frente; 
    fila->Frente = fila->Frente->Prox; // Avanca o ponteiro Frente para a proxima celula, ou seja, a celula real 
    free(celulaRemovida);


    // Se a celula removida era a ultima, atualiza o ponteiro Tras
    if (fila->Frente->Prox == NULL) {
        fila->Tras = fila->Frente; /// ANTES ERA NULL
    }

    fila->Tamanho--;
    return processoRemovido;
}

// Funcao para remover o primeiro elemento da primeira fila nao vazia de um conjunto de filas
int desenfileirarFilas(Fila** filas, int numFilas){
    int pidProcessoRemovido = -1, i = 0;

    while (pidProcessoRemovido == -1 && i < numFilas){ // Itera pelas filas ate encontrar uma vazia
        pidProcessoRemovido = desenfileirarPID(filas[i]); // Tenta remover o elemento da fila atual

        if (pidProcessoRemovido >= 0){ // Se encontrou um elemento, retorna o PID
            return pidProcessoRemovido;
        } else{
            i++;
        }
    }

    return pidProcessoRemovido;
}

// Funcao para criar uma celula com os dados de PID e tempo executado
PidTempo criaCelulaPidTempo(int PID, int tempoExecutado){
    PidTempo celula;
    celula.pid = PID;
    celula.tempoExecutado = tempoExecutado;

    return celula;
}

// Funcao para imprimir os elementos de uma fila
void imprimeFila(Fila *fila){
    if (filaEhVazia(fila)){
        printf("\n  Fila esta Vazia :( \n");

    } else {
        CelulaPidTempo *celula = fila->Frente; // Ponteiro para iterar pelos elementos da fila

        while (celula != NULL) {
            if (celula->pidTempo.tempoExecutado == -1){ // Se o tempo executado for -1, imprime apenas o PID
                printf("\n   Pid: %d", celula->pidTempo.pid);
            } else{
                printf("\n   Pid: %d, Tempo bloqueado: %d", celula->pidTempo.pid, celula->pidTempo.tempoExecutado);
            }
            celula = celula->Prox;
        }
        putchar('\n');
    }
}

// Funcao para imprimir os elementos de um conjunto de filas
void imprimeFilas(Fila** filas, int numFilas){
    for (int i = 0; i < numFilas; i++){
        Fila* fila = filas[i];

        printf("\nFila numero: %d:", i);

        imprimeFila(filas[i]); // Chama a funcao de imprimir fila para cada fila do conjunto
    }
}