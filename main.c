#include "./Modulos/ProcessoControle/ProcessoControle.h"
#include "./Modulos/ProcessoImpressao/ProcessoImpressao.h"
#include "./Modulos/ProcessoImpressao/ProcessoImpressaoRR.h"
#include <unistd.h> 
#include <sys/wait.h>

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

int SelecaoEntrada(FILE **arquivoDeEntrada){
    int opcao = 0 ;
    while (opcao == 0){
        printf("╔══════════════════════════════╗\n");
        printf("║      OPÇÕES DE ENTRADA:      ║\n");
        printf("╠══════════════════════════════╣\n");
        printf("║ 1) Entrada pelo terminal     ║\n");
        printf("║ 2) Entrada por arquivo       ║\n");
        printf("║ 3) Sair                      ║\n");
        printf("╚══════════════════════════════╝\n");
        //printf("\n1) Entrada padrão");
        //printf("\n2) Entrada por arquivo");
        //printf("\n>> Escolha a opção: ");
        printf("\x1b[32m >> \x1b[0m Escolha a opção de entrada: ");
        scanf("%d", &opcao);
        printf("\n");
        if (opcao == 1){
            printf("╔════════════════════════════════════════════════════════════════════════════════════╗\n");
            printf("║                  ENTRE COM UM DOS COMANDOS A SEGUIR:                               ║\n");
            printf("╠════════════════════════════════════════════════════════════════════════════════════╣\n");
            printf("║ U) Fim de uma unidade de tempo                                                     ║\n");
            printf("║ I) Executar o processo de impressão                                                ║\n");
            printf("║ M) Imprimir o tempo médio de resposta e finalizar o sistema                        ║\n");
            printf("╚════════════════════════════════════════════════════════════════════════════════════╝\n");
        }
        else if (opcao == 2){
            *arquivoDeEntrada = LerArquivo("./arquivos/comandos");
        }
        else{
            printf("\n Opção inválida :( \n");
            printf(" Tente novamente!! \n");
            opcao = 0;
            sleep(2);
            system("clear");
        }
    }

    return opcao;
}

// Nova função para seleção do algoritmo de escalonamento
int SelecaoEscalonamento() {
    int opcaoEscalonamento = 0 ;

    printf("╔════════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                                \x1b[32m  🖥️​⚙️​  BEM-VINDO ;) \x1b[0m                                 ║\n");
    printf("╚════════════════════════════════════════════════════════════════════════════════════╝\n");

    while (opcaoEscalonamento == 0){
        printf("╔════════════════════════════════════════════════════════════════════════════════════╗\n");
        printf("║                        ALGORITMO DE ESCALONAMENTO                                  ║\n");
        printf("╠════════════════════════════════════════════════════════════════════════════════════╣\n");
        printf("║ 1) Filas Múltiplas com Prioridade - Quantum (2^prioridade)                         ║\n");
        printf("║ 2) Round Robin - Quantum fixo (3U)                                                 ║\n");
        printf("╚════════════════════════════════════════════════════════════════════════════════════╝\n");
        printf("\x1b[32m >> \x1b[0m Escolha o algoritmo de escalonamento: ");
        scanf("%d", &opcaoEscalonamento);
        printf("\n");
        
        if (opcaoEscalonamento != 1 && opcaoEscalonamento != 2 ) {
            printf("\n Opção inválida :( \n");
            printf(" Tente novamente!! \n");
            opcaoEscalonamento = 0;
            sleep(2); 
            system("clear");
        }
    };

    return opcaoEscalonamento;
}

/*
 * ANALISE DO SISTEMA DE GERENCIAMENTO DE PROCESSOS
 * 
 * comunicaçao entre processos usando:
 * - fork() para criar processo pai e filho
 * - pipes para comunicaçao bidirecional
 * - Sincronizaçao entre processos
 * - Seleçao de algoritmos de escalonamento
 */

