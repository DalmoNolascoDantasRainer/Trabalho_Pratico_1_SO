#include "GerenciadorProcessos.h"

// Função que inicializa o gerenciador de processos
GerenciadorProcessos *inicializaGerenciador(int numCPUs)
{
    // Aloca memória para o gerenciador de processos
    GerenciadorProcessos *gerenciador = (GerenciadorProcessos *)malloc(sizeof(GerenciadorProcessos));

    // Inicializa os atributos do gerenciador
    gerenciador->tempo = 0;
    gerenciador->quantidadeProcessosIniciados = 0;
    gerenciador->tempoTotalExecucao = 0;
    gerenciador->numCPUs = numCPUs;

    // Aloca memória para as CPUs e seus estados de execução
    gerenciador->cpus = (CPU **)malloc(numCPUs * sizeof(CPU *));
    gerenciador->estadoExecucao = (int *)malloc(numCPUs * sizeof(int));

    // Inicializa cada CPU e define o estado como vazio
    for (int i = 0; i < numCPUs; i++)
    {
        gerenciador->cpus[i] = inicializaCPU();
        gerenciador->estadoExecucao[i] = NUMEROVAZIO;
    }

    // Cria a tabela de processos
    gerenciador->tabelaProcessos = criaLista();

    // Aloca memória para as filas de processos prontos, uma para cada classe de prioridade
    gerenciador->estadoPronto = (Fila **)malloc(sizeof(Fila *) * NUMCLASPRIORI);

    // Inicializa as filas de processos prontos
    for (int i = 0; i < NUMCLASPRIORI; i++)
    {
        gerenciador->estadoPronto[i] = criaFila();
    }

    // Inicializa a fila de processos bloqueados
    gerenciador->estadoBloqueado = criaFila();

    return gerenciador;
}

// Função que inicia o processo inicial (init)
void iniciaProcessoInit(GerenciadorProcessos *gerenciador)
{
    // Cria o processo inicial
    ProcessoSimulado *processoInit = criaProcessoInit(gerenciador->tempo);

    // Enfileira o processo inicial na fila de prioridade mais alta
    enfileira(processoInit->pid, NUMEROVAZIO, gerenciador->estadoPronto[0]);

    // Insere o processo inicial na tabela de processos
    insereTabela(gerenciador->tabelaProcessos, processoInit);

    // Incrementa o contador de processos iniciados
    gerenciador->quantidadeProcessosIniciados += 1;
}

// Função principal que gerencia os processos com base no comando recebido
void gerenciadorProcessos(GerenciadorProcessos *gerenciador, char comando)
{
    if (comando == 'U') // Comando para avançar uma unidade de tempo
    {
        encerraUnidadeTempo(gerenciador); // Incrementa o tempo do sistema

        if (gerenciador->tempo == 1) // Na primeira unidade de tempo
        {
            iniciaProcessoInit(gerenciador); // Inicia o processo inicial
            escalonaProcessosCPUs(gerenciador); // Escalona os processos para as CPUs
            executaCPUs(gerenciador); // Executa os processos nas CPUs
            trocaDeContexto(gerenciador); // Realiza troca de contexto, se necessário
        }
        else
        {
            escalonaProcessosCPUs(gerenciador);
            executaCPUs(gerenciador);
            trocaDeContexto(gerenciador);
        }
    }
}

// Incrementa o tempo do sistema
void encerraUnidadeTempo(GerenciadorProcessos *gerenciador)
{
    gerenciador->tempo += 1;
}

// Escalona processos para as CPUs disponíveis
void escalonaProcessosCPUs(GerenciadorProcessos *gerenciador)
{
    verificaBloqueados(gerenciador); // Verifica e desbloqueia processos, se necessário

    for (int i = 0; i < gerenciador->numCPUs; i++)
    {
        if (cpuLivre(gerenciador->cpus[i]) == 1) // Verifica se a CPU está livre
        {
            if (filasVazias(gerenciador->estadoPronto, NUMCLASPRIORI) == 0) // Verifica se há processos prontos
            {
                escalonaProcesso(gerenciador->tabelaProcessos, gerenciador->cpus[i], gerenciador->estadoExecucao + i, gerenciador->estadoPronto);
            }
        }
    }
}

