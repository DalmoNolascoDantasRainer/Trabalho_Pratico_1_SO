#include "Menu.h"

int MenuInicial(FILE **arquivoDeEntrada){
    int opcao;
    do{
        printf("\n\n=========== MENU ===========");
        printf("\n1) Entrada padrão");
        printf("\n2) Entrada por arquivo");
        printf("\n>> Escolha a opção: ");
        scanf("%d", &opcao);
        if (opcao == 1){
            printf("\n\n=================================================================");
            printf("\nEntre com um dos comandos a seguir para:\n");
            printf("U) Indicar o fim de uma unidade de tempo;\n");
            printf("I) Executar o processo de impressão;\n");
            printf("M) Imprimir o tempo médio de resposta e finalizar o sistema.");
            printf("\n=================================================================\n");
        }
        else if (opcao == 2){
            *arquivoDeEntrada = abreArquivoRead("./data/comandosControle");
        }
        else{
            printf("Opção inválida! Tente novamente.\n");
        }

    } while (opcao != 1 && opcao != 2);

    return opcao;
}

// Nova função para seleção do algoritmo de escalonamento
int MenuEscalonamento() {
    int opcaoEscalonamento;
    
    do {
        printf("\n\n========== ALGORITMO DE ESCALONAMENTO ==========");
        printf("\n1) Filas Múltiplas com Prioridade (Padrão)");
        printf("\n2) Round Robin");
        printf("\n>> Escolha o algoritmo de escalonamento: ");
        scanf("%d", &opcaoEscalonamento);
        
        if (opcaoEscalonamento == 1) {
            printf("\n✓ Algoritmo selecionado: Filas Múltiplas com Prioridade");
            printf("\n  - Utiliza múltiplas filas com diferentes prioridades");
            printf("\n  - Quantum varia por prioridade (2^prioridade)");
            printf("\n  - Processos podem ter prioridade rebaixada\n");
        }
        else if (opcaoEscalonamento == 2) {
            printf("\n✓ Algoritmo selecionado: Round Robin");
            printf("\n  - Utiliza uma única fila circular");
            printf("\n  - Quantum fixo de 3 unidades de tempo");
            printf("\n  - Todos os processos têm a mesma prioridade\n");
        }
        else {
            printf("Opção inválida! Tente novamente.\n");
        }
        
    } while (opcaoEscalonamento != 1 && opcaoEscalonamento != 2);

    return opcaoEscalonamento;
}

FILE *abreArquivoRead(char *nomeArquivo){
    FILE *fptr;

    fptr = fopen(nomeArquivo, "r");

    if (fptr == NULL){
        printf("\nERRO: Arquivo nao encontrado!\nPor favor, insira um arquivo valido!\n");
        return NULL;
    }
    else{
        return fptr;
    }
}