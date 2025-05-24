#include "./Modulos/ProcessoControle/ProcessoControle.h"
#include "./Modulos/ProcessoImpressao/ProcessoImpressao.h"
#include <sys/wait.h>

/*
 * ANÁLISE DO SISTEMA DE GERENCIAMENTO DE PROCESSOS
 * 
 * Este programa implementa um sistema de comunicação entre processos usando:
 * - fork() para criar processo pai e filho
 * - pipes para comunicação bidirecional
 * - Sincronização entre processos
 */

int main(int argc, char **argv) {
    // ========== INICIALIZAÇÃO ==========
    int fd[2];       // Pipe principal: pai -> filho (comandos)
    int syncPipe[2]; // Pipe sincronização: filho -> pai (confirmações)
    char comando = '!';
    FILE *arquivoDeEntrada;
    int numCPUs = atoi(argv[1]);

    printf("Numero de CPUs: %d\n", numCPUs);
    
    // Inicializa estrutura de gerenciamento
    GerenciadorProcessos *gerenciador = inicializaGerenciador(numCPUs);
    printf("Gerenciador de processos inicializado\n");
    
    // Menu inicial retorna opção escolhida pelo usuário
    int opcao = MenuInicial(&arquivoDeEntrada);

    // Criação dos pipes de comunicação
    if (pipe(fd) == -1 || pipe(syncPipe) == -1){
        perror("Erro ao criar os pipes");
        return 1;
    }

    // ========== FORK - CRIAÇÃO DO PROCESSO FILHO ==========
    pid_t pid = fork();

    if (pid == -1){
        perror("Erro ao criar o processo");
        return 1;
    }

    // ========== PROCESSO PAI ==========
    if (pid > 0){
        printf("Processo pai\n");
        close(syncPipe[1]); // Fecha escrita do pipe de sincronização
        
        while (1) {
            if (opcao == 2) {
                // Modo arquivo: lê comandos automaticamente
                comando = controle(arquivoDeEntrada, opcao);
                escreverCaracterePipe(fd[1], comando);
            }
            else {
                // Modo interativo: usuário digita comandos
                scanf(" %c", &comando);
                escreverCaracterePipe(fd[1], comando);
                
                // Sincronização especial para comando de impressão
                if (comando == 'I') {
                    char confirmacao;
                    // ← PAI FICA BLOQUEADO AQUI esperando o 'D'
                    read(syncPipe[0], &confirmacao, sizeof(char));
                    
                    /* 
                     * Quando recebe o 'D', o PAI sabe que:
                     * "A impressão terminou, posso continuar!"
                     */
                    printf("Impressão concluída!\n");
                }
            }
            
            // Comando de saída
            if (comando == 'M') {
                break;
            }
        }
        
        // Aguarda término do processo filho
        wait(NULL);
    }
    
    // ========== PROCESSO FILHO ==========
    else {
        printf("Processo filho\n");
        close(syncPipe[0]); // Fecha leitura do pipe de sincronização
        
        while (1) {
            // Recebe comando do processo pai
            comando = lerCaracterePipe(fd[0]);
            
            if (comando == 'U') {
                // Comando de atualização/gerenciamento
                gerenciadorProcessos(gerenciador, comando);
            }
            else if (comando == 'I') {
                // Comando de impressão
                if (opcao == 2) {
                    // Modo arquivo: impressão simples
                    impressaoArquivo(gerenciador);
                }
                else {
                    // Modo interativo: cria processo específico para impressão
                    pid_t pidImpressao = fork();
                    
                    if (pidImpressao < 0) {
                        printf("ERRO NO FORK() IMPRESSAO\n");
                    }
                    else if (pidImpressao == 0) {
                        // Processo neto: executa impressão
                        ImprimeGerenciadorProcessos(gerenciador);
                        exit(0);
                    }
                    else {
                        // Processo filho: aguarda impressão e confirma
                        wait(NULL); // ← ESPERA o processo de impressão terminar
                        
                        // ENVIA SINAL 'D' = "DONE" (TERMINEI)
                        char confirmacao = 'D'; // 'D' de Done
                        write(syncPipe[1], &confirmacao, sizeof(char));
                        
                        /* 
                         * O 'D' é enviado pelo FILHO para o PAI dizendo:
                         * "Oi pai, a impressão que você pediu JÁ TERMINOU!"
                         */
                    }
                }
            }
            
            // Comando de saída
            if (comando == 'M') {
                break;
            }
        }
    }
    
    return 0;
}

/*
 * FLUXO DE COMUNICAÇÃO:
 * 
 * 1. PAI -> FILHO (fd[1] -> fd[0]):
 *    - Comandos ('U', 'I', 'M')
 *    - Controle de operações
 * 
 * 2. FILHO -> PAI (syncPipe[1] -> syncPipe[0]):
 *    - Confirmações de operações concluídas
 *    - Sincronização de processos
 * 
 * COMANDOS PRINCIPAIS:
 * - 'U': Atualização/gerenciamento de processos
 * - 'I': Impressão do estado atual
 * - 'M': Encerramento do programa
 * 
 * MODOS DE OPERAÇÃO:
 * - opcao == 2: Leitura automática de arquivo
 * - opcao != 2: Modo interativo com entrada manual
 */