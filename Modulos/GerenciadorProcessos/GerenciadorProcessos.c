#include "GerenciadorProcessos.h"
// Funcao que inicializa o gerenciador de processos
GerenciadorProcessos *inicializaGerenciador(int numCPUs) {
    // Aloca memoria para o gerenciador de processos
    GerenciadorProcessos *gerenciador = (GerenciadorProcessos *)malloc(sizeof(GerenciadorProcessos));
    if (gerenciador == NULL) {
        printf("Erro: Falha na alocacao de memoria para o gerenciador\n");
        exit(1);
    }
    
    // Inicializa os atributos do gerenciador
    gerenciador->tempo = 0;
    gerenciador->quantidadeProcessosIniciados = 0;
    gerenciador->tempoTotalExecucao = 0;
    gerenciador->numCPUs = numCPUs;
    
    // Aloca memoria para o array de ponteiros das CPUs
    gerenciador->cpus = (CPU **)malloc(numCPUs * sizeof(CPU *));
    if (gerenciador->cpus == NULL) {
        printf("Erro: Falha na alocacao de memoria para array de CPUs\n");
        free(gerenciador);
        exit(1);
    }
    
    // Aloca memoria para o array de estados de execucao
    gerenciador->estadoExecucao = (int *)malloc(numCPUs * sizeof(int));
    if (gerenciador->estadoExecucao == NULL) {
        printf("Erro: Falha na alocacao de memoria para estados de execucao\n");
        free(gerenciador->cpus);
        free(gerenciador);
        exit(1);
    }

    // CORREÇÃO: Aloca cada CPU individualmente e depois inicializa
    for (int i = 0; i < numCPUs; i++){
        // Primeiro aloca memoria para a CPU individual
        gerenciador->cpus[i] = (CPU *)malloc(sizeof(CPU));
        if (gerenciador->cpus[i] == NULL) {
            printf("Erro: Falha na alocacao de memoria para CPU %d\n", i);
            
            // Libera CPUs já alocadas em caso de erro
            for (int j = 0; j < i; j++) {
                free(gerenciador->cpus[j]);
            }
            free(gerenciador->cpus);
            free(gerenciador->estadoExecucao);
            free(gerenciador);
            exit(1);
        }
        
        // Agora inicializa a CPU já alocada
        inicializaCPU(&gerenciador->cpus[i]);
        
        // Define o estado como vazio
        gerenciador->estadoExecucao[i] = NUMEROVAZIO;
    }

    // Cria a tabela de processos
    gerenciador->tabelaProcessos = criaLista();
    if (gerenciador->tabelaProcessos == NULL) {
        printf("Erro: Falha na criacao da tabela de processos\n");
        
        // Libera recursos já alocados
        for (int i = 0; i < numCPUs; i++) {
            free(gerenciador->cpus[i]);
        }
        free(gerenciador->cpus);
        free(gerenciador->estadoExecucao);
        free(gerenciador);
        exit(1);
    }

    // Aloca memoria para as filas de processos prontos, uma para cada classe de prioridade
    gerenciador->estadoPronto = (Fila **)malloc(sizeof(Fila *) * CLASSESPRIORIDADES);
    if (gerenciador->estadoPronto == NULL) {
        printf("Erro: Falha na alocacao de memoria para filas de pronto\n");
        
        // Libera recursos já alocados
        for (int i = 0; i < numCPUs; i++) {
            free(gerenciador->cpus[i]);
        }
        free(gerenciador->cpus);
        free(gerenciador->estadoExecucao);
        free(gerenciador);
        exit(1);
    }

    // Inicializa as filas de processos prontos
    for (int i = 0; i < CLASSESPRIORIDADES; i++){
        gerenciador->estadoPronto[i] = criaFila();
        if (gerenciador->estadoPronto[i] == NULL) {
            printf("Erro: Falha na criacao da fila de pronto %d\n", i);
            
            // Libera filas já criadas
            for (int j = 0; j < i; j++) {
                // Assumindo que existe uma função para liberar fila
                // liberaFila(gerenciador->estadoPronto[j]);
            }
            
            // Libera outros recursos
            for (int j = 0; j < numCPUs; j++) {
                free(gerenciador->cpus[j]);
            }
            free(gerenciador->cpus);
            free(gerenciador->estadoExecucao);
            free(gerenciador->estadoPronto);
            free(gerenciador);
            exit(1);
        }
    }

    // Inicializa a fila de processos bloqueados
    gerenciador->estadoBloqueado = criaFila();
    if (gerenciador->estadoBloqueado == NULL) {
        printf("Erro: Falha na criacao da fila de bloqueados\n");
        
        // Libera recursos já alocados
        for (int i = 0; i < CLASSESPRIORIDADES; i++) {
            // liberaFila(gerenciador->estadoPronto[i]);
        }
        for (int i = 0; i < numCPUs; i++) {
            free(gerenciador->cpus[i]);
        }
        free(gerenciador->cpus);
        free(gerenciador->estadoExecucao);
        free(gerenciador->estadoPronto);
        free(gerenciador);
        exit(1);
    }

    printf("Gerenciador inicializado com sucesso: %d CPUs\n", numCPUs);
    return gerenciador;
}

