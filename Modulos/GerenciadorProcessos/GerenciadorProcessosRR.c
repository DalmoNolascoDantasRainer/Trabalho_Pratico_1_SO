#include "GerenciadorProcessosRR.h"

// Funcao que inicializa o gerenciador de processos
GerenciadorProcessosRR *inicializaGerenciadorRR(int numCPUs){

    GerenciadorProcessosRR *gerenciador = (GerenciadorProcessosRR *)malloc(sizeof(GerenciadorProcessosRR));
    if (gerenciador == NULL){
        printf("Erro: Falha na alocacao de memoria para o gerenciador\n");
        exit(1);
    }

    // Inicializa os atributos do gerenciador
    gerenciador->tempo = 0;
    gerenciador->quantidadeProcessosIniciados = 0;
    gerenciador->tempoTotalExecucao = 0;
    gerenciador->numCPUs = numCPUs;

    // Aloca memoria para o vetor de ponteiros das CPUs
    gerenciador->cpus = (CPU_RR **)malloc(numCPUs * sizeof(CPU_RR *));
    if (gerenciador->cpus == NULL){
        printf("Erro: Falha na alocacao de memoria para vetor de CPUs\n");
        free(gerenciador);
        exit(1);
    }

    // Aloca memoria para o vetor de estados de execucao
    gerenciador->estadoExecucao = (int *)malloc(numCPUs * sizeof(int));
    if (gerenciador->estadoExecucao == NULL) {
        printf("Erro: Falha na alocacao de memoria para estados de execucao\n");
        free(gerenciador->cpus);
        free(gerenciador);
        exit(1);
    }

    // Aloca cada CPU individualmente e depois inicializa
    for (int i = 0; i < numCPUs; i++) {
        // Primeiro aloca memoria para a CPU individual
        gerenciador->cpus[i] = (CPU_RR *)malloc(sizeof(CPU_RR));
        if (gerenciador->cpus[i] == NULL)
        {
            printf("Erro: Falha na alocacao de memoria para CPU %d\n", i);

            // Libera CPUs ja alocadas em caso de erro
            for (int j = 0; j < i; j++){
                free(gerenciador->cpus[j]);
            }
            free(gerenciador->cpus);
            free(gerenciador->estadoExecucao);
            free(gerenciador);
            exit(1);
        }

        inicializaCPURR(&gerenciador->cpus[i]);
        
        gerenciador->estadoExecucao[i] = NUMEROVAZIO;
    }
    
    // Cria a tabela de processos
    gerenciador->tabelaProcessos = criaLista();
    if (gerenciador->tabelaProcessos == NULL){
        printf("Erro: Falha na criacao da tabela de processos\n");

        // Libera recursos ja alocados
        for (int i = 0; i < numCPUs; i++){
            free(gerenciador->cpus[i]);
        }
        free(gerenciador->cpus);
        free(gerenciador->estadoExecucao);
        free(gerenciador);
        exit(1);
    }

    // Inicializa a fila Round Robin
    gerenciador->filaRoundRobin = criaFila();
    if (gerenciador->filaRoundRobin == NULL){
        printf("Erro: Falha na criacao da fila Round Robin\n");

        // Libera recursos ja alocados
        for (int i = 0; i < numCPUs; i++){
            free(gerenciador->cpus[i]);
        }
        free(gerenciador->cpus);
        free(gerenciador->estadoExecucao);
        free(gerenciador);
        exit(1);
    }

    // Inicializa a fila de processos bloqueados
    gerenciador->estadoBloqueado = criaFila();
    if (gerenciador->estadoBloqueado == NULL) {
        printf("Erro: Falha na criacao da fila de bloqueados\n");

        // Libera recursos ja alocados
        for (int i = 0; i < numCPUs; i++){
            free(gerenciador->cpus[i]);
        }
        free(gerenciador->cpus);
        free(gerenciador->estadoExecucao);
        free(gerenciador);
        exit(1);
    }

    return gerenciador;
}

// Funcao que inicia o processo inicial (init)
void iniciaProcessoInitRR(GerenciadorProcessosRR *gerenciador){
    ProcessoSimulado *processoInit = criaProcessoInit(gerenciador->tempo);
    enfileirar(processoInit->pid, NUMEROVAZIO, gerenciador->filaRoundRobin); // Enfileira na fila Round Robin

    insereNaTabela(processoInit, gerenciador->tabelaProcessos);

    gerenciador->quantidadeProcessosIniciados += 1;
}

// Incrementa o tempo do sistema
void encerraUnidadeTempoRR(GerenciadorProcessosRR *gerenciador) {
    gerenciador->tempo += 1;
}

// Escalona um processo da fila Round Robin para uma CPU
void escalonaProcessoRR(Lista *tabelaProcessos, CPU_RR *cpu, int *estadoExecucao, Fila *filaRR){

    PidTempo *pidTempo = desenfileirar(filaRR); // Obtem o proximo processo da fila Round Robin
    
    if (pidTempo != NULL){
        int pidProcesso = pidTempo->pid;
        *estadoExecucao = pidProcesso; // Atualiza o estado de execucao da CPU

        ProcessoSimulado *proximoProcesso = buscaProcesso(tabelaProcessos, pidProcesso); // Busca o processo na tabela

        if (proximoProcesso != NULL) {
            proximoProcesso->estadoProcesso = EXECUCAO; // Define o estado do processo como em execucao

            insereProcessoCPURR(cpu, proximoProcesso); // Carrega o processo na CPU
        }

        free(pidTempo);
    }
}

// Escalona processos para as CPUs disponiveis
void escalonaProcessosCPUsRR(GerenciadorProcessosRR *gerenciador) {
    verificaBloqueadosRR(gerenciador); // Verifica e desbloqueia processos, se necessario

    for (int i = 0; i < gerenciador->numCPUs; i++){
        if (cpuLivreRR(gerenciador->cpus[i]) == 1){
            if (gerenciador->filaRoundRobin->Tamanho > 0){
                escalonaProcessoRR(gerenciador->tabelaProcessos, gerenciador->cpus[i],
                                   &gerenciador->estadoExecucao[i], gerenciador->filaRoundRobin);
            }
        }
    }
}

// Executa os processos carregados nas CPUs
void executaCPUsRR(GerenciadorProcessosRR *gerenciador) {
    for (int i = 0; i < gerenciador->numCPUs; i++) {
        if (!(cpuLivreRR(gerenciador->cpus[i]))){ // Verifica se a CPU esta ocupada
            executaProxInstrucaoCPURR(gerenciador->cpus[i], gerenciador->tempo, gerenciador->tabelaProcessos,
                                &gerenciador->quantidadeProcessosIniciados, gerenciador->filaRoundRobin, gerenciador->estadoBloqueado);
        }
    }
}

// Realiza a troca de contexto nas CPUs (Round Robin)
void trocaDeContextoRR(GerenciadorProcessosRR *gerenciador){
    for (int i = 0; i < gerenciador->numCPUs; i++){
        // Verifica se a CPU esta ocupada e remove, se necessario
        if (!(cpuLivreRR(gerenciador->cpus[i]))){
            removeProcessoCPURR(gerenciador->cpus[i], gerenciador->tabelaProcessos, gerenciador->filaRoundRobin);
        }
    }
}

// Remove um processo da CPU e o coloca na fila Round Robin
void removeProcessoCPURR(CPU_RR *cpu, Lista *tabelaProcessos, Fila *filaRR){
    ProcessoSimulado *processoNaCPU = buscaProcesso(tabelaProcessos, cpu->pidProcessoAtual);

    if (processoNaCPU != NULL) {
        const int QUANTUM_RR = 3; // Quantum fixo para Round Robin

        if (cpu->fatiaQuantum >= QUANTUM_RR){ // Verifica se o quantum foi excedido
            
            *(processoNaCPU->pc) = cpu->pcProcessoAtual; // Atualiza o PC do processo
            processoNaCPU->tempoCPU += cpu->fatiaQuantum; // Atualiza o tempo de CPU do processo
            
            // Verifica se o processo terminou
            if (*processoNaCPU->pc == NUMEROVAZIO){
                removeDaTabela(tabelaProcessos, processoNaCPU->pid);
                zeraCPURR(cpu);
                return;
            }

            processoNaCPU->estadoProcesso = PRONTO; // Define o estado como pronto

            // No Round Robin, reenfileira no final da fila
            enfileirar(processoNaCPU->pid, NUMEROVAZIO, filaRR);
            zeraCPURR(cpu); // Libera a CPU
        }
        else if (processoNaCPU->estadoProcesso == BLOQUEADO){
            processoNaCPU->tempoCPU += cpu->fatiaQuantum;
            zeraCPURR(cpu);

            if (*processoNaCPU->pc == NUMEROVAZIO){ // Remove o processo se ele terminou
                removeDaTabela(tabelaProcessos, processoNaCPU->pid);
            }
        }
    }
}

// Verifica e desbloqueia processos bloqueados, se necessario
void verificaBloqueadosRR(GerenciadorProcessosRR *gerenciador){
    int tamanhoOriginal = gerenciador->estadoBloqueado->Tamanho;

    // Processa todos os elementos que estavam na fila no inicio 
    for (int i = 0; i < tamanhoOriginal; i++){
        PidTempo *pidTempo = desenfileirar(gerenciador->estadoBloqueado);

        if (pidTempo != NULL){
            pidTempo->tempoExecutado--; 

            if (pidTempo->tempoExecutado <= 0){  // Processo deve ser desbloqueado
                ProcessoSimulado *processo = buscaProcesso(gerenciador->tabelaProcessos, pidTempo->pid);

                if (processo != NULL){
                    processo->estadoProcesso = PRONTO;
                    enfileirar(pidTempo->pid, NUMEROVAZIO, gerenciador->filaRoundRobin); // "Devolve" o proces
                }
                free(pidTempo);
            }
            else{

                // Reenfileira o processo bloqueado
                enfileirar(pidTempo->pid, pidTempo->tempoExecutado, gerenciador->estadoBloqueado);
            }
        }
    }
}

// Funcao principal que gerencia os processos com base no comando recebido
void gerenciadorProcessosRR(GerenciadorProcessosRR *gerenciador, char comando) {
    if (comando == 'U'){
        encerraUnidadeTempoRR(gerenciador); 

        if (gerenciador->tempo == 1) {
            iniciaProcessoInitRR(gerenciador);
        }

        executaCPUsRR(gerenciador);           // Executa quem ja esta na CPU
        trocaDeContextoRR(gerenciador);       // Verifica se precisa trocar (Round Robin)
        escalonaProcessosCPUsRR(gerenciador); // Envia novos processos para CPUs livres
    }
}

