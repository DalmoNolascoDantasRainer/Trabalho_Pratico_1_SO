#ifndef CPU_H
#define CPU_H


#include <stdio.h>
#include <stdlib.h>

#include "../EstruturaDeDados/Fila.h"
#include "../EstruturaDeDados/Lista.h"

typedef struct CPU{
    int *pidProcessoAtual;
    Instrucao **programaProcessoAtual;
    int *pcProcessoAtual;
    int **variaveisProcessoAtual;
    int fatiaQuantum;       //Quanto do "quantum" de tempo já foi usada pelo processo em execução
} CPU;

CPU* inicializaCPU();
void carregaProcesso(CPU* cpu, ProcessoSimulado* processoAtual);
void executaProxInstrucao(CPU* cpu, int tempoAtualSistema, Lista* tabelaProcessos,
                             int* quantidadeProcessosIniciados, Fila** estadoPronto, Fila* estadoBloqueado);
                             
int cpuLivre(CPU* cpu);
void zeraCPU(CPU* cpu);

/* -------------- Instruçẽos de programa que são processadas na CPU -------------- */

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