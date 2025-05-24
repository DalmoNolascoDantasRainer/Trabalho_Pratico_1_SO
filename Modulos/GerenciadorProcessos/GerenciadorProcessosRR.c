#include "GerenciadorProcessosRR.h"

// Função que inicializa o gerenciador de processos
GerenciadorProcessosRR *inicializaGerenciadorRR(int numCPUs){

    // Aloca memoria para o gerenciador de processos
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

    // Aloca memoria para o array de ponteiros das CPUs
    gerenciador->cpus = (CPU_RR **)malloc(numCPUs * sizeof(CPU_RR *));
    if (gerenciador->cpus == NULL)
    {
        printf("Erro: Falha na alocacao de memoria para array de CPUs\n");
        free(gerenciador);
        exit(1);
    }

    // Aloca memoria para o array de estados de execucao
    gerenciador->estadoExecucao = (int *)malloc(numCPUs * sizeof(int));
    if (gerenciador->estadoExecucao == NULL)
    {
        printf("Erro: Falha na alocacao de memoria para estados de execucao\n");
        free(gerenciador->cpus);
        free(gerenciador);
        exit(1);
    }

    // Aloca cada CPU individualmente e depois inicializa
    for (int i = 0; i < numCPUs; i++)
    {
        // Primeiro aloca memoria para a CPU individual
        gerenciador->cpus[i] = (CPU_RR *)malloc(sizeof(CPU_RR));
        if (gerenciador->cpus[i] == NULL)
        {
            printf("Erro: Falha na alocacao de memoria para CPU %d\n", i);

            // Libera CPUs já alocadas em caso de erro
            for (int j = 0; j < i; j++)
            {
                free(gerenciador->cpus[j]);
            }
            free(gerenciador->cpus);
            free(gerenciador->estadoExecucao);
            free(gerenciador);
            exit(1);
        }

        // Agora inicializa a CPU já alocada
        inicializaCPURR(&gerenciador->cpus[i]);
        
        // Define o estado como vazio
        gerenciador->estadoExecucao[i] = NUMEROVAZIO;
    }
    
    // Cria a tabela de processos
    gerenciador->tabelaProcessos = criaLista();
    if (gerenciador->tabelaProcessos == NULL)
    {
        printf("Erro: Falha na criacao da tabela de processos\n");

        // Libera recursos já alocados
        for (int i = 0; i < numCPUs; i++)
        {
            free(gerenciador->cpus[i]);
        }
        free(gerenciador->cpus);
        free(gerenciador->estadoExecucao);
        free(gerenciador);
        exit(1);
    }

    // Inicializa a fila Round Robin
    gerenciador->filaRoundRobin = criaFila();
    if (gerenciador->filaRoundRobin == NULL)
    {
        printf("Erro: Falha na criacao da fila Round Robin\n");

        // Libera recursos já alocados
        for (int i = 0; i < numCPUs; i++)
        {
            free(gerenciador->cpus[i]);
        }
        free(gerenciador->cpus);
        free(gerenciador->estadoExecucao);
        free(gerenciador);
        exit(1);
    }

    // Inicializa a fila de processos bloqueados
    gerenciador->estadoBloqueado = criaFila();
    if (gerenciador->estadoBloqueado == NULL)
    {
        printf("Erro: Falha na criacao da fila de bloqueados\n");

        // Libera recursos já alocados
        for (int i = 0; i < numCPUs; i++)
        {
            free(gerenciador->cpus[i]);
        }
        free(gerenciador->cpus);
        free(gerenciador->estadoExecucao);
        free(gerenciador);
        exit(1);
    }

    printf("Gerenciador Round Robin inicializado com sucesso: %d CPUs\n", numCPUs);
    return gerenciador;
}

