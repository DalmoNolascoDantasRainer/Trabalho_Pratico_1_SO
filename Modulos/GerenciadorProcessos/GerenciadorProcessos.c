#include "GerenciadorProcessos.h"
// Funcao que inicializa o gerenciador de processos
GerenciadorProcessos *inicializaGerenciador(int numCPUs)
{
    // Aloca memoria para o gerenciador de processos
    GerenciadorProcessos *gerenciador = (GerenciadorProcessos *)malloc(sizeof(GerenciadorProcessos));
    if (gerenciador == NULL)
    {
        printf("Erro: Falha na alocacao de memoria para o gerenciador\n");
        exit(1);
    }

    // Inicializa os atributos do gerenciador
    gerenciador->tempo = 0;
    gerenciador->quantidadeProcessosIniciados = 0;
    gerenciador->tempoTotalExecucao = 0;
    gerenciador->numCPUs = numCPUs;

    gerenciador->tipoEscalonamento = ESC_FILAS_MULTIPLAS;

    // Aloca memoria para o array de ponteiros das CPUs
    gerenciador->cpus = (CPU **)malloc(numCPUs * sizeof(CPU *));
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

    // CORREÇÃO: Aloca cada CPU individualmente e depois inicializa
    for (int i = 0; i < numCPUs; i++)
    {
        // Primeiro aloca memoria para a CPU individual
        gerenciador->cpus[i] = (CPU *)malloc(sizeof(CPU));
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
        inicializaCPU(&gerenciador->cpus[i]);

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

    // Aloca memoria para as filas de processos prontos, uma para cada classe de prioridade
    gerenciador->estadoPronto = (Fila **)malloc(sizeof(Fila *) * CLASSESPRIORIDADES);
    if (gerenciador->estadoPronto == NULL)
    {
        printf("Erro: Falha na alocacao de memoria para filas de pronto\n");

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

    // Inicializa as filas de processos prontos
    for (int i = 0; i < CLASSESPRIORIDADES; i++)
    {
        gerenciador->estadoPronto[i] = criaFila();
        if (gerenciador->estadoPronto[i] == NULL)
        {
            printf("Erro: Falha na criacao da fila de pronto %d\n", i);

            // Libera filas já criadas
            for (int j = 0; j < i; j++)
            {
                // Assumindo que existe uma função para liberar fila
                // liberaFila(gerenciador->estadoPronto[j]);
            }

            // Libera outros recursos
            for (int j = 0; j < numCPUs; j++)
            {
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
    if (gerenciador->estadoBloqueado == NULL)
    {
        printf("Erro: Falha na criacao da fila de bloqueados\n");

        // Libera recursos já alocados
        for (int i = 0; i < CLASSESPRIORIDADES; i++)
        {
            // liberaFila(gerenciador->estadoPronto[i]);
        }
        for (int i = 0; i < numCPUs; i++)
        {
            free(gerenciador->cpus[i]);
        }
        free(gerenciador->cpus);
        free(gerenciador->estadoExecucao);
        free(gerenciador->estadoPronto);
        free(gerenciador);
        exit(1);
    }

    gerenciador->filaRoundRobin = criaFila();
    if (gerenciador->filaRoundRobin == NULL)
    {
        printf("Erro: Falha na criacao da fila Round Robin\n");

        // Libera recursos já alocados
        for (int i = 0; i < CLASSESPRIORIDADES; i++)
        {
            // liberaFila(gerenciador->estadoPronto[i]);
        }
        for (int i = 0; i < numCPUs; i++)
        {
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

// Funcao para definir o tipo de escalonamento
void defineEscalonamento(GerenciadorProcessos *gerenciador, TipoEscalonamento tipo){
    gerenciador->tipoEscalonamento = tipo;
    printf("Tipo de escalonamento definido: %d\n", tipo);
}

// Funcao que inicia o processo inicial (init)
void iniciaProcessoInit(GerenciadorProcessos *gerenciador){
    // Cria o processo inicial
    ProcessoSimulado *processoInit = criaProcessoInit(gerenciador->tempo);

    // Verifica o tipo de escalonamento para decidir onde enfileirar
    if (gerenciador->tipoEscalonamento == ESC_ROBIN){
        printf("Entrou no iniciaProcessoInit e vai enfileirar na fila Round Robin\n");
        // Para Round Robin, enfileira na fila única
        enfileirar(processoInit->pid, NUMEROVAZIO, gerenciador->filaRoundRobin);
        printf("Enfileirou na fila Round Robin\n");
    }
    else{
        // Para filas múltiplas, enfileira na fila de prioridade mais alta
        enfileirar(processoInit->pid, NUMEROVAZIO, gerenciador->estadoPronto[0]);
    }

    // Insere o processo inicial na tabela de processos
    insereNaTabela(processoInit, gerenciador->tabelaProcessos);
    printf("Inseriu o processo init na tabela de processos\n");
    //printf("gerenciador->filaRoundRobin %p\n", gerenciador->filaRoundRobin);

    // Incrementa o contador de processos iniciados
    gerenciador->quantidadeProcessosIniciados += 1;
    printf("Quantidade de processos iniciados: %d\n", gerenciador->quantidadeProcessosIniciados);
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
        ProcessoSimulado *proximoProcesso = buscaProcesso(tabelaProcessos, pidProcesso); // Busca o processo na tabela
        proximoProcesso->estadoProcesso = EXECUCAO; // Define o estado do processo como em execucao
        insereProcessoCPU(cpu, proximoProcesso); // Carrega o processo na CPU
    }
}

void escalonaProcessoRR(Lista *tabelaProcessos, CPU *cpu, int *estadoExecucao, Fila *filaRR){
    PidTempo *pidTempo = desenfileirar(filaRR); // Obtem o proximo processo da fila Round Robin
    
    if (pidTempo != NULL){
        printf("Entrou no EscalonaProcessoRR\n");
        int pidProcesso = pidTempo->pid;
        *estadoExecucao = pidProcesso; // Atualiza o estado de execucao da CPU

        ProcessoSimulado *proximoProcesso = buscaProcesso(tabelaProcessos, pidProcesso); // Busca o processo na tabela

        proximoProcesso->estadoProcesso = EXECUCAO; // Define o estado do processo como em execucao
        printf("Entrou em execucao\n");

        insereProcessoCPU(cpu, proximoProcesso); // Carrega o processo na CPU
        printf("Inseriu na CPU\n");

        // Se pidTempo foi alocado dinamicamente, libere aqui:
        free(pidTempo);
    }
}

// Escalona processos para as CPUs disponiveis
void escalonaProcessosCPUs(GerenciadorProcessos *gerenciador){
    verificaBloqueados(gerenciador); // Verifica e desbloqueia processos, se necessario

    for (int i = 0; i < gerenciador->numCPUs; i++){

        if (cpuLivre(gerenciador->cpus[i]) == 1)
        { // Verifica se a CPU esta livre
            if (gerenciador->tipoEscalonamento == ESC_ROBIN)
            {
                // Escalonamento Round Robin - usa uma única fila
                if (gerenciador->filaRoundRobin->Tamanho > 0)
                {
                    printf("Entrou no escalona processos CPUS e vai EscalonaProcessoRR \n");
                    escalonaProcessoRR(gerenciador->tabelaProcessos, gerenciador->cpus[i],
                                       gerenciador->estadoExecucao + i, gerenciador->filaRoundRobin);
                }
            }
            else {
                // Escalonamento com filas múltiplas (comportamento original)
                if (filasVazias(gerenciador->estadoPronto, CLASSESPRIORIDADES) == 0) {
                    escalonaProcesso(gerenciador->tabelaProcessos, gerenciador->cpus[i],
                                     gerenciador->estadoExecucao + i, gerenciador->estadoPronto);
                }
            }
        }
    }
}

// Executa os processos carregados nas CPUs
void executaCPUs(GerenciadorProcessos *gerenciador)
{
    for (int i = 0; i < gerenciador->numCPUs; i++)
    {
        if (!(cpuLivre(gerenciador->cpus[i])))
        { // Verifica se a CPU esta ocupada
            printf("Entrou no executaCPUS %d\n ", gerenciador->tempo);
            executaProxInstrucao(gerenciador->cpus[i], gerenciador->tempo, gerenciador->tabelaProcessos, &gerenciador->quantidadeProcessosIniciados, gerenciador->estadoPronto, gerenciador->estadoBloqueado);
        }
    }
}

// Realiza a troca de contexto nas CPUs
void trocaDeContexto(GerenciadorProcessos *gerenciador)
{
    for (int i = 0; i < gerenciador->numCPUs; i++)
    {
        // Verifica se a CPU esta ocupada
        if (!(cpuLivre(gerenciador->cpus[i])) )
        {
            if (gerenciador->tipoEscalonamento == ESC_ROBIN)
            {
                printf("TA PRA ENTRAR NA TROCA DE CONTEXTO DO ROUND ROBIN HEINNN \n");
                // Para Round Robin, usa a versão específica da troca de contexto
                removeProcessoCPU_RR(gerenciador->cpus[i], gerenciador->tabelaProcessos, gerenciador->filaRoundRobin);
                printf("TROCOU DE CONTEXTO NO ROBINHOOO \n");
            }
            else
            {
                // Para filas múltiplas, usa a versão original
                removeProcessoCPU(gerenciador->cpus[i], gerenciador->tabelaProcessos, gerenciador->estadoPronto);
                //printf("TROCA DE CONTEXTO NAS FILAS MP");
            }
        }
    }
}

// Funcao principal que gerencia os processos com base no comando recebido
void gerenciadorProcessos(GerenciadorProcessos *gerenciador, char comando)
{
    if (comando == 'U')
    {
        encerraUnidadeTempo(gerenciador); // 1. Avança o tempo

        if (gerenciador->tempo == 1)
        {
            iniciaProcessoInit(gerenciador);
        }

        executaCPUs(gerenciador);           // 2. Executa quem já está na CPU
        trocaDeContexto(gerenciador);       // 3. Verifica se precisa trocar (RR, prioridade, etc)
        escalonaProcessosCPUs(gerenciador); // 4. Envia novos processos para CPUs livres
    }
}


// Remove um processo da CPU e o coloca na fila apropriada
void removeProcessoCPU(CPU *cpu, Lista *tabelaProcessos, Fila **estadoPronto)
{
    ProcessoSimulado *processoNaCPU = buscaProcesso(tabelaProcessos, (cpu->pidProcessoAtual)); // Busca o processo na CPU

    if (processoNaCPU != NULL)
    {
        if (cpu->fatiaQuantum >= calcularQuantumTotal(2, processoNaCPU->prioridade))
        { // Verifica se o quantum foi excedido
            printf("Quantum excedido, removendo processo da CPU\n");
            *(processoNaCPU->pc) = cpu->pcProcessoAtual; // Atualiza o PC do processo

            processoNaCPU->estadoProcesso = PRONTO; // Define o estado como pronto

            if (processoNaCPU->prioridade < CLASSESPRIORIDADES - 1) // Ajusta a prioridade, se necessario
            {
                processoNaCPU->prioridade++;
            }
            processoNaCPU->tempoCPU += cpu->fatiaQuantum; // Atualiza o tempo de CPU do processo

            enfileirar(processoNaCPU->pid, NUMEROVAZIO, estadoPronto[processoNaCPU->prioridade]); // Reenfileira o processo
            printf("Reenfileirou o processo na fila de prioridade %d\n", processoNaCPU->prioridade);
            zeraCPU(cpu);                                                                         // Libera a CPU
        }
        else if (processoNaCPU->estadoProcesso == BLOQUEADO) // Caso o processo esteja bloqueado
        {
            printf("Processo bloqueado, removendo da CPU\n");
            processoNaCPU->tempoCPU += cpu->fatiaQuantum;
            zeraCPU(cpu);

            if (*processoNaCPU->pc == NUMEROVAZIO) // Remove o processo se ele terminou
            {
                removeDaTabela(tabelaProcessos, processoNaCPU->pid);
            }
        }
    }
}

// Remove um processo da CPU e o coloca na fila Round Robin (nova implementacao)
void removeProcessoCPU_RR(CPU *cpu, Lista *tabelaProcessos, Fila *filaRR)
{
    ProcessoSimulado *processoNaCPU = buscaProcesso(tabelaProcessos, (cpu->pidProcessoAtual)); // Busca o processo na CPU

    if (processoNaCPU != NULL)
    {
        const int QUANTUM_RR = 3;

        if (cpu->fatiaQuantum >= QUANTUM_RR) // Verifica se o quantum foi excedido
        {
            *(processoNaCPU->pc) = cpu->pcProcessoAtual; // Atualiza o PC do processo
            printf("Atualizou pc do processo\n");

            processoNaCPU->estadoProcesso = PRONTO;       // Define o estado como pronto
            printf("Definiu o estado como pronto\n");
            processoNaCPU->tempoCPU += cpu->fatiaQuantum; // Atualiza o tempo de CPU do processo
            printf("Atualizou o tempo de CPU do processo\n");
            
            if (*processoNaCPU->pc == NUMEROVAZIO) {
                removeDaTabela(tabelaProcessos, processoNaCPU->pid);
                zeraCPU(cpu);
                return;
            }

            // No Round Robin, não há alteração de prioridade - apenas reenfileira no final
            enfileirar(processoNaCPU->pid, NUMEROVAZIO, filaRR); 
            // Reenfileira o processo no final da fila RR
  
            zeraCPU(cpu);                                        // Libera a CPU
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

        if (pidTempo->tempoExecutado <= 0)
        { // Se o tempo de bloqueio acabou

            ProcessoSimulado *processo = buscaProcesso(gerenciador->tabelaProcessos, pidTempo->pid);

            if (gerenciador->tipoEscalonamento == ESC_ROBIN)
            {
                // Para Round Robin, reenfileira na fila única
                enfileirar(pidTempo->pid, NUMEROVAZIO, gerenciador->filaRoundRobin);
            }
            else
            {
                // Para filas múltiplas, reenfileira na fila de prioridade correspondente
                enfileirar(pidTempo->pid, NUMEROVAZIO, gerenciador->estadoPronto[processo->prioridade]);
            }
        }
        else
        {
            enfileirar(pidTempo->pid, pidTempo->tempoExecutado, gerenciador->estadoBloqueado); // Reenfileira o processo bloqueado
        }
    }
}

// Remove um processo da tabela de processos
void removeProcessoTabela(ProcessoSimulado *processoEscolhido, GerenciadorProcessos *gerenciador){
    gerenciador->tempoTotalExecucao += processoEscolhido->tempoCPU;       // Atualiza o tempo total de execucao
    removeDaTabela(gerenciador->tabelaProcessos, processoEscolhido->pid); // Remove o processo da tabela
}

void adicionaProcessoEscalonamento(GerenciadorProcessos *gerenciador, ProcessoSimulado *processo)
{
    if (gerenciador->tipoEscalonamento == ESC_ROBIN)
    {
        // Para Round Robin, adiciona na fila única
        enfileirar(processo->pid, NUMEROVAZIO, gerenciador->filaRoundRobin);
    }
    else
    {
        // Para filas múltiplas, adiciona na fila de prioridade mais alta
        enfileirar(processo->pid, NUMEROVAZIO, gerenciador->estadoPronto[0]);
    }
}

// Calcula o quantum total com base na prioridade do processo
double calcularQuantumTotal(double base, int expoente)
{
    double resultado = 1.0;

    for (int i = 0; i < expoente; i++)
    {
        resultado = resultado * base;
    }

    return resultado;
}