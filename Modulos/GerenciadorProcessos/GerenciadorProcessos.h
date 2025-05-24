#ifndef GERENCIADORPROCESSOS_H
#define GERENCIADORPROCESSOS_H


#include "../Pipe/Pipe.h"
#include "Cpu.h"
#include <math.h>
#define CLASSESPRIORIDADES 4 
#include <stdio.h>
#include <stdlib.h>

typedef enum {
    ESC_FILAS_MULTIPLAS = 0,
    ESC_ROBIN = 1
} TipoEscalonamento;

typedef struct GerenciadorProcessos {
    int tempo;
    CPU** cpus;
    Lista *tabelaProcessos;
    Fila** estadoPronto;
    Fila* estadoBloqueado;
    int* estadoExecucao;
    int quantidadeProcessosIniciados;
    int tempoTotalExecucao;
    int numCPUs;   
    int tipoEscalonamento;          // Novo campo para tipo de escalonamento
    Fila* filaRoundRobin;          // Nova fila para Round Robin
} GerenciadorProcessos;

GerenciadorProcessos* inicializaGerenciador(int numCPUs);
void iniciaProcessoInit(GerenciadorProcessos *gerenciador);
void encerraUnidadeTempo(GerenciadorProcessos *gerenciador);
void escalonaProcesso(Lista* tabelaProcessos, CPU* cpu, int* estadoExecucao, Fila** estadoPronto);
void escalonaProcessosCPUs(GerenciadorProcessos* gerenciador);
void executaCPUs(GerenciadorProcessos* gerenciador);
void trocaDeContexto(GerenciadorProcessos* gerenciador);
void gerenciadorProcessos(GerenciadorProcessos* gerenciador, char comando);
void removeProcessoCPU(CPU* cpu, Lista* tabelaProcessos, Fila** estadoPronto);
void verificaBloqueados(GerenciadorProcessos* gerenciador);
double calcularQuantumTotal(double base, int expoente);
void defineEscalonamento(GerenciadorProcessos *gerenciador, TipoEscalonamento tipo);
void escalonaProcessoRR(Lista* tabelaProcessos, CPU* cpu, int* estadoExecucao, Fila* filaRR);
void removeProcessoCPU_RR(CPU* cpu, Lista* tabelaProcessos, Fila* filaRR);
void adicionaProcessoEscalonamento(GerenciadorProcessos* gerenciador, ProcessoSimulado* processo);

#endif // GERENCIADOR_PROCESSOS

