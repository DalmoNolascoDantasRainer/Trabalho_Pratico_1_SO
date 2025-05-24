#ifndef GERENCIADOR_PROCESSOS_H
#define GERENCIADOR_PROCESSOS_H

#include "../Pipe/Pipe.h"
#include "Cpu.h"
#include <math.h>
#define CLASSESPRIORIDADES 4 
#include <stdio.h>
#include <stdlib.h>


// Enum para tipos de escalonamento
typedef enum {
    ESC_FILAS_MULTIPLAS = 0,
    ESC_ROBIN = 1
} TipoEscalonamento;

// Estrutura do gerenciador de processos (atualizada)
typedef struct {
    int tempo;
    int quantidadeProcessosIniciados;
    int tempoTotalExecucao;
    int numCPUs;
    int tipoEscalonamento;          // Novo campo para tipo de escalonamento
    CPU **cpus;
    int *estadoExecucao;
    Lista *tabelaProcessos;
    Fila **estadoPronto;            // Para filas múltiplas
    Fila *estadoBloqueado;
    Fila *filaRoundRobin;          // Nova fila para Round Robin
} GerenciadorProcessos;

// Declarações das funções existentes
GerenciadorProcessos *inicializaGerenciador(int numCPUs);
void iniciaProcessoInit(GerenciadorProcessos *gerenciador);
void encerraUnidadeTempo(GerenciadorProcessos *gerenciador);
void escalonaProcesso(Lista *tabelaProcessos, CPU *cpu, int *estadoExecucao, Fila **estadoPronto);
void escalonaProcessosCPUs(GerenciadorProcessos *gerenciador);
void executaCPUs(GerenciadorProcessos *gerenciador);
void trocaDeContexto(GerenciadorProcessos *gerenciador);
void gerenciadorProcessos(GerenciadorProcessos *gerenciador, char comando);
void removeProcessoCPU(CPU *cpu, Lista *tabelaProcessos, Fila **estadoPronto);
void verificaBloqueados(GerenciadorProcessos *gerenciador);
double calcularPotencia(double base, int expoente);

// Novas declarações para Round Robin
void defineEscalonamento(GerenciadorProcessos *gerenciador, TipoEscalonamento tipo);
void escalonaProcessoRR(Lista *tabelaProcessos, CPU *cpu, int *estadoExecucao, Fila *filaRR);
void removeProcessoCPU_RR(CPU *cpu, Lista *tabelaProcessos, Fila *filaRR);
void adicionaProcessoEscalonamento(GerenciadorProcessos *gerenciador, ProcessoSimulado *processo);

#endif // GERENCIADOR_PROCESSOS_H
