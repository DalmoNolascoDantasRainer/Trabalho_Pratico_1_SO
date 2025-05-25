#ifndef GERENCIADORPROCESSOSRR_H
#define GERENCIADORPROCESSOSRR_H


#include "../Pipe/Pipe.h"
#include "Cpu.h"
#include <math.h>
#define CLASSESPRIORIDADES 4 

typedef struct GerenciadorProcessos {
    int tempo;
    CPU** cpus;
    Lista *tabelaProcessos;
    Fila** estadoPronto;  // E um ponteiro para ponteiro pois cada posicao do vetor guarda uma lista (de acordo com a prioridade)
    Fila* estadoBloqueado;
    int* estadoExecucao;
    int quantidadeProcessosIniciados;
    int tempoTotalExecucao;
    int numCPUs;   
} GerenciadorProcessos;

GerenciadorProcessos* inicializaGerenciador(int numCPUs);
void iniciaProcessoInit(GerenciadorProcessos *gerenciador);
void gerenciadorProcessos(GerenciadorProcessos* gerenciador, char comando);
void encerraUnidadeTempo(GerenciadorProcessos *gerenciador);


/*------------------------------- Funçoes que operam processos -------------------------------*/
void escalonaProcessosCPUs(GerenciadorProcessos* gerenciador);
void escalonaProcesso(Lista* tabelaProcessos, CPU* cpu, int* estadoExecucao, Fila** estadoPronto);
void executaCPUs(GerenciadorProcessos* gerenciador);
void trocaDeContexto(GerenciadorProcessos* gerenciador);
void removeProcessoCPU(CPU *cpu, Lista *tabelaProcessos, Fila **estadoPronto, GerenciadorProcessos *gerenciador);
void verificaBloqueados(GerenciadorProcessos* gerenciador);
void iniciaProcessoInit(GerenciadorProcessos *gerenciador);
double calcularPotencia(double base, int expoente);
void removeProcessoTabela(ProcessoSimulado *processoEscolhido, GerenciadorProcessos *gerenciador);

#endif 