#include "Instrucao.h"

// funcao que inicializa uma instrcao a partir de uma string lida
void inicializaInstrucao(char *instrucaoLida, Instrucao* instrucao) {
    char* token;

    // Divide a string em tokens separados por espaco
    token = strtok(instrucaoLida, " ");
    
    // Define o tipo da instrucao e inicializa os parametros com valores padrao
    instrucao->tipoInstrucao = token[0];
    instrucao->parametroNum1 = NUMEROVAZIO;
    instrucao->parametroNum2 = NUMEROVAZIO;
    strcpy(instrucao->paramTexto, TEXTOVAZIO);

    // Le os parametros restantes da instrucao
    token = strtok(NULL, " ");
    while (token != NULL) {
        // Instrucoes que possuem apenas um parametro numerico
        if (instrucao->tipoInstrucao == 'N' ||
            instrucao->tipoInstrucao == 'D' ||
            instrucao->tipoInstrucao == 'F' ||
            instrucao->tipoInstrucao == 'B'){
            instrucao->parametroNum1 = atoi(token); // Converte o parametro para inteiro
            break;
        }
        // Instrucoes que possuem dois parametros numericos
        else if (instrucao->tipoInstrucao == 'V' ||
                 instrucao->tipoInstrucao == 'S' ||
                 instrucao->tipoInstrucao == 'A'){
            instrucao->parametroNum1 = atoi(token); // Primeiro parametro
            token = strtok(NULL, " ");
            instrucao->parametroNum2 = atoi(token); // Segundo parametro
            break;
        }
        // Instrucoes que possuem um parametro de texto
        else if (instrucao->tipoInstrucao == 'R'){
            strcpy(instrucao->paramTexto, token); // Copia o texto
            break;
        }
        // Instrucao de termino (sem parametros)
        else if (instrucao->tipoInstrucao == 'T') {
            break;
        }
        // Caso de erro (tipo de instrcao invalido)
        else{
            printf("\nErro");
            break;
        }
    }
}

// Funcao que le as instrucoes de um arquivo e as armazena em um vetor
void leInstrucoesArquivo(char* caminhoArq, Instrucao** vetorPrograma){
    int i = 0;

    // Aloca memoria para o vetor de instrucoes
    free(*(vetorPrograma));
    Instrucao* vetorInstrucoes = (Instrucao*) malloc(MAXINSTRUCOES * sizeof(Instrucao));
    char* linha = malloc(BUFFER * sizeof(char));

    // Abre o arquivo para leitura
    FILE* arquivoDeEntrada = abreArquivoRead(caminhoArq);

    // Le cada linha do arquivo e inicializa as instrucoes
    while (fgets(linha, BUFFER, arquivoDeEntrada) != NULL){   
        linha[strcspn(linha, "\n")] = '\0'; // Remove o caractere de nova linha
        inicializaInstrucao(linha, vetorInstrucoes + i); // Inicializa a instrcao
        i++;
    }

    *vetorPrograma = vetorInstrucoes; // Define o vetor de instrucoes
    
}

// funcao que copia uma instrucao para outra
void copiaInstrucao(Instrucao* novaInstrucao, Instrucao* instrucaoBase){
    
    novaInstrucao->tipoInstrucao = instrucaoBase->tipoInstrucao; // Copia o tipo
    novaInstrucao->parametroNum1 = instrucaoBase->parametroNum1; // Copia primeiro parametro
    novaInstrucao->parametroNum2 = instrucaoBase->parametroNum2; // Copiasegundo parametro
    strcpy(novaInstrucao->paramTexto, instrucaoBase->paramTexto); // Copia o texto
}

//  OLHAR DE PASSAR ISSO PARA PROCESSO IMPRESSAO
// funcao que imprime uma instrucao
void imprimeInstrucao(Instrucao instrucao, int apontadorInst){
    if (apontadorInst == 1){ // Verifica se a instrucao é a atual (indicada pelo apontador)
        printf("->|Instrucao do Tipo %c", instrucao.tipoInstrucao);
    } 
    else{
        printf("  |Instrucao do Tipo %c", instrucao.tipoInstrucao);
    }
    
    // Imprime os parametros da instrucao
    printf(" | Parametro 1 %3d", instrucao.parametroNum1);
    printf(" | Parametro 2 %5d", instrucao.parametroNum2);
    printf(" | Parametro txt %10s|\n", instrucao.paramTexto);
}


//  OLHAR DE PASSAR ISSO PARA PROCESSO IMPRESSAO
// funcao que imprime o vetor de instrucoes de um programa
void imprimeVetorPrograma(Instrucao* vetorPrograma, int pc){
    int i = 0;
    printf("  +------------------------ Programa do processo ------------------------+\n");
    while (vetorPrograma[i-1].tipoInstrucao != 'T'){ // Percorre o vetor de instrucoes ate encontrar a instrucao de termino ('T')   
        if (i == pc){ // Verifica se a instrucao atual e a apontada pelo PC
            imprimeInstrucao(vetorPrograma[i], 1); // Imprime com destaque
        } 
        else {
            imprimeInstrucao(vetorPrograma[i], 0); // Imprime normalmente
        }
        i++;
    }
}