// Função que inicia o processo inicial (init)
void iniciaProcessoInitRR(GerenciadorProcessosRR *gerenciador)
{
    // Cria o processo inicial
    ProcessoSimulado *processoInit = criaProcessoInit(gerenciador->tempo);

    // Enfileira na fila Round Robin
    printf("Iniciando processo init e enfileirando na fila Round Robin\n");
    enfileirar(processoInit->pid, NUMEROVAZIO, gerenciador->filaRoundRobin);
    printf("Processo init enfileirado na fila Round Robin\n");

    // Insere o processo inicial na tabela de processos
    insereNaTabela(processoInit, gerenciador->tabelaProcessos);
    printf("Processo init inserido na tabela de processos\n");

    // Incrementa o contador de processos iniciados
    gerenciador->quantidadeProcessosIniciados += 1;
    printf("Quantidade de processos iniciados: %d\n", gerenciador->quantidadeProcessosIniciados);
}

// Incrementa o tempo do sistema
void encerraUnidadeTempoRR(GerenciadorProcessosRR *gerenciador)
{
    gerenciador->tempo += 1;
}

// Escalona um processo da fila Round Robin para uma CPU
void escalonaProcessoRR(Lista *tabelaProcessos, CPU_RR *cpu, int *estadoExecucao, Fila *filaRR)
{

    printf("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n");
    imprimeFila(filaRR);

    printf("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n");
    PidTempo *pidTempo = desenfileirar(filaRR); // Obtem o proximo processo da fila Round Robin
    
    if (pidTempo != NULL)
    {
        printf("Escalonando processo PID %d na CPU\n", pidTempo->pid);
        int pidProcesso = pidTempo->pid;
        *estadoExecucao = pidProcesso; // Atualiza o estado de execucao da CPU

        ProcessoSimulado *proximoProcesso = buscaProcesso(tabelaProcessos, pidProcesso); // Busca o processo na tabela

        if (proximoProcesso != NULL)
        {
            proximoProcesso->estadoProcesso = EXECUCAO; // Define o estado do processo como em execucao
            printf("Processo PID %d entrou em execucao\n", pidProcesso);

            insereProcessoCPURR(cpu, proximoProcesso); // Carrega o processo na CPU
            printf("Processo PID %d inserido na CPU\n", pidProcesso);
        }

        // Libera a memoria do PidTempo
        free(pidTempo);
    }
}

// Escalona processos para as CPUs disponiveis
void escalonaProcessosCPUsRR(GerenciadorProcessosRR *gerenciador)
{
    verificaBloqueadosRR(gerenciador); // Verifica e desbloqueia processos, se necessario

    for (int i = 0; i < gerenciador->numCPUs; i++)
    {
        if (cpuLivreRR(gerenciador->cpus[i]) == 1) // Verifica se a CPU esta livre
        {
            if (gerenciador->filaRoundRobin->Tamanho > 0)
            {
                printf("CPU %d livre, escalonando processo da fila Round Robin\n", i);
                escalonaProcessoRR(gerenciador->tabelaProcessos, gerenciador->cpus[i],
                                   &gerenciador->estadoExecucao[i], gerenciador->filaRoundRobin);
            }
        }
    }
}

// Executa os processos carregados nas CPUs
void executaCPUsRR(GerenciadorProcessosRR *gerenciador)
{
    for (int i = 0; i < gerenciador->numCPUs; i++)
    {
        if (!(cpuLivreRR(gerenciador->cpus[i]))) // Verifica se a CPU esta ocupada
        {
            printf("Executando instrucao na CPU %d no tempo %d\n", i, gerenciador->tempo);
            executaProxInstrucaoCPURR(gerenciador->cpus[i], gerenciador->tempo, gerenciador->tabelaProcessos,
                                &gerenciador->quantidadeProcessosIniciados, NULL, gerenciador->estadoBloqueado);
        }
    }
}

// Realiza a troca de contexto nas CPUs (Round Robin)
void trocaDeContextoRR(GerenciadorProcessosRR *gerenciador)
{
    for (int i = 0; i < gerenciador->numCPUs; i++)
    {
        // Verifica se a CPU esta ocupada
        if (!(cpuLivreRR(gerenciador->cpus[i])))
        {
            printf("Verificando troca de contexto na CPU %d\n", i);
            removeProcessoCPURR(gerenciador->cpus[i], gerenciador->tabelaProcessos, gerenciador->filaRoundRobin);
        }
    }
}

