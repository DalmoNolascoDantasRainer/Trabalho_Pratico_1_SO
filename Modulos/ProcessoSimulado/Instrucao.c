#include "Instrucao.h"

FILE *LerArquivo(char *nomeArquivo) {
    FILE *ponteiro_arquivo = NULL;

    while (ponteiro_arquivo == NULL) {
        ponteiro_arquivo = fopen(nomeArquivo, "r");

        if (ponteiro_arquivo == NULL) {
            printf("\n\x1b[31m ERRO: \x1b[0m Arquivo não encontrado! :( \nPor favor, insira um arquivo válido!\n");
            printf("Digite o nome do arquivo novamente: ");
            scanf("%s", nomeArquivo);
        }
    }
    return ponteiro_arquivo;
}

// Funcao que inicializa uma instrcao a partir de uma string lida
void inicializaInstrucao(char *instrucaoLida, Instrucao* instrucao) {
    char* partes;

    // Divide a instrucao em partess separados por espaco
    partes = strtok(instrucaoLida, " ");
    
    // Define o tipo da instrucao e inicializa os parametros com valores padrao
    instrucao->tipoInstrucao = partes[0];
    instrucao->parametroNum1 = NUMEROVAZIO;
    instrucao->parametroNum2 = NUMEROVAZIO;
    strcpy(instrucao->paramTexto, TEXTOVAZIO);

    // Le os parametros restantes da instrucao
    partes = strtok(NULL, " ");
    while (partes != NULL) {
        // Instrucoes que possuem apenas um parametro numerico
        if (instrucao->tipoInstrucao == 'N' ||
            instrucao->tipoInstrucao == 'D' ||
            instrucao->tipoInstrucao == 'F' ||
            instrucao->tipoInstrucao == 'B'){
            instrucao->parametroNum1 = atoi(partes); // Converte o parametro para inteiro
            break;
        }

        // Instrucoes que possuem dois parametros numericos
        else if (instrucao->tipoInstrucao == 'V' ||
                 instrucao->tipoInstrucao == 'S' ||
                 instrucao->tipoInstrucao == 'A'){
            instrucao->parametroNum1 = atoi(partes); // Primeiro parametro
            partes = strtok(NULL, " ");
            instrucao->parametroNum2 = atoi(partes); // Segundo parametro
            break;
        }
        // Instrucoes que possuem um parametro de texto
        else if (instrucao->tipoInstrucao == 'R'){
            strcpy(instrucao->paramTexto, partes); // Copia o texto
            break;
        }

        // Instrucao de termino (sem parametros)
        else if (instrucao->tipoInstrucao == 'T') {
            break;
        }

        // Caso de erro (tipo de instrucao invalido)
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
    FILE* arquivoDeEntrada = LerArquivo(caminhoArq);

    // Le cada linha do arquivo e inicializa as instrucoes
    while (fgets(linha, BUFFER, arquivoDeEntrada) != NULL){   
        linha[strcspn(linha, "\n")] = '\0'; // Remove o caractere de nova linha
        inicializaInstrucao(linha, vetorInstrucoes + i); // Inicializa a instrcao
        i++;
    }

    *vetorPrograma = vetorInstrucoes; // Define o vetor de instrucoes
    
}

// Funcao que copia uma instrucao para outra
void copiaInstrucao(Instrucao* novaInstrucao, Instrucao* instrucaoBase){
    
    novaInstrucao->tipoInstrucao = instrucaoBase->tipoInstrucao; 
    novaInstrucao->parametroNum1 = instrucaoBase->parametroNum1; 
    novaInstrucao->parametroNum2 = instrucaoBase->parametroNum2; 
    strcpy(novaInstrucao->paramTexto, instrucaoBase->paramTexto); 
}

// Funcao que imprime uma instrucao
void imprimeInstrucao(Instrucao instrucao, int instrucaoAtual){

    if (instrucaoAtual == 1){ // instrução atual
        printf("║\033[42;30m   %c    \033[0m║\033[42;30m     %10d     \033[0m║\033[42;30m    %10d     \033[0m║\033[42;30m  %-14s\033[0m║\n", 
                instrucao.tipoInstrucao, instrucao.parametroNum1, instrucao.parametroNum2, instrucao.paramTexto);
    } else {
        printf("║   %c    ║     %10d     ║    %10d     ║  %-14s║\n", 
               instrucao.tipoInstrucao, instrucao.parametroNum1, instrucao.parametroNum2, instrucao.paramTexto);
    }

    printf("╚════════╩════════════════════╩═══════════════════╩════════════════╝\n");
}


// Funcao que imprime o vetor de instrucoes de um programa
void imprimeVetorPrograma(Instrucao* vetorPrograma, int pc){
    printf("\n\n");
    printf("╔══════════════════════════════════════════════════════════════════╗\n");
    printf("║                      PROGRAMA DO PROCESSO                        ║\n");
    printf("║                                                                  ║\n");
    printf("╠════════╦════════════════════╦═══════════════════╦════════════════╠\n");
    printf("║  TIPO  ║    Parametro 1     ║    Parametro 2    ║  Parametro txt ║\n");
    printf("╠══════╬════════════════════╬═══════════════════╬════════════════╣\n");

    int i = 0;
    while (vetorPrograma[i-1].tipoInstrucao != 'T'){ // Percorre o vetor de instrucoes ate encontrar a instrucao de termino ('T')   
        if (i == pc){ // Verifica se a instrucao atual e a apontada pelo PC
            imprimeInstrucao(vetorPrograma[i], 1); // Imprime com destaque (atual)
        } 
        else {
            imprimeInstrucao(vetorPrograma[i], 0); // Imprime normalmente
        }
        i++;
    }
}