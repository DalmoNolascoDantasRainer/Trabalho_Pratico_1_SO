#ifndef PROCESSO_IMPRESSAORR_H
#define PROCESSO_IMPRESSAORR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../GerenciadorProcessos/GerenciadorProcessosRR.h"

void impressaoArquivoRR(GerenciadorProcessosRR *gerenciador);
void ImprimeGerenciadorProcessosRR(GerenciadorProcessosRR *gerenciador);
void imprimeCPUsRR(GerenciadorProcessosRR *gerenciador);
void imprimeCPURR(CPU_RR *cpu);
void imprimeTabelaProcessosRR(GerenciadorProcessosRR *gerenciador);
void imprimePIDProcessosRR(GerenciadorProcessosRR *gerenciador);
void imprimeTempoMedioRespostaRR(GerenciadorProcessosRR *gerenciador);

#endif // PROCESSO_IMPRESSAO_H