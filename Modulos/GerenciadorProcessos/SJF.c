#include "SJF.h"
#include <stdio.h>
#include <limits.h>

/* Funcao para encontrar o processo com menor tempo de execucao 
    Percorre a lista.

    Mantem uma variavel menorTempo inicializada com INT_MAX.

    Sempre que encontra um processo com tempoExecucao menor, atualiza.
*/
ProcessoSimulado* encontraMenorTempo(Lista* tabelaProcessos) {
    if (listaVazia(tabelaProcessos)) {
        return NULL;
    }
    
    ProcessoSimulado* menorProcesso = NULL;
    int menorTempo = INT_MAX;
    
    // Percorre todos os processos para encontrar o menor tempo
    CelulaPtr atual = tabelaProcessos->Primeiro->Prox;
    while (atual != NULL) {
        ProcessoSimulado* processo = atual->processo;
        
        if (processo->tempoExecucao < menorTempo) {
            menorTempo = processo->tempoExecucao;
            menorProcesso = processo;
        }
        atual = atual->Prox;
    }
    
    return menorProcesso; // o processo com menor tempo, ou NULL se a lista estiver vazia.
}

// Funcao para verificar se ainda ha processos para executar
int temProcessosPendentes(Lista* tabelaProcessos) {
    return !listaVazia(tabelaProcessos); // retorna 1 (verdadeiro) se a lista nao esta vazia.
}

// Funcao principal do algoritmo SJF
void sjf(Lista* tabelaProcessos) {
    if (listaVazia(tabelaProcessos)) {
        printf("Nenhum processo para escalonar.\n");
        return;
    }
    
    int tempoTotal = 0;
    int processosExecutados = 0;
    int totalProcessos = 0;
    
    // Conta o total de processos
    CelulaPtr contador = tabelaProcessos-> Primeiro->Prox;
    while (contador != NULL) {
        totalProcessos++;
        contador = contador->Prox;
    }
    
    printf("\n=== EXECUCAO SJF (Shortest Job First) ===\n");
    printf("Total de processos: %d\n\n", totalProcessos);
    
    // Loop principal do SJF
    while (temProcessosPendentes(tabelaProcessos)) {
        // Encontra o processo com menor tempo de execucao
        ProcessoSimulado* processoEscolhido = encontraMenorTempo(tabelaProcessos);
        
        if (processoEscolhido == NULL) {
            break;
        }
        
        // Executa o processo completamente
        printf("Tempo %d: Executando processo PID %d por %d unidades", 
               tempoTotal, processoEscolhido->pid, processoEscolhido->tempoExecucao);
        
        // Calcula estatísticas do processo
        processoEscolhido->tempoTurnaround = tempoTotal + processoEscolhido->tempoExecucao - processoEscolhido->tempoInicio;
        
        // Atualiza o tempo total
        tempoTotal += processoEscolhido->tempoExecucao;
        processosExecutados++;
        
        printf(" - FINALIZADO\n");
        
        // Remove o processo da tabela (já foi executado)
        removeTabela(tabelaProcessos, processoEscolhido->pid);
    }
    
    printf("\n=== ESTATISTICAS FINAIS ===\n");
    printf("Tempo total de execucao: %d\n", tempoTotal);
    printf("Processos executados: %d/%d\n", processosExecutados, totalProcessos);
}

// Funcao para executar SJF e calcular estatísticas detalhadas
void executarSJF(Lista* tabelaProcessos) {
    if (listaVazia(tabelaProcessos)) {
        printf("Tabela de processos vazia!\n");
        return;
    }
    
    // Cria uma copia da lista para preservar os dados originais
    Lista* copia = criaLista();
    copiarProcessos(tabelaProcessos, copia);
    
    printf("Iniciando escalonamento SJF...\n");
    
    // Mostra processos antes da execucao
    printf("\n--- Processos antes da execucao ---\n");
    printf("PID\tChegada\tExecucao\n");
    CelulaPtr atual = copia->Primeiro->Prox;
    while (atual != NULL) {
        ProcessoSimulado* processo = atual->processo;
        printf("%d\t%d\t%d\n", processo->pid, processo->tempoInicio, processo->tempoExecucao);
        atual = atual->Prox;
    }
    
    // Executa o algoritmo
    sjf(copia);
    
    // Calcula estatisticas finais
    calcularEstatisticasSJF(tabelaProcessos);
}

// Funcao auxiliar para copiar processos entre listas
void copiarProcessos(Lista* origem, Lista* destino) {
    if (listaVazia(origem)) return;
    
    CelulaPtr atual = origem->Primeiro->Prox;
    while (atual != NULL) {
        ProcessoSimulado* processoOriginal = atual->processo;
        
        // Cria uma copia do processo
        ProcessoSimulado* copia = (ProcessoSimulado*) malloc(sizeof(ProcessoSimulado));
        copia->pid = processoOriginal->pid;
        copia->tempoInicio = processoOriginal->tempoInicio;
        copia->tempoExecucao = processoOriginal->tempoExecucao;
        copia->tempoRestante = processoOriginal->tempoExecucao;
        copia->tempoTurnaround = 0;
        
        insereTabela(destino, copia);
        atual = atual->Prox;
    }
}

// Funcao para calcular estatisticas do SJF
void calcularEstatisticasSJF(Lista* tabelaProcessos) {
    if (listaVazia(tabelaProcessos)) return;
    
    // Simula a execução SJF para calcular tempos
    Lista* processosCopia = criaLista();
    copiarProcessos(tabelaProcessos, processosCopia);
    
    int tempoAtual = 0;
    int somaTurnaround = 0;
    int somaEspera = 0;
    int numProcessos = 0;
    
    printf("\n--- Ordem de execucao SJF ---\n");
    printf("Ordem\tPID\tTempo Exec\tTurnaround\tEspera\n");
    
    int ordem = 1;
    while (!listaVazia(processosCopia)) {
        ProcessoSimulado* menor = encontraMenorTempo(processosCopia);
        
        int tempoTurnaround = tempoAtual + menor->tempoExecucao - menor->tempoInicio;
        int tempoEspera = tempoTurnaround - menor->tempoExecucao;
        
        printf("%d\t%d\t%d\t\t%d\t\t%d\n", 
               ordem, menor->pid, menor->tempoExecucao, tempoTurnaround, tempoEspera);
        
        tempoAtual += menor->tempoExecucao;
        somaTurnaround += tempoTurnaround;
        somaEspera += tempoEspera;
        numProcessos++;
        ordem++;
        
        removeTabela(processosCopia, menor->pid);
    }
    
    if (numProcessos > 0) {
        printf("\n--- Medias SJF ---\n");
        printf("Tempo medio de turnaround: %.2f\n", (float)somaTurnaround / numProcessos);
        printf("Tempo medio de espera: %.2f\n", (float)somaEspera / numProcessos);
        printf("Tempo total de execucao: %d\n", tempoAtual);
    }
}