int main(int argc, char **argv) {
    (void)argc;
    // ========== INICIALIZAÇÃO ==========
    int fd[2];       // Pipe principal: pai -> filho (comandos)
    int syncPipe[2]; // Pipe sincronização: filho -> pai (confirmações)
    char comando = '!';
    FILE *arquivoDeEntrada;
    int numCPUs = atoi(argv[1]);

    printf("Numero de CPUs: %d\n", numCPUs);
    
    // Inicializa estrutura de gerenciamento
    GerenciadorProcessos *gerenciador = inicializaGerenciador(numCPUs);

    GerenciadorProcessosRR *gerenciadorRR = inicializaGerenciadorRR(numCPUs);

    // ADICIONADO: Menu para seleção do algoritmo de escalonamento
    int opcaoEscalonamento = SelecaoEscalonamento();
    
    // Menu inicial retorna opção escolhida pelo usuário
    int opcao = SelecaoEntrada(&arquivoDeEntrada);
    
    
    // algoritmo de escalonamento baseado na escolha do usuário
    if (opcaoEscalonamento == 1) {
        // Filas Múltiplas (comportamento padrão - não precisa alterar nada)
        //defineEscalonamento(gerenciador, ESC_FILAS_MULTIPLAS);

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
                    if (comando == 'I') {
                        char confirmacao;
                        read(syncPipe[0], &confirmacao, sizeof(char));
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
            close(syncPipe[0]); // Fecha leitura do pipe de sincronização
            
            while (1) {
                // Recebe comando do processo pai
                comando = lerCaracterePipe(fd[0]);
                
                if (comando == 'U') {
                    // Comando de atualização/gerenciamento
                    gerenciadorProcessos(gerenciador, comando);
                    
                    // ADICIONADO: Feedback visual do algoritmo em execução (opcional)

                    printf("⏰ [FM] Tempo: %d\n", gerenciador->tempo);
                }
                else if (comando == 'I') {
                    // Comando de impressão
                    if (opcao == 2) {
                        // Modo arquivo: impressão simples
                        printf("\n📋 Gerando relatório do sistema...\n");
                        impressaoArquivo(gerenciador);
                    }
                    else {
                        // Modo interativo: cria processo específico para impressão
                        pid_t pidImpressao = fork();
                        
                        if (pidImpressao < 0) {
                            printf("ERRO NO FORK() IMPRESSAO\n");
                        }
                        else if (pidImpressao == 0) {
                            printf("\n📋 Gerando relatório do sistema...\n");
                            ImprimeGerenciadorProcessos(gerenciador);
                            exit(0);
                        }
                        else {
                            // Processo filho: aguarda impressão e confirma
                            wait(NULL); // ← ESPERA o processo de impressão terminar
                            
                            // ENVIA SINAL 'D' = "DONE" (TERMINEI)
                            char confirmacao = 'D'; // 'D' de Done
                            write(syncPipe[1], &confirmacao, sizeof(char));
                            
                        }
                    }
                }
                
                // Comando de saída
                if (comando == 'M') {
                    printf("🏁 Encerrando processo de escalonamento\n");
                    break;
                }
            }
        }
        
        printf("\n✅ Sistema finalizado com sucesso!\n");


    } else {
        // Round Robin

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
            close(syncPipe[0]); // Fecha leitura do pipe de sincronização
            
            // ADICIONADO: Exibe informações sobre o algoritmo em uso
        
            printf("🔄 Executando com Round Robin - Quantum: 3 unidades\n");
           
            
            while (1) {
                // Recebe comando do processo pai
                comando = lerCaracterePipe(fd[0]);
                
                if (comando == 'U') {
                    // Comando de atualização/gerenciamento
                    gerenciadorProcessosRR(gerenciadorRR, comando);
                    // ADICIONADO: Feedback visual do algoritmo em execução (opcional)
                    
                    printf("⏰ [RR] Tempo: %d\n", gerenciadorRR->tempo);
                   
                }
                else if (comando == 'I') {
                    // Comando de impressão
                    if (opcao == 2) {
                        // Modo arquivo: impressão simples
                        printf("\n📋 Gerando relatório do sistema...\n");
                        impressaoArquivoRR(gerenciadorRR);
                    }
                    else {
                        // Modo interativo: cria processo específico para impressão
                        pid_t pidImpressao = fork();
                        
                        if (pidImpressao < 0) {
                            printf("ERRO NO FORK() IMPRESSAO\n");
                        }
                        else if (pidImpressao == 0) {
                            // Processo neto: executa impressão
                            printf("\n📋 Gerando relatório do sistema...\n");
                            ImprimeGerenciadorProcessosRR(gerenciadorRR);
                            exit(0);
                        }
                        else {
                            // Processo filho: aguarda impressão e confirma
                            wait(NULL); // ← ESPERA o processo de impressão terminar
                            char confirmacao = 'D'; // 'D' de Done
                            write(syncPipe[1], &confirmacao, sizeof(char));
                        }
                    }
                }
                
                // Comando de saída
                if (comando == 'M') {
                    printf("🏁 Encerrando processo de escalonamento\n");
                    break;
                }
            }
        }
        
        printf("\n✅ Sistema finalizado com sucesso!\n");
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
 * 
 * ALGORITMOS DE ESCALONAMENTO:
 * - opcaoEscalonamento == 1: Filas Múltiplas de Prioridade
 * - opcaoEscalonamento == 2: Round Robin (Quantum = 3)
 */