// Escalona um processo específico para uma CPU
void escalonaProcesso(Lista *tabelaProcessos, CPU *cpu, int *estadoExecucao, Fila **estadoPronto)
{
    int pidProcesso = desenfileirarFilas(estadoPronto, NUMCLASPRIORI); // Obtém o próximo processo da fila

    if (pidProcesso >= 0)
    {
        *estadoExecucao = pidProcesso; // Atualiza o estado de execução da CPU

        ProcessoSimulado *proximoProceso = buscaProcesso(tabelaProcessos, pidProcesso); // Busca o processo na tabela

        proximoProceso->estadoProcesso = EXECUCAO; // Define o estado do processo como em execução

        carregaProcesso(cpu, proximoProceso); // Carrega o processo na CPU
    }
}

// Executa os processos carregados nas CPUs
void executaCPUs(GerenciadorProcessos *gerenciador)
{
    for (int i = 0; i < gerenciador->numCPUs; i++)
    {
        if (!(cpuLivre(gerenciador->cpus[i]))) // Verifica se a CPU está ocupada
        {
            executaProxInstrucao(gerenciador->cpus[i], gerenciador->tempo, gerenciador->tabelaProcessos, &gerenciador->quantidadeProcessosIniciados, gerenciador->estadoPronto, gerenciador->estadoBloqueado);
        }
    }
}

// Realiza a troca de contexto nas CPUs
void trocaDeContexto(GerenciadorProcessos *gerenciador)
{
    for (int i = 0; i < gerenciador->numCPUs; i++)
    {
        if (!(cpuLivre(gerenciador->cpus[i]))) // Verifica se a CPU está ocupada
        {
            removeProcessoCPU(gerenciador->cpus[i], gerenciador->tabelaProcessos, gerenciador->estadoPronto);
        }
    }
}

// Remove um processo da CPU e o coloca na fila apropriada
void removeProcessoCPU(CPU *cpu, Lista *tabelaProcessos, Fila **estadoPronto)
{
    ProcessoSimulado *processoNaCPU = buscaProcesso(tabelaProcessos, *(cpu->pidProcessoAtual)); // Busca o processo na CPU

    if (processoNaCPU != NULL)
    {
        if (cpu->fatiaQuantum >= calcPot(2, processoNaCPU->prioridade)) // Verifica se o quantum foi excedido
        {
            processoNaCPU->estadoProcesso = PRONTO; // Define o estado como pronto

            if (processoNaCPU->prioridade < NUMCLASPRIORI - 1) // Ajusta a prioridade, se necessário
            {
                processoNaCPU->prioridade++;
            }
            processoNaCPU->tempoCPU += cpu->fatiaQuantum; // Atualiza o tempo de CPU do processo

            enfileira(processoNaCPU->pid, NUMEROVAZIO, estadoPronto[processoNaCPU->prioridade]); // Reenfileira o processo
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

// Verifica e desbloqueia processos bloqueados, se necessário
void verificaBloqueados(GerenciadorProcessos *gerenciador)
{
    for (int i = 0; i < gerenciador->estadoBloqueado->Tamanho; i++)
    {
        PidTempo *pidTempo = desenfileirar(gerenciador->estadoBloqueado); // Obtém o próximo processo bloqueado

        pidTempo->tempoExecutado--; // Decrementa o tempo de bloqueio

        if (pidTempo->tempoExecutado <= 0) // Se o tempo de bloqueio acabou
        {
            ProcessoSimulado *processo = buscaProcesso(gerenciador->tabelaProcessos, pidTempo->pid);
            enfileira(pidTempo->pid, NUMEROVAZIO, gerenciador->estadoPronto[processo->prioridade]); // Reenfileira o processo como pronto
        }
        else
        {
            enfileira(pidTempo->pid, pidTempo->tempoExecutado, gerenciador->estadoBloqueado); // Reenfileira o processo bloqueado
        }
    }
}

// Remove um processo da tabela de processos
void removeProcessoTabela(ProcessoSimulado *processoEscolhido, GerenciadorProcessos *gerenciador)
{
    gerenciador->tempoTotalExecucao += processoEscolhido->tempoCPU; // Atualiza o tempo total de execução
    removeTabela(gerenciador->tabelaProcessos, processoEscolhido->pid); // Remove o processo da tabela
}

// Calcula a potência de um número
double calcPot(double base, int expoente)
{
    double resultado = 1.0;

    for (int i = 0; i < expoente; i++)
    {
        resultado *= base;
    }

    return resultado;
}