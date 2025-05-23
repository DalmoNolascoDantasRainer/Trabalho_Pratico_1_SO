#include "RoundRobin.h"
#include <stdio.h>
#include <stdlib.h>

// Inicializa o escalonador Round Robin com suporte a múltiplas prioridades
EscalonadorRR *inicializaEscalonadorRR(int quantum, int numPrioridades){
    EscalonadorRR *escalonador = (EscalonadorRR *)malloc(sizeof(EscalonadorRR));
    
    if (!escalonador){
        printf("Erro: Falha ao alocar memória para o escalonador ;( \n");
        return NULL;
    }

    escalonador->quantum = quantum;
    escalonador->numPrioridades = numPrioridades;
    escalonador->prioridadeAtual = 0; // Inicia com a maior prioridade

    // Aloca array de filas para cada nível de prioridade
    escalonador->filasPrioridade = (Fila **)malloc(numPrioridades * sizeof(Fila *));

    if (!escalonador->filasPrioridade){
        free(escalonador);
        printf("Erro: Falha ao alocar memória para as filas de prioridade\n");
        return NULL;
    }

    // Inicializa cada fila de prioridade
    for (int i = 0; i < numPrioridades; i++){
        escalonador->filasPrioridade[i] = criaFila();
        if (!escalonador->filasPrioridade[i]){
            // Libera filas já alocadas em caso de erro
            for (int j = 0; j < i; j++){
                liberaFila(escalonador->filasPrioridade[j]);
            }
            free(escalonador->filasPrioridade);
            free(escalonador);
            return NULL;
        }
    }
    
    // RETIRAR DPS
    printf("Escalonador Round Robin inicializado - Quantum: %d, Prioridades: %d\n",
           quantum, numPrioridades);

    return escalonador;
}

// Verifica se o quantum foi esgotado baseado no fatiaQuantum da CPU
int quantumEsgotado(CPU *cpu, int quantum){
    return cpu->fatiaQuantum >= quantum; // Retorna 1 se o quantum foi atingido ou excedido
}

// Seleciona o próximo processo usando Round Robin com prioridades
int selecionaProximoProcessoRR(EscalonadorRR *escalonador, Lista *tabelaProcessos){
    int tentativas = 0;

    // Procura por processos prontos, começando pela maior prioridade
    while (tentativas < escalonador->numPrioridades){
        Fila *filaAtual = escalonador->filasPrioridade[escalonador->prioridadeAtual];

        // Se a fila atual não está vazia, remove o primeiro processo
        if (!filaVazia(filaAtual)){
            PidTempo *elemento = desenfileirar(filaAtual);
            int pidSelecionado = elemento->pid;

            // Verifica se o processo ainda está no estado PRONTO
            ProcessoSimulado *processo = buscaProcesso(tabelaProcessos, pidSelecionado);
            if (processo && processo->estadoProcesso == PRONTO)
            {
                printf("Processo PID %d selecionado (Prioridade %d)\n",
                       pidSelecionado, escalonador->prioridadeAtual);
                return pidSelecionado;
            }

            // Se o processo não está mais pronto, libera o elemento e continua
            free(elemento);
        }

        // Move para a próxima prioridade (Round Robin entre prioridades)
        escalonador->prioridadeAtual = (escalonador->prioridadeAtual + 1) % escalonador->numPrioridades;
        tentativas++;
    }

    // Não há processos prontos em nenhuma fila
    return NUMEROVAZIO;
}

// Adiciona processo na fila de prioridade apropriada
void adicionaProcessoProntoRR(EscalonadorRR *escalonador, int pid, int prioridade){
    // Valida a prioridade
    if (prioridade < 0 || prioridade >= escalonador->numPrioridades){
        printf("Erro: Prioridade %d inválida. Usando prioridade 0.\n", prioridade);
        prioridade = 0;
    }

    // Adiciona o processo na fila da prioridade correspondente
    enfileirar(pid, NUMEROVAZIO, escalonador->filasPrioridade[prioridade]);
    printf("Processo PID %d adicionado à fila de prioridade %d\n", pid, prioridade);
}

// Remove processo da CPU e recoloca na fila (preempção por quantum)
void preemptaProcesso(CPU *cpu, EscalonadorRR *escalonador, Lista *tabelaProcessos){
    if (cpuLivre(cpu)){
        return; // CPU já está livre
    }

    int pidAtual = *(cpu->pidProcessoAtual);
    ProcessoSimulado *processo = buscaProcesso(tabelaProcessos, pidAtual);

    if (!processo)
    {
        printf("Erro: Processo PID %d não encontrado na tabela\n", pidAtual);
        return;
    }

    // Atualiza o tempo de execução do processo
    processo->tempoCPU += cpu->fatiaQuantum;

    // Muda o estado do processo para PRONTO (foi preemptado)
    processo->estadoProcesso = PRONTO;

    // Recoloca o processo na fila de sua prioridade
    adicionaProcessoProntoRR(escalonador, pidAtual, processo->prioridade);

    printf("Processo PID %d preemptado após %d instruções (Quantum esgotado)\n",
           pidAtual, cpu->fatiaQuantum);

    // Libera a CPU
    zeraCPU(cpu);
}

