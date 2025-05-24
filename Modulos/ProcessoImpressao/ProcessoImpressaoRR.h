#ifndef PROCESSO_IMPRESSAORR_H
#define PROCESSO_IMPRESSAORR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../GerenciadorProcessos/GerenciadorProcessosRR.h"


int numeroVariaveisProcessoRR(Instrucao *vetorPrograma);
void impressaoArquivoRR(GerenciadorProcessosRR *gerenciador);
void ImprimeEstadoAtualSistemaRR(GerenciadorProcessosRR *gerenciador);
void ImprimeGerenciadorProcessosRR(GerenciadorProcessosRR *gerenciador);
void imprimeInfosGeraisProcessoRR(ProcessoSimulado *processo);
void imprimeVariaveisProcessoRR(int *vetorVariaveis, int tamanho);
void imprimeEstadoProcessoRR(Estado estado);
void imprimeCPUsRR(GerenciadorProcessosRR *gerenciador);
void imprimeCPURR(CPU_RR *cpu);
void imprimeTabelaProcessosRR(GerenciadorProcessosRR *gerenciador);

#endif // PROCESSO_IMPRESSAO_H