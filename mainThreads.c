#include "./Threads/Threads.h"
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
            // Falhou (ex: letra)
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
            *arquivoDeEntrada = LerArquivo("./arquivos/comandos"); // Abre o arquivo padrão de comandos
        }
        else if (opcao == 3){
            printf("\nEncerrando o programa...\n");
            sleep(1);
            exit(0); 
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


int main(int argc, char **argv) {
    if (argc < 2) { 
        printf("Uso: %s <numero_CPUs>\n", argv[0]);
        return 1;
    }
    
    int numCPUs = atoi(argv[1]); 
    FILE *arquivoDeEntrada;
    
    printf("=== SIMULADOR COM THREADS ===\n");
    printf("╔════════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                            SIMULADOR COM THREADS                                   ║\n");
    printf("╚════════════════════════════════════════════════════════════════════════════════════╝\n");
    printf("Numero de CPUs: %d\n", numCPUs);
    
    GerenciadorProcessos *gerenciador = inicializaGerenciador(numCPUs); 
    printf("Gerenciador de processos inicializado\n");
    
    int opcao = SelecaoEntrada(&arquivoDeEntrada); 
    

    DadosGlobais dados;
    dados.gerenciador = gerenciador; // Associa o gerenciador de processos
    dados.arquivo = arquivoDeEntrada; // Associa o arquivo de entrada (se houver)
    dados.opcao = opcao; 
    dados.numCPUs = numCPUs; 
    dados.sistema_ativo = 1; // Marca o sistema como ativo
    
    // Inicializacao do buffer de comandos
    BufferComando buffer;
    buffer.comando_disponivel = 0; 
    buffer.comando_processado = 0; 
    pthread_mutex_init(&buffer.mutex, NULL); 
    pthread_cond_init(&buffer.cond_comando, NULL); 
    pthread_cond_init(&buffer.cond_processado, NULL);
    dados.buffer = &buffer; // Associa o buffer aos dados globais
    
    // Inicialização da sincronizacao
    pthread_mutex_init(&dados.mutex_gerenciador, NULL); 
    sem_init(&dados.sem_impressao, 0, 1); // Inicializa semaforo para impressao
    
    // Criação das threads
    pthread_t tid_controle, tid_gerenciador;
    
    if (pthread_create(&tid_controle, NULL, thread_controle, &dados) != 0) { // Cria thread de controle
        printf("ERRO: Falha ao criar thread controle :(\n");
        return 1;
    }
    
    if (pthread_create(&tid_gerenciador, NULL, thread_gerenciador, &dados) != 0) { // Cria thread do gerenciador
        printf("ERRO: Falha ao criar thread gerenciador :(\n");
        return 1;
    }
    
    printf("Sistema em execução!\n\n");
    
    // Aguarda finalizacao das threads
    pthread_join(tid_controle, NULL); // Espera a thread de controle terminar
    pthread_join(tid_gerenciador, NULL); // Espera a thread do gerenciador terminar
    
    // Limpeza dos recursos
    pthread_mutex_destroy(&buffer.mutex); // Destroi mutex do buffer
    pthread_cond_destroy(&buffer.cond_comando); // Destroi condicao de comando
    pthread_cond_destroy(&buffer.cond_processado); // Destroi condicao de processado
    pthread_mutex_destroy(&dados.mutex_gerenciador); // Destroi mutex do gerenciador
    sem_destroy(&dados.sem_impressao); // Destroi semaforo de impressao
    
    if (opcao == 2 && arquivoDeEntrada) { // Se usou arquivo, fecha o arquivo
        fclose(arquivoDeEntrada);
    }
    imprimeTempoMedioResposta(gerenciador);
    printf("Sistema finalizado!\n");
    return 0; 
}