// Executa um ciclo completo do escalonador Round Robin
int executaCicloRR(CPU *cpu, EscalonadorRR *escalonador, Lista *tabelaProcessos,
                   int tempoSistema, int *quantidadeProcessosIniciados,
                   Fila *estadoBloqueado)
{

    // Se CPU está ocupada, verifica se precisa preemptar
    if (!cpuLivre(cpu))
    {
        // Verifica se o quantum foi esgotado
        if (quantumEsgotado(cpu, escalonador->quantum))
        {
            preemptaProcesso(cpu, escalonador, tabelaProcessos);
        }
        else
        {
            // CPU ainda tem quantum, executa próxima instrução
            executaProxInstrucao(cpu, tempoSistema, tabelaProcessos,
                                 quantidadeProcessosIniciados,
                                 escalonador->filasPrioridade, estadoBloqueado);

            // Verifica se o processo terminou ou foi bloqueado
            ProcessoSimulado *processoAtual = buscaProcesso(tabelaProcessos, *(cpu->pidProcessoAtual));
            if (!processoAtual || processoAtual->estadoProcesso != EXECUCAO)
            {
                // Atualiza tempo de execução antes de liberar CPU
                if (processoAtual)
                {
                    processoAtual->tempoCPU += cpu->fatiaQuantum;
                }
                zeraCPU(cpu);
            }

            return 1; // Instrução executada
        }
    }

    // CPU está livre, seleciona próximo processo
    if (cpuLivre(cpu))
    {
        int proximoPid = selecionaProximoProcessoRR(escalonador, tabelaProcessos);

        if (proximoPid != NUMEROVAZIO)
        {
            // Carrega o processo na CPU
            ProcessoSimulado *proximoProcesso = buscaProcesso(tabelaProcessos, proximoPid);

            if (proximoProcesso && proximoProcesso->estadoProcesso == PRONTO)
            {
                proximoProcesso->estadoProcesso = EXECUCAO;
                insereProcessoCPU(cpu, proximoProcesso);

                printf("Processo PID %d carregado na CPU (PC: %d)\n",
                       proximoPid, *(proximoProcesso->pc));

                // Executa primeira instrução
                executaProxInstrucao(cpu, tempoSistema, tabelaProcessos,
                                     quantidadeProcessosIniciados,
                                     escalonador->filasPrioridade, estadoBloqueado);

                // Verifica se o processo terminou ou foi bloqueado imediatamente
                if (proximoProcesso->estadoProcesso != EXECUCAO)
                {
                    proximoProcesso->tempoCPU += cpu->fatiaQuantum;
                    zeraCPU(cpu);
                }

                return 1; // Instrução executada
            }
        }
    }

    // Não há processos prontos para executar
    return 0;
}

// Libera toda a memória alocada para o escalonador
void liberaEscalonadorRR(EscalonadorRR *escalonador)
{
    if (!escalonador)
        return;

    // Libera todas as filas de prioridade
    if (escalonador->filasPrioridade)
    {
        for (int i = 0; i < escalonador->numPrioridades; i++)
        {
            if (escalonador->filasPrioridade[i])
            {
                liberaFila(escalonador->filasPrioridade[i]);
            }
        }
        free(escalonador->filasPrioridade);
    }

    free(escalonador);
    printf("Escalonador Round Robin liberado\n");
}

// Função de debug para visualizar o estado das filas
void imprimeEstadoEscalonadorRR(EscalonadorRR *escalonador)
{
    if (!escalonador)
    {
        printf("Escalonador não inicializado\n");
        return;
    }

    printf("\n=== Estado do Escalonador Round Robin ===\n");
    printf("Quantum: %d | Prioridade Atual: %d\n",
           escalonador->quantum, escalonador->prioridadeAtual);

    for (int i = 0; i < escalonador->numPrioridades; i++)
    {
        printf("Fila Prioridade %d: ", i);

        if (filaVazia(escalonador->filasPrioridade[i]))
        {
            printf("Vazia\n");
        }
        else
        {
            printf("[");
            // Aqui seria ideal ter uma função para imprimir fila sem modificá-la
            // Por simplicidade, mostramos apenas que não está vazia
            printf("Processos presentes");
            printf("]\n");
        }
    }
    printf("==========================================\n\n");
}