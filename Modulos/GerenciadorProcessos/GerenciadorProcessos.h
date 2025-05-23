#ifndef GERENCIADORPROCESSOS_H
#define GERENCIADORPROCESSOS_H


#include "../Pipe/Pipe.h"
#include "Cpu.h"
#include <math.h>
#define CLASSESPRIORIDADES 4 

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
void removeProcessoCPU(CPU* cpu, Lista* tabelaProcessos, Fila** estadoPronto);
void verificaBloqueados(GerenciadorProcessos* gerenciador);
void iniciaProcessoInit(GerenciadorProcessos *gerenciador);
void removeProcessoTabela(ProcessoSimulado *processoEscolhido, GerenciadorProcessos *gerenciador);
double calcularPotencia(double base, int expoente);

#endif 