// Funcao que inicia o processo inicial (init)
void iniciaProcessoInit(GerenciadorProcessos *gerenciador) {
    // Cria o processo inicial
    ProcessoSimulado *processoInit = criaProcessoInit(gerenciador->tempo);
    
    // Enfileira o processo inicial na fila de prioridade mais alta
    enfileirar(processoInit->pid, NUMEROVAZIO, gerenciador->estadoPronto[0]);

    // Insere o processo inicial na tabela de processos
    insereNaTabela(processoInit, gerenciador->tabelaProcessos);

    // Incrementa o contador de processos iniciados
    gerenciador->quantidadeProcessosIniciados += 1;
}

// Incrementa o tempo do sistema
void encerraUnidadeTempo(GerenciadorProcessos *gerenciador){
    gerenciador->tempo += 1;
}

// Escalona um processo especifico para uma CPU
void escalonaProcesso(Lista *tabelaProcessos, CPU *cpu, int *estadoExecucao, Fila **estadoPronto){
    int pidProcesso = desenfileirarFilas(estadoPronto, CLASSESPRIORIDADES); // Obtem o proximo processo da fila

    if (pidProcesso >= 0){
        *estadoExecucao = pidProcesso; // Atualiza o estado de execucao da CPU

        ProcessoSimulado *proximoProceso = buscaProcesso(tabelaProcessos, pidProcesso); // Busca o processo na tabela

        proximoProceso->estadoProcesso = EXECUCAO; // Define o estado do processo como em execucao

        insereProcessoCPU(cpu, proximoProceso); // Carrega o processo na CPU
    }
}


// Escalona processos para as CPUs disponiveis
void escalonaProcessosCPUs(GerenciadorProcessos *gerenciador){
    verificaBloqueados(gerenciador); // Verifica e desbloqueia processos, se necessario

    for (int i = 0; i < gerenciador->numCPUs; i++){
        if (cpuLivre(gerenciador->cpus[i]) == 1){ // Verifica se a CPU esta livre
            if (filasVazias(gerenciador->estadoPronto, CLASSESPRIORIDADES) == 0){ // Verifica se ha processos prontos
                escalonaProcesso(gerenciador->tabelaProcessos, gerenciador->cpus[i], gerenciador->estadoExecucao + i, gerenciador->estadoPronto);
            }
        }
    }
}

// Executa os processos carregados nas CPUs
void executaCPUs(GerenciadorProcessos *gerenciador){
    for (int i = 0; i < gerenciador->numCPUs; i++) {
        if (!(cpuLivre(gerenciador->cpus[i]))){ // Verifica se a CPU esta ocupada
            executaProxInstrucao(gerenciador->cpus[i], gerenciador->tempo, gerenciador->tabelaProcessos, &gerenciador->quantidadeProcessosIniciados, gerenciador->estadoPronto, gerenciador->estadoBloqueado);
        }
    }
}

// Realiza a troca de contexto nas CPUs
void trocaDeContexto(GerenciadorProcessos *gerenciador){
    for (int i = 0; i < gerenciador->numCPUs; i++){
        if (!(cpuLivre(gerenciador->cpus[i]))){ // Verifica se a CPU esta ocupado
            removeProcessoCPU(gerenciador->cpus[i], gerenciador->tabelaProcessos, gerenciador->estadoPronto);
        }
    }
}


