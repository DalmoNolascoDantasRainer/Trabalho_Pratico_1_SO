#include "Instrucao.h"

// Função que inicializa uma instrução a partir de uma string lida
void inicializaInstrucao(char *instrucaoLida, Instrucao* instrucao)
{
    char* token;

    // Divide a string em tokens separados por espaço
    token = strtok(instrucaoLida, " ");
    
    // Define o tipo da instrução e inicializa os parâmetros com valores padrão
    instrucao->tipoInstrucao = token[0];
    instrucao->paramNum1 = NUMEROVAZIO;
    instrucao->paramNum2 = NUMEROVAZIO;
    strcpy(instrucao->paramTexto, TEXTOVAZIO);

    // Lê os parâmetros restantes da instrução
    token = strtok(NULL, " ");
    while (token != NULL)
    {
        // Instruções que possuem apenas um parâmetro numérico
        if (instrucao->tipoInstrucao == 'N' ||
            instrucao->tipoInstrucao == 'D' ||
            instrucao->tipoInstrucao == 'F' ||
            instrucao->tipoInstrucao == 'B')
        {
            instrucao->paramNum1 = atoi(token); // Converte o parâmetro para inteiro
            break;
        }
        // Instruções que possuem dois parâmetros numéricos
        else if (instrucao->tipoInstrucao == 'V' ||
                 instrucao->tipoInstrucao == 'S' ||
                 instrucao->tipoInstrucao == 'A')
        {
            instrucao->paramNum1 = atoi(token); // Primeiro parâmetro
            token = strtok(NULL, " ");
            instrucao->paramNum2 = atoi(token); // Segundo parâmetro
            break;
        }
        // Instruções que possuem um parâmetro de texto
        else if (instrucao->tipoInstrucao == 'R')
        {
            strcpy(instrucao->paramTexto, token); // Copia o texto
            break;
        }
        // Instrução de término (sem parâmetros)
        else if (instrucao->tipoInstrucao == 'T')
        {
            break;
        }
        // Caso de erro (tipo de instrução inválido)
        else
        {
            printf("\nErro");
            break;
        }
    }
}

// Função que lê as instruções de um arquivo e as armazena em um vetor
void leInstrucoesArquivo(char* caminhoArq, Instrucao** vetorPrograma)
{
    int i = 0;

    // Aloca memória para o vetor de instruções
    Instrucao* vetorInstrucoes = (Instrucao*) malloc(MAXINSTRUCOES * sizeof(Instrucao));
    char* linha = malloc(BUFFER * sizeof(char));

    // Abre o arquivo para leitura
    FILE* arquivoDeEntrada = abreArquivoRead(caminhoArq);

    // Lê cada linha do arquivo e inicializa as instruções
    while (fgets(linha, BUFFER, arquivoDeEntrada) != NULL)
    {   
        linha[strcspn(linha, "\n")] = '\0'; // Remove o caractere de nova linha
        inicializaInstrucao(linha, vetorInstrucoes + i); // Inicializa a instrução
        i++;
    }

    *vetorPrograma = vetorInstrucoes; // Define o vetor de instruções
}

// Função que copia uma instrução para outra
void copiaInstrucao(Instrucao* novaInstrucao, Instrucao* instrucaoBase)
{
    novaInstrucao->tipoInstrucao = instrucaoBase->tipoInstrucao; // Copia o tipo
    novaInstrucao->paramNum1 = instrucaoBase->paramNum1; // Copia o primeiro parâmetro
    novaInstrucao->paramNum2 = instrucaoBase->paramNum2; // Copia o segundo parâmetro
    strcpy(novaInstrucao->paramTexto, instrucaoBase->paramTexto); // Copia o texto
}

// Função que imprime uma instrução
void imprimeInstucao(Instrucao instrucao, int apontadorInst)
{
    // Verifica se a instrução é a atual (indicada pelo apontador)
    if (apontadorInst == 1)
    {
        printf("->|Instrução: Tipo %c", instrucao.tipoInstrucao);
    } 
    else
    {
        printf("  |Instrução: Tipo %c", instrucao.tipoInstrucao);
    }
    
    // Imprime os parâmetros da instrução
    printf(" | Param 1 %3d", instrucao.paramNum1);
    printf(" | Param 2 %5d", instrucao.paramNum2);
    printf(" | Param txt %10s|\n", instrucao.paramTexto);
}

// Função que imprime o vetor de instruções de um programa
void imprimeVetorPrograma(Instrucao* vetorPrograma, int pc)
{
    int i = 0;
    printf("  +------------------------ Programa do processo ------------------------+\n");

    // Percorre o vetor de instruções até encontrar a instrução de término ('T')
    while (vetorPrograma[i-1].tipoInstrucao != 'T')
    {   
        // Verifica se a instrução atual é a apontada pelo PC
        if (i == pc)
        {
            imprimeInstucao(vetorPrograma[i], 1); // Imprime com destaque
        } 
        else 
        {
            imprimeInstucao(vetorPrograma[i], 0); // Imprime normalmente
        }
        
        i++;
    }
}