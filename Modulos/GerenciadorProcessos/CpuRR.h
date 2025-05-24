#ifndef CPURR_H
#define CPURR_H

#include <stdio.h>
#include <stdlib.h>

#include "../EstruturasDeDados/Fila.h"
#include "TabelaProcessos.h"
#include "../ProcessoSimulado/ProcessoSimulado.h"

typedef struct CPU_RR {
    int pidProcessoAtual;
    Instrucao *programaProcessoAtual;
    int pcProcessoAtual;
    int **variaveisProcessoAtual;
    int fatiaQuantum;       // Quanto do "quantum" de tempo ja foi usada pelo processo em execucao
} CPU_RR;

void inicializaCPURR(CPU_RR** cpu);
void insereProcessoCPURR(CPU_RR* cpu, ProcessoSimulado* processoAtual);
void executaProxInstrucaoCPURR(CPU_RR* cpu, int tempoAtualSistema, Lista* tabelaProcessos,
                          int* quantidadeProcessosIniciados, Fila* filaRoundRobin, Fila* estadoBloqueado);
int cpuLivreRR(CPU_RR* cpu);
void zeraCPURR(CPU_RR* cpu);

/* -------------- Instrucoes de programa que sao processadas na CPU_RR -------------- */

int* instrucaoTipoNRR(int n);
void instrucaoTipoDRR(int x, int *vetorVariaveis);
void instrucaoTipoVRR(int x, int n, int *vetorVariaveis);
void instrucaoTipoARR(int x, int n, int *vetorVariaveis);
void instrucaoTipoSRR(int x, int n, int *vetorVariaveis);
void instrucaoTipoBRR(int n, int* pidProcessoAtual, Lista* tabelaProcessos, Fila* estadoBloqueado);
void instrucaoTipoTRR(int* pidProcessoAtual, Lista* tabelaProcessos);
void instrucaoTipoFRR(int n, int* pidProcessoAtual, int* pcProcessoAtual, int* quantidadeProcessosIniciados, 
                    int tempoAtualSistema, Lista* tabelaProcessos, Fila* filaRoundRobin);
void instrucaoTipoRRR(char *nomeDoArquivo, Instrucao** vetorPrograma, int* pcProcessoAtual);

#endif