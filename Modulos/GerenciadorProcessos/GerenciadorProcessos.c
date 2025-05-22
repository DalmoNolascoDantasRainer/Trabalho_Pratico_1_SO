#include "GerenciadorProcessos.h"

// Funcao que inicializa o gerenciador de processos
GerenciadorProcessos *inicializaGerenciador(int numCPUs) {
    // Aloca memoria para o gerenciador de processos
    GerenciadorProcessos *gerenciador = (GerenciadorProcessos *)malloc(sizeof(GerenciadorProcessos));

    // Inicializa os atributos do gerenciador
    gerenciador->tempo = 0;
    gerenciador->quantidadeProcessosIniciados = 0;
    gerenciador->tempoTotalExecucao = 0;
    gerenciador->numCPUs = numCPUs;

    // Aloca memoria para as CPUs e seus estados de execucao
    gerenciador->cpus = (CPU **)malloc(numCPUs * sizeof(CPU *));
    gerenciador->estadoExecucao = (int *)malloc(numCPUs * sizeof(int));

    // Inicializa cada CPU e define o estado como vazio
    for (int i = 0; i < numCPUs; i++){
        gerenciador->cpus[i] = inicializaCPU();
        gerenciador->estadoExecucao[i] = NUMEROVAZIO;
    }

    // Cria a tabela de processos
    gerenciador->tabelaProcessos = criaLista();

    // Aloca memoria para as filas de processos prontos, uma para cada classe de prioridade
    gerenciador->estadoPronto = (Fila **)malloc(sizeof(Fila *) * CLASSESPRIORIDADES);

    // Inicializa as filas de processos prontos
    for (int i = 0; i < CLASSESPRIORIDADES; i++){
        gerenciador->estadoPronto[i] = criaFila();
    }

    // Inicializa a fila de processos bloqueados
    gerenciador->estadoBloqueado = criaFila();

    return gerenciador;
}

// Funcao que inicia o processo inicial (init)
void iniciaProcessoInit(GerenciadorProcessos *gerenciador) {
    // Cria o processo inicial
    ProcessoSimulado *processoInit = criaProcessoInit(gerenciador->tempo);

    // Enfileira o processo inicial na fila de prioridade mais alta
    enfileirar(processoInit->pid, NUMEROVAZIO, gerenciador->estadoPronto[0]);

    // Insere o processo inicial na tabela de processos
    insereTabela(gerenciador->tabelaProcessos, processoInit);

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

        carregaProcesso(cpu, proximoProceso); // Carrega o processo na CPU
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
    ProcessoSimulado *processoNaCPU = buscaProcesso(tabelaProcessos, *(cpu->pidProcessoAtual)); // Busca o processo na CPU

    if (processoNaCPU != NULL){
        if (cpu->fatiaQuantum >= calcPot(2, processoNaCPU->prioridade)){ // Verifica se o quantum foi excedido
        
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
                removeTabela(tabelaProcessos, processoNaCPU->pid);
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
void removeProcessoTabela(ProcessoSimulado *processoEscolhido, GerenciadorProcessos *gerenciador)
{
    gerenciador->tempoTotalExecucao += processoEscolhido->tempoCPU; // Atualiza o tempo total de execucao
    removeTabela(gerenciador->tabelaProcessos, processoEscolhido->pid); // Remove o processo da tabela
}

// Calcula a potencia de um numero
double calcPot(double base, int expoente)
{
    double resultado = 1.0;

    for (int i = 0; i < expoente; i++)
    {
        resultado *= base;
    }

    return resultado;
}