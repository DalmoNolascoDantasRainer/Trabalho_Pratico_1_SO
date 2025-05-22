#ifndef CPU_H
#define CPU_H


#include <stdio.h>
#include <stdlib.h>

#include "../EstruturasDeDados/Fila.h"
#include "../EstruturasDeDados/Lista.h"

typedef struct CPU{
    int *pidProcessoAtual;
    Instrucao **programaProcessoAtual;
    int *pcProcessoAtual;
    int **variaveisProcessoAtual;
    int fatiaQuantum;       //Quanto do "quantum" de tempo ja foi usada pelo processo em execucao
} CPU;

CPU* inicializaCPU();
void carregaProcesso(CPU* cpu, ProcessoSimulado* processoAtual);
void executaProxInstrucao(CPU* cpu, int tempoAtualSistema, Lista* tabelaProcessos,
                             int* quantidadeProcessosIniciados, Fila** estadoPronto, Fila* estadoBloqueado);
                             
int cpuLivre(CPU* cpu);
void zeraCPU(CPU* cpu);

/* -------------- Instruceos de programa que sao processadas na CPU -------------- */

int* instrucaoTipoN(int n);
void instrucaoTipoD(int x, int *vetorVariaveis);
void instrucaoTipoV(int x, int n, int *vetorVariaveis);
void instrucaoTipoA(int x, int n, int *vetorVariaveis);
void instrucaoTipoS(int x, int n, int *vetorVariaveis);
void instrucaoTipoB(int n, int* pidProcessoAtual, Lista* tabelaProcessos, Fila* estadoBloqueado);
void instrucaoTipoT(int* pidProcessoAtual, Lista* tabelaProcessos);
void instrucaoTipoF(int n, int* pidProcessoAtual, int* pcProcessoAtual, int* quantidadeProcessosIniciados, int tempoAtualSistema, Lista* tabelaProcessos, Fila** filaPronto);
void instrucaoTipoR(char *nomeDoArquivo, Instrucao** arrPrograma, int* pcProcessoAtual);


#endif 