// Funcao principal que gerencia os processos com base no comando recebido
void gerenciadorProcessos(GerenciadorProcessos *gerenciador, char comando){
    if (comando == 'U'){ // Comando para avancar uma unidade de tempo
        encerraUnidadeTempo(gerenciador); // Incrementa o tempo do sistema

        if (gerenciador->tempo == 1){ // Na primeira unidade de tempo
            iniciaProcessoInit(gerenciador); // Inicia o processo inicial
            escalonaProcessosCPUs(gerenciador); // Escalona os processos para as CPUs
            executaCPUs(gerenciador); // Executa os processos nas CPUs
            trocaDeContexto(gerenciador); // Realiza troca de contexto, se necessario
        }
        else{
            escalonaProcessosCPUs(gerenciador);
            executaCPUs(gerenciador);
            trocaDeContexto(gerenciador);
        }
    }
}

// Remove um processo da CPU e o coloca na fila apropriada
void removeProcessoCPU(CPU *cpu, Lista *tabelaProcessos, Fila **estadoPronto) {
    ProcessoSimulado *processoNaCPU = buscaProcesso(tabelaProcessos, (cpu->pidProcessoAtual)); // Busca o processo na CPU

    if (processoNaCPU != NULL){
        if (cpu->fatiaQuantum >= calcularPotencia(2, processoNaCPU->prioridade)){ // Verifica se o quantum foi excedido
            
            *(processoNaCPU->pc) = cpu->pcProcessoAtual; // Atualiza o PC do processo

            processoNaCPU->estadoProcesso = PRONTO; // Define o estado como pronto

            if (processoNaCPU->prioridade < CLASSESPRIORIDADES - 1) // Ajusta a prioridade, se necessario
            {
                processoNaCPU->prioridade++;
            }
            processoNaCPU->tempoCPU += cpu->fatiaQuantum; // Atualiza o tempo de CPU do processo

            enfileirar(processoNaCPU->pid, NUMEROVAZIO, estadoPronto[processoNaCPU->prioridade]); // Reenfileira o processo
            zeraCPU(cpu); // Libera a CPU
        }
        else if (processoNaCPU->estadoProcesso == BLOQUEADO) // Caso o processo esteja bloqueado
        {
            processoNaCPU->tempoCPU += cpu->fatiaQuantum;
            zeraCPU(cpu);

            if (*processoNaCPU->pc == NUMEROVAZIO) // Remove o processo se ele terminou
            {
                removeDaTabela(tabelaProcessos, processoNaCPU->pid);
            }
        }
    }
}

// Verifica e desbloqueia processos bloqueados, se necessario
void verificaBloqueados(GerenciadorProcessos *gerenciador)
{
    for (int i = 0; i < gerenciador->estadoBloqueado->Tamanho; i++)
    {
        PidTempo *pidTempo = desenfileirar(gerenciador->estadoBloqueado); // Obtem o proximo processo bloqueado

        pidTempo->tempoExecutado--; // Decrementa o tempo de bloqueio

        if (pidTempo->tempoExecutado <= 0) // Se o tempo de bloqueio acabou
        {
            ProcessoSimulado *processo = buscaProcesso(gerenciador->tabelaProcessos, pidTempo->pid);
            enfileirar(pidTempo->pid, NUMEROVAZIO, gerenciador->estadoPronto[processo->prioridade]); // Reenfileira o processo como pronto
        }
        else
        {
            enfileirar(pidTempo->pid, pidTempo->tempoExecutado, gerenciador->estadoBloqueado); // Reenfileira o processo bloqueado
        }
    }
}

// Remove um processo da tabela de processos
void removeProcessoTabela(ProcessoSimulado *processoEscolhido, GerenciadorProcessos *gerenciador){
    gerenciador->tempoTotalExecucao += processoEscolhido->tempoCPU; // Atualiza o tempo total de execucao
    removeDaTabela(gerenciador->tabelaProcessos, processoEscolhido->pid); // Remove o processo da tabela
}

// Calcula a potencia de um numero
double calcularPotencia(double base, int expoente){
    double resultado = 1.0;

    for (int i = 0; i < expoente; i++){
        resultado = resultado * base;
    }

    return resultado;
}