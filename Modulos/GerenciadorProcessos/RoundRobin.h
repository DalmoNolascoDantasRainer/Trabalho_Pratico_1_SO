#ifndef ROUND_ROBIN_H
#define ROUND_ROBIN_H

#include "ProcessoSimulado.h"
#include "Cpu.h"
#include "Fila.h"
#include "Lista.h"
#include "GerenciadorProcessos.h"

// Estrutura do escalonador Round Robin adaptada para múltiplas CPUs
typedef struct {
    int quantumBase;               // Quantum base para Round Robin puro
    int usarQuantumFixo;          // 1 = quantum fixo, 0 = quantum variável por prioridade
    int numCPUs;                  // Número de CPUs no sistema
    int* proximaFila;             // Próxima fila a ser verificada para cada CPU (Round Robin entre filas)
} EscalonadorRR;

/**
 * Inicializa o escalonador Round Robin para múltiplas CPUs
 * @param quantumBase Quantum base para todas as filas (se fixo) ou para prioridade 0
 * @param usarQuantumFixo 1 para quantum fixo, 0 para quantum crescente por prioridade
 * @param numCPUs Número de CPUs no sistema
 * @return Ponteiro para o escalonador inicializado
 */
EscalonadorRR* inicializaEscalonadorRR(int quantumBase, int usarQuantumFixo, int numCPUs);

/**
 * Calcula o quantum para um processo baseado em sua prioridade
 * @param escalonador Ponteiro para o escalonador RR
 * @param prioridade Prioridade do processo (0 = maior prioridade)
 * @return Quantum a ser usado para este processo
 */
int calculaQuantum(EscalonadorRR* escalonador, int prioridade);

/**
 * Verifica se o quantum do processo atual foi esgotado
 * @param cpu Ponteiro para a CPU
 * @param quantum Quantum configurado para o processo atual
 * @return 1 se quantum esgotado, 0 caso contrário
 */
int quantumEsgotadoRR(CPU* cpu, int quantum);

/**
 * Escalona processos usando Round Robin puro (sem mudança de prioridade)
 * Substitui a função escalonaProcessosCPUs do gerenciador original
 * @param gerenciador Ponteiro para o gerenciador de processos
 * @param escalonadorRR Ponteiro para o escalonador Round Robin
 */
void escalonaProcessosRR(GerenciadorProcessos* gerenciador, EscalonadorRR* escalonadorRR);

/**
 * Seleciona próximo processo usando Round Robin entre todas as filas
 * @param estadoPronto Array de filas de prioridade
 * @param escalonadorRR Ponteiro para o escalonador RR
 * @param cpuIndex Índice da CPU que está solicitando processo
 * @return PID do próximo processo ou valor negativo se não há processos
 */
int selecionaProximoProcessoRR(Fila** estadoPronto, EscalonadorRR* escalonadorRR, int cpuIndex);

/**
 * Remove processo da CPU aplicando Round Robin puro
 * Substitui removeProcessoCPU para não alterar prioridades
 * @param cpu Ponteiro para a CPU
 * @param tabelaProcessos Tabela de processos
 * @param estadoPronto Array de filas de prioridade
 * @param escalonadorRR Ponteiro para o escalonador RR
 */
void removeProcessoCPU_RR(CPU* cpu, Lista* tabelaProcessos, Fila** estadoPronto, EscalonadorRR* escalonadorRR);

/**
 * Executa um ciclo completo do gerenciador com Round Robin
 * Versão modificada da função gerenciadorProcessos
 * @param gerenciador Ponteiro para o gerenciador de processos
 * @param escalonadorRR Ponteiro para o escalonador Round Robin
 * @param comando Comando a ser executado ('U' para unidade de tempo)
 */
void gerenciadorProcessosRR(GerenciadorProcessos* gerenciador, EscalonadorRR* escalonadorRR, char comando);

/**
 * Realiza troca de contexto usando Round Robin
 * Versão modificada da função trocaDeContexto
 * @param gerenciador Ponteiro para o gerenciador de processos
 * @param escalonadorRR Ponteiro para o escalonador Round Robin
 */
void trocaDeContextoRR(GerenciadorProcessos* gerenciador, EscalonadorRR* escalonadorRR);

/**
 * Libera a memória alocada para o escalonador
 * @param escalonadorRR Ponteiro para o escalonador a ser liberado
 */
void liberaEscalonadorRR(EscalonadorRR* escalonadorRR);

/**
 * Imprime estatísticas do escalonador Round Robin
 * @param escalonadorRR Ponteiro para o escalonador
 * @param gerenciador Ponteiro para o gerenciador de processos
 */
void imprimeEstatisticasRR(EscalonadorRR* escalonadorRR, GerenciadorProcessos* gerenciador);

/**
 * Função auxiliar para imprimir estado das filas (debug)
 * @param estadoPronto Array de filas de prioridade
 * @param escalonadorRR Ponteiro para o escalonador
 */
void imprimeEstadoFilasRR(Fila** estadoPronto, EscalonadorRR* escalonadorRR);

#endif // ROUND_ROBIN_H