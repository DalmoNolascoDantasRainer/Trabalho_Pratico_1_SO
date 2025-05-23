#ifndef TABELAPROCESSOS_H
#define TABELAPROCESSOS_H

#include <stdio.h>
#include <stdlib.h>
#include "../EstruturasDeDados/Lista.h"

void insereNaTabela(ProcessoSimulado* processo, Lista* lista);
void removeDaTabela(Lista* lista, int PID);
ProcessoSimulado* buscaProcesso(Lista* lista, int PID);
int maiorPIDTabela(Lista* lista);
void imprimeTabela(Lista* lista);

#endif