#ifndef SJF_H
#define SJF_H

#include "Lista.h"


ProcessoSimulado* encontraMenorTempo(Lista* tabelaProcessos); // Encontra o processo com menor tempo de execucao
int temProcessosPendentes(Lista* tabelaProcessos); // Verifica se ainda tem processos para ser executad
void sjf(Lista* tabelaProcessos); // funcao principal do alg
void executarSJF(Lista* tabelaProcessos); // executa e calcula as estatisticas do escalonador
void copiarProcessos(Lista* origem, Lista* destino); // copia os processos entre as listas
void calcularEstatisticasSJF(Lista* tabelaProcessos);

#endif
