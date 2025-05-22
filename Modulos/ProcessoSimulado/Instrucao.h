#ifndef INSTRUCAO_H
#define INSTRUCAO_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../Menu/Menu.h"

#define TEXTOVAZIO "#######" // Define um texto vazio para inicialização
#define BUFFER 100 // Tam maximo do buffer
#define MAXINSTRUCOES 100 // Número máximo de instrucoes
#define NUMEROVAZIO -1 // Define um valor vazio para parâmetros numéricos

// Intrucao do processo simulado
typedef struct Instrucao {
    char tipoInstrucao; // Letra que representa a instrução
    char paramTexto[BUFFER]; // guarda nomes de arquivos ou outros parametros
    int paramNum1; // Parametros numericos 
    int paramNum2;
    
} Instrucao;


void inicializaInstrucao(char* instrucaoLida, Instrucao* instrucao);
void leInstrucoesArquivo(char* caminhoArq, Instrucao** programa);
void copiaInstrucao(Instrucao* novaInstrucao, Instrucao* instrucaoBase);
void imprimeInstrucao(Instrucao Instrucao, int apontadorInst);
void imprimeVetorPrograma(Instrucao* vetorPrograma, int pc); // Imprime todas as instrucoes do programa a partir do pc (contador)

#endif