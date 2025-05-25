#ifndef PROCESSO_IMPRESSAO_H
#define PROCESSO_IMPRESSAO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../GerenciadorProcessos/GerenciadorProcessos.h"


void impressaoArquivo(GerenciadorProcessos *gerenciador);
void ImprimeGerenciadorProcessos(GerenciadorProcessos *gerenciador);
void imprimeCPUs(GerenciadorProcessos *gerenciador);
void imprimeCPU(CPU *cpu);
void imprimeTabelaProcessos(GerenciadorProcessos *gerenciador);

#endif // PROCESSO_IMPRESSAO_H