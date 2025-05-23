/*
void sjf(Lista* tabelaProcessos) {
    while (temProcessosPendentes(tabelaProcessos)) {
        // Busca o processo com menor tempo de execução
        ProcessoSimulado* menorProcesso = encontraMenorTempo(tabelaProcessos);
        
        // Executa completamente
        printf("Executando PID %d por %d unidades\n", 
               menorProcesso->pid, menorProcesso->tempoExecucao);
        
        tempoTotal += menorProcesso->tempoExecucao;
        removeTabela(tabelaProcessos, menorProcesso->pid);
    }
}
    
*/

#include "Lista.h"
#include <stdio.h>
#include <limits.h>

/* Funcao para encontrar o processo com menor tempo de execucao 
    Percorre a lista.

    Mantém uma variável menorTempo inicializada com INT_MAX.

    Sempre que encontra um processo com tempoExecucao menor, atualiza.
*/
ProcessoSimulado* encontraMenorTempo(Lista* tabelaProcessos) {
    if (listaVazia(tabelaProcessos)) {
        return NULL;
    }
    
    ProcessoSimulado* menorProcesso = NULL;
    int menorTempo = INT_MAX;
    
    // Percorre todos os processos para encontrar o menor tempo
    Celula* percorre = tabelaProcessos->inicio->proximo;
    while (percorre != NULL) {
        ProcessoSimulado* processo = percorre->processo;
        
        if (processo->tempoExecucao < menorTempo) {
            menorTempo = processo->tempoExecucao;
            menorProcesso = processo;
        }
        percorre = percorre->proximo;
    }
    
    return menorProcesso; // o processo com menor tempo, ou NULL se a lista estiver vazia.
}

// Funcao para verificar se ainda há processos para executar
int temProcessosPendentes(Lista* tabelaProcessos) {
    return !listaVazia(tabelaProcessos); // retorna 1 (verdadeiro) se a lista não está vazia.
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
    Celula* contador = tabelaProcessos->inicio->proximo;
    while (contador != NULL) {
        totalProcessos++;
        contador = contador->proximo;
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
        processoEscolhido->tempoTurnaround = tempoTotal + processoEscolhido->tempoExecucao - processoEscolhido->tempoChegada;
        
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
    
    // Cria uma cópia da lista para preservar os dados originais
    Lista* copia = criaLista();
    copiarProcessos(tabelaProcessos, copia);
    
    printf("Iniciando escalonamento SJF...\n");
    
    // Mostra processos antes da execução
    printf("\n--- Processos antes da execucao ---\n");
    printf("PID\tChegada\tExecucao\n");
    Celula* percorre = copia->inicio->proximo;
    while (percorre != NULL) {
        ProcessoSimulado* processo = percorre->processo;
        printf("%d\t%d\t%d\n", processo->pid, processo->tempoChegada, processo->tempoExecucao);
        percorre = percorre->proximo;
    }
    
    // Executa o algoritmo
    sjf(copia);
    
    // Calcula estatísticas finais
    calcularEstatisticasSJF(tabelaProcessos);
}

// Funcao auxiliar para copiar processos entre listas
void copiarProcessos(Lista* origem, Lista* destino) {
    if (listaVazia(origem)) return;
    
    Celula* percorre = origem->inicio->proximo;
    while (percorre != NULL) {
        ProcessoSimulado* processoOriginal = percorre->processo;
        
        // Cria uma cópia do processo
        ProcessoSimulado* copia = (ProcessoSimulado*) malloc(sizeof(ProcessoSimulado));
        copia->pid = processoOriginal->pid;
        copia->tempoChegada = processoOriginal->tempoChegada;
        copia->tempoExecucao = processoOriginal->tempoExecucao;
        copia->tempoRestante = processoOriginal->tempoExecucao;
        copia->tempoTurnaround = 0;
        
        insereTabela(destino, copia);
        percorre = percorre->proximo;
    }
}

// Funcao para calcular estatísticas do SJF
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
        
        int tempoTurnaround = tempoAtual + menor->tempoExecucao - menor->tempoChegada;
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