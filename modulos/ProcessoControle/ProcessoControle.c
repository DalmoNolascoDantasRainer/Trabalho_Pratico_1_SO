#include "ProcessoControle.h"

char controle(FILE *arquivoDeEntrada, int opcao){
    char linha[BUFFER];
    char comando;

    switch (opcao){
        case 1:
            scanf(" %c", &comando);
            break;

        case 2:
            if(fgets(linha, BUFFER, arquivoDeEntrada) != NULL){
                comando = linha[0];
            }
            break;

        default:
            break;
    }
    return comando;
}
