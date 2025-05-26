#include "./Modulos/ProcessoControle/ProcessoControle.h"
#include "./Modulos/ProcessoImpressao/ProcessoImpressao.h"
#include "./Modulos/ProcessoImpressao/ProcessoImpressaoRR.h"
#include <unistd.h> 
#include <sys/wait.h>



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
       
        printf("\x1b[32m >> \x1b[0m Escolha a opção de entrada: ");
    
        if (scanf("%d", &opcao) != 1) {
            
            opcao = 0;
            printf("\nEntrada inválida :( Digite um número!\n");

            // Limpa o buffer
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);

            sleep(2);
            system("clear");
            continue;
        } else {
            // Limpa o buffer mesmo se a leitura foi bem-sucedida
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);
        }
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
        else if (opcao == 3){
            printf("\nEncerrando o programa...\n");
            sleep(1);
            exit(0);
        }
        else{
            printf("\n Opção inválida :( \n");
            printf(" Tente novamente!! \n");
            printf("\x1b[32m >> \x1b[0m Escolha a opção de entrada: ");
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
    while (opcaoEscalonamento == 0){

        printf("╔════════════════════════════════════════════════════════════════════════════════════╗\n");
        printf("║                        ALGORITMO DE ESCALONAMENTO                                  ║\n");
        printf("╠════════════════════════════════════════════════════════════════════════════════════╣\n");
        printf("║ 1) Filas Múltiplas com Prioridade - Quantum (2^prioridade)                         ║\n");
        printf("║ 2) Round Robin - Quantum fixo (3U)                                                 ║\n");
        printf("╚════════════════════════════════════════════════════════════════════════════════════╝\n");
        printf("\x1b[32m >> \x1b[0m Escolha o algoritmo de escalonamento: ");
        
        if (scanf("%d", &opcaoEscalonamento) != 1) {
            // Falhou (ex: letra)
            opcaoEscalonamento = 0;
            printf("\nEntrada inválida :( Digite um número!\n");

            // Limpa o buffer
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);

            sleep(2);
            system("clear");
            continue;
        } else {
            // Limpa o buffer mesmo se a leitura foi bem-sucedida
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);
        }
        
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

int main(int argc, char **argv) {
    (void)argc;
    // ========== INICIALIZAÇÃO ==========
    int fd[2];       // Pipe principal: pai -> filho (comandos)
    int syncPipe[2]; // Pipe sincronização: filho -> pai (confirmações)
    char comando = '!';
    FILE *arquivoDeEntrada;
    int numCPUs = atoi(argv[1]);

    
    
    // Inicializa estrutura de gerenciamento
    GerenciadorProcessos *gerenciador = inicializaGerenciador(numCPUs);

    GerenciadorProcessosRR *gerenciadorRR = inicializaGerenciadorRR(numCPUs);
    printf("╔════════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                                \x1b[32m  🖥️​⚙️​  BEM-VINDO ;) \x1b[0m                                 ║\n");
    printf("╚════════════════════════════════════════════════════════════════════════════════════╝\n");
    printf("╔════════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                        Número de CPUs Utilizadas: %d                                ║\n", numCPUs);
    printf("╚════════════════════════════════════════════════════════════════════════════════════╝\n");

    
    
    int opcaoEscalonamento = SelecaoEscalonamento();
    
    
    // Menu inicial retorna opção escolhida pelo usuário
    int opcao = SelecaoEntrada(&arquivoDeEntrada);
    
    
    // algoritmo de escalonamento baseado na escolha do usuário
    if (opcaoEscalonamento == 1) {
        


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
                    imprimeTempoMedioResposta(gerenciador);
                    printf("🏁 Encerrando processo de escalonamento\n");
                    break;
                }
            }
        }
        
        


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
                    gerenciadorProcessosRR(gerenciadorRR, comando);
                    
                    
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
                    imprimeTempoMedioRespostaRR(gerenciadorRR);
                    printf("🏁 Encerrando processo de escalonamento\n");
                    break;
                }
            }
        }
        
    }


    return 0;
}
