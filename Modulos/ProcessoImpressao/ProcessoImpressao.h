#ifndef PROCESSO_IMPRESSAO_H
#define PROCESSO_IMPRESSAO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../GerenciadorProcessos/GerenciadorProcessos.h"


int numeroVariaveisProcesso(Instrucao *vetorPrograma);
void impressaoArquivo(GerenciadorProcessos *gerenciador);
void ImprimeEstadoAtualSistema(GerenciadorProcessos *gerenciador);
void ImprimeGerenciadorProcessos(GerenciadorProcessos *gerenciador);
void imprimeInfosGeraisProcesso(ProcessoSimulado *processo);
void imprimeVariaveisProcesso(int *vetorVariaveis, int tamanho);
void imprimeEstadoProcesso(Estado estado);
void imprimeCPUs(GerenciadorProcessos *gerenciador);
void imprimeCPU(CPU *cpu);
void imprimeTabelaProcessos(GerenciadorProcessos *gerenciador);

#endif // PROCESSO_IMPRESSAO_H