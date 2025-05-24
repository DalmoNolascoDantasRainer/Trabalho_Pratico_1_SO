#ifndef MENU_H
#define MENU_H

#include <stdio.h>
#include <stdlib.h>

// Declarações das funções do menu
int MenuInicial(FILE **arquivoDeEntrada);
int MenuEscalonamento(); // Nova função para seleção de escalonamento
FILE *abreArquivoRead(char *nomeArquivo);

#endif // MENU_H