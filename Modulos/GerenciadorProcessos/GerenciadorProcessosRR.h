#ifndef GerenciadorProcessosRR_H
#define GerenciadorProcessosRR_H

#include "../Pipe/Pipe.h"
#include "CpuRR.h"
#include <math.h>

typedef struct GerenciadorProcessosRR {
    int tempo;
    CPU_RR** cpus;
    Lista *tabelaProcessos;
    Fila* filaRoundRobin;        // Fila única de prontos (Round Robin)
    Fila* estadoBloqueado;
    int* estadoExecucao;
    int quantidadeProcessosIniciados;
    int tempoTotalExecucao;
    int numCPUs;
} GerenciadorProcessosRR;

// Inicialização
GerenciadorProcessosRR* inicializaGerenciadorRR(int numCPUs);

// Gerência de processos
void iniciaProcessoInitRR(GerenciadorProcessosRR *gerenciador);
void gerenciadorProcessosRR(GerenciadorProcessosRR* gerenciador, char comando);
void encerraUnidadeTempoRR(GerenciadorProcessosRR *gerenciador);

// Escalonamento e execução
void escalonaProcessosCPUsRR(GerenciadorProcessosRR* gerenciador);
void escalonaProcessoRR(Lista* tabelaProcessos, CPU_RR* cpu, int* estadoExecucao, Fila* filaRR);
void executaCPUsRR(GerenciadorProcessosRR* gerenciador);
void trocaDeContextoRR(GerenciadorProcessosRR* gerenciador);
void removeProcessoCPURR(CPU_RR* cpu, Lista* tabelaProcessos, Fila* filaRR);
void verificaBloqueadosRR(GerenciadorProcessosRR* gerenciador);

#endif
