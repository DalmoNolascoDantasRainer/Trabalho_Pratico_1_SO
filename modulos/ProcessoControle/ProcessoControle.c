#include "ProcessoControle.h"

// Funcao que le um comando de acordo com a opcao escolhida
char controle(FILE *arquivoDeEntrada, int opcao){
    char linha[BUFFER]; // Vetor de caracteres para armazenar uma linha do arquivo
    char comando = '\0';  // Inicializa com valor neutro (para evitar erros)

    switch (opcao){
        case 1: // Opcao 1: Le comando do teclado (entrada padrao)
            scanf(" %c", &comando);
            break;

        case 2: // Opcao 2: Le comando de um arquivo
            if (fgets(linha, BUFFER, arquivoDeEntrada) != NULL) {
                comando = linha[0];
            } else {
                return EOF;  // Retorna EOF (-1) para indicar erro na leitura do arquivo
            }
            break;

        default:
            return '\0';  // Retorna caractere nulo para opcao invalida
            break;
    }
    return comando; 
}