// Remove um processo da CPU e o coloca na fila Round Robin
void removeProcessoCPURR(CPU_RR *cpu, Lista *tabelaProcessos, Fila *filaRR)
{
    ProcessoSimulado *processoNaCPU = buscaProcesso(tabelaProcessos, cpu->pidProcessoAtual);

    if (processoNaCPU != NULL)
    {
        const int QUANTUM_RR = 3; // Quantum fixo para Round Robin

        if (cpu->fatiaQuantum >= QUANTUM_RR) // Verifica se o quantum foi excedido
        {
            printf("Quantum excedido para processo PID %d, fazendo troca de contexto\n", processoNaCPU->pid);
            
            *(processoNaCPU->pc) = cpu->pcProcessoAtual; // Atualiza o PC do processo
            processoNaCPU->tempoCPU += cpu->fatiaQuantum; // Atualiza o tempo de CPU do processo
            
            // Verifica se o processo terminou
            if (*processoNaCPU->pc == NUMEROVAZIO)
            {
                printf("Processo PID %d terminou, removendo da tabela\n", processoNaCPU->pid);
                removeDaTabela(tabelaProcessos, processoNaCPU->pid);
                zeraCPURR(cpu);
                return;
            }

            processoNaCPU->estadoProcesso = PRONTO; // Define o estado como pronto

            // No Round Robin, reenfileira no final da fila sem alterar prioridade
            enfileirar(processoNaCPU->pid, NUMEROVAZIO, filaRR);
            printf("Processo PID %d reenfileirado na fila Round Robin\n", processoNaCPU->pid);

            zeraCPURR(cpu); // Libera a CPU
        }
        else if (processoNaCPU->estadoProcesso == BLOQUEADO) // Caso o processo esteja bloqueado
        {
            printf("Processo PID %d bloqueado, removendo da CPU\n", processoNaCPU->pid);
            processoNaCPU->tempoCPU += cpu->fatiaQuantum;
            zeraCPURR(cpu);

            if (*processoNaCPU->pc == NUMEROVAZIO) // Remove o processo se ele terminou
            {
                printf("Processo bloqueado PID %d terminou, removendo da tabela\n", processoNaCPU->pid);
                removeDaTabela(tabelaProcessos, processoNaCPU->pid);
            }
        }
    }
}

// Verifica e desbloqueia processos bloqueados, se necessario
void verificaBloqueadosRR(GerenciadorProcessosRR *gerenciador){
    int tamanhoOriginal = gerenciador->estadoBloqueado->Tamanho;
    for (int i = 0; i < tamanhoOriginal; i++){
        PidTempo *pidTempo = desenfileirar(gerenciador->estadoBloqueado);
        if (pidTempo != NULL){
            pidTempo->tempoExecutado--; // Decrementa o tempo de bloqueio
            if (pidTempo->tempoExecutado <= 0){
                printf("Processo PID %d desbloqueado, reenfileirando na fila Round Robin\n", pidTempo->pid);
                ProcessoSimulado *processo = buscaProcesso(gerenciador->tabelaProcessos, pidTempo->pid);
                if (processo != NULL){
                    processo->estadoProcesso = PRONTO;
                    enfileirar(pidTempo->pid, NUMEROVAZIO, gerenciador->filaRoundRobin);
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

// Função principal que gerencia os processos com base no comando recebido
void gerenciadorProcessosRR(GerenciadorProcessosRR *gerenciador, char comando) {
    if (comando == 'U')
    {
        encerraUnidadeTempoRR(gerenciador); // 1. Avança o tempo

        if (gerenciador->tempo == 1)
        {
            iniciaProcessoInitRR(gerenciador);
        }

        executaCPUsRR(gerenciador);           // 2. Executa quem já está na CPU
        trocaDeContextoRR(gerenciador);       // 3. Verifica se precisa trocar (Round Robin)
        escalonaProcessosCPUsRR(gerenciador); // 4. Envia novos processos para CPUs livres
    }
}

