#include "ProcessoImpressaoRR.h" 


// Função que exibe o menu de impressão e executa as opções escolhidas
void ImprimeGerenciadorProcessosRR(GerenciadorProcessosRR *gerenciador) {
    int opcao = 0; // Varivel para armazenar a opcao do menu
    int PID; 
    //int opcaoProcesso = 0;
    ProcessoSimulado *processo; // Ponteiro para processo simulado

    // Loop até o usuário escolher sair (opção 8)
    while (opcao != 9){
       
        printf("╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗\n");
        printf("║                                                     MENU DE IMPRESSÃO                                                          ║\n");
        printf("╠════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣\n");
        printf("║ 1 - Estado atual do sistema                                                                                                    ║\n");
        printf("║ 2 - Detalhes de um processo                                                                                                    ║\n");
        printf("║ 3 - Processos em estado bloqueado                                                                                              ║\n");
        printf("║ 4 - Processos em estado pronto                                                                                                 ║\n");
        printf("║ 5 - Processos em estado de execução                                                                                            ║\n");
        printf("║ 6 - Quantidade de processos executados até o momento                                                                           ║\n");
        printf("║ 7 - Informações sobre a/as CPU no momento atual                                                                                ║\n");
        printf("║ 8 - Informações sobre a Tabela de Processos no momento atual                                                                   ║\n");
        printf("║ 9 - Sair da impressão                                                                                                          ║\n");
        printf("╚════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝\n");
        printf("\x1b[32m >> \x1b[0mEscolha uma opção: ");

        // Lê a opção do usuário
        if (scanf("%d", &opcao) != 1) {
            opcao = 0;
            printf("\nEntrada inválida :( Digite um número!\n");

            // Limpa o buffer
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);

            sleep(2);           // Espera um pouco
            system("clear");    // Limpa a tela (use "cls" no Windows)
            continue;           // Volta ao menu
        }

        // Switch para verificar qual opção foi escolhida
        switch (opcao) {
            case 1:
                // Imprime o estado atual do sistema
                // Imprime o cabeçalho do estado do sistema
                printf("\n");
                printf("╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗\n");
                printf("║                                        ESTADO DO SISTEMA - INFORMAÇÕES ATUAIS                                                  ║\n");
                printf("╠════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣\n");

                // Tempo de uso do sistema
                printf("║ Tempo de uso do sistema no momento atual: %3d                                                                                  ║\n", gerenciador->tempo);
                printf("║ Quantidade de processos em estado bloqueado: %3d                                                                               ║\n", gerenciador->estadoBloqueado->Tamanho + 1);
                printf("║ Quantidade de processos em estado pronto: %3d                                                                                  ║\n", gerenciador->filaRoundRobin->Tamanho + 1);
                //printf("║ Quantidade de processos em estados de execução: %3d                                                                            ║\n", gerenciador->estadoPronto->Tamanho + 1);
                // Chamada para função que imprime as informações das CPUs (essa função deve manter o estilo visual também)
                imprimeCPUsRR(gerenciador);
               
                printf("╚════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝\n");
                break;


            case 2:
                // Imprime o tempo de uso do sistema
                // Pergunta se deseja detalhes de algum processo
                    imprimePIDProcessosRR(gerenciador);
                    
                    // Lê o PID do processo desejado
                    printf("\nInforme o PID do processo: ");
                    
                    if (scanf(" %d%*[^\n]", &PID) != 1) {
                        printf("\nEntrada inválida :( Digite um número!\n");

                        // Limpa o buffer
                        int ch;
                        while ((ch = getchar()) != '\n' && ch != EOF);

                        sleep(2);
                        system("clear");
                        break;  // Ou use continue, dependendo de onde está inserido
                    }

                    // Busca o processo
                    processo = buscaProcesso(gerenciador->tabelaProcessos, PID);
                    if (processo == NULL) {
                        printf("\nProcesso com PID %d não encontrado!\n", PID);
                        sleep(2);
                        system("clear");
                        break;  // Ou continue, dependendo do seu loop
                    }
                    
                    printf("\n\n");
                    // Imprime informações detalhadas do processo
                    printf("╔════════╦══════════╦══════╦════════════╦════════════════╦════════════════╦═══════════════╗\n");
                    printf("║  PID   ║ PID PAI  ║  PC  ║ Prioridade ║    Estado      ║ Tempo de Início║ Tempo de CPU  ║\n");
                    printf("╠════════╬══════════╬══════╬════════════╬════════════════╬════════════════╬═══════════════╣\n");
                    imprimeProcesso(*processo, 1);
                    if (processo->vetorVariaveis == NULL){
                    }
                    printf("╔══════════════════════════════════════════════════════════════════╗\n");
                    imprimeVariaveis(processo->vetorVariaveis, numeroVariaveis(*processo->conjuntoInstrucoes));
                    printf("\n╚══════════════════════════════════════════════════════════════════╝\n");
                    
                    
                    imprimeVetorPrograma(*processo->conjuntoInstrucoes, *processo->pc);
                    
                
                break;

            case 3:
                // Imprime os processos em estado bloqueado
                printf("\n\nProcessos em estado bloqueado:");

                // Verifica se a fila está vazia
                if (filaEhVazia(gerenciador->estadoBloqueado)){
                    printf("\n   Fila Vazia!\n");
                }
                else{
                    // Percorre a fila de bloqueados e imprime informações de cada processo
                    ElementoPidTempo *celula = gerenciador->estadoBloqueado->Frente;
                    while (celula != NULL)
                    {
                        processo = buscaProcesso(gerenciador->tabelaProcessos, celula->pidTempo.pid);
                        printf("\n");
                        imprimeProcesso(*processo, 1);
                        celula = celula->Prox;
                    }
                    putchar('\n');
                }
                printf("Há %d processos bloqueados na fila\n", gerenciador->estadoBloqueado->Tamanho);
                break;

            case 4:
                // Imprime os processos em estado pronto
                printf("\n\n                             PROCESSOS EM ESTADO PRONTO:                ");
                printf("\n");

                Fila *fila = gerenciador->filaRoundRobin;

                    // Verifica se a fila está vazia
                    if (filaEhVazia(fila)){
                        printf("\n   Nao existem processos em estado pronto \n\n");
                    }
                    else{
                        // Percorre a fila e imprime informações de cada processo 
                        ElementoPidTempo *celula = fila->Frente;
                            printf("╔════════╦══════════╦══════╦════════════╦════════════════╦═══════════════╗\n");
                            printf("║  PID   ║ PID PAI  ║  PC  ║  Estado    ║Tempo de Início ║ Tempo de CPU  ║\n");
                            printf("╠════════╬══════════╬══════╬════════════╬════════════════╬═══════════════╣\n");
                        while (celula != NULL){
                            processo = buscaProcesso(gerenciador->tabelaProcessos, celula->pidTempo.pid);
                            imprimeProcessoRR(*processo,1);

                            celula = celula->Prox;
                        }
                        putchar('\n');
                    }
                break;

           case 5:
                printf("\n\nProcessos em estado de execução (rodando nas CPUs):\n");

                bool algum = false;
                for (int i = 0; i < gerenciador->numCPUs; i++) {
                    CPU_RR *cpu = gerenciador->cpus[i];

                    // Se a CPU está ocupada, busca e imprime o processo
                    if (!cpuLivreRR(cpu)) {
                        ProcessoSimulado *proc = buscaProcesso(gerenciador->tabelaProcessos, cpu->pidProcessoAtual);
                        if (proc) {
                            printf("\nCPU %d:\n", i);
                            printf("╔════════╦══════════╦══════╦════════════╦════════════════╦═══════════════╗\n");
                            printf("║  PID   ║ PID PAI  ║  PC  ║  Estado    ║ Tempo de Início║ Tempo de CPU  ║\n");
                            printf("╠════════╬══════════╬══════╬════════════╬════════════════╬═══════════════╣\n");
                            imprimeProcessoRR(*proc, 1);  // ou sem opção, se você já simplificou
                            algum = true;
                        }
                    }
                }

                if (!algum) {
                    printf("   Não existem processos em execução no momento.\n");
                }
                break;




            case 6:
                 // Imprime a quantidade de processos executados até o momento
                printf("\n\nQuantidade de processos executados até o momento: %d\n\n", gerenciador->quantidadeProcessosIniciados);
                break;
            case 7:
                // Imprime informações das CPUs
                imprimeCPUsRR(gerenciador);
                break;

            case 8:
                /// Imprime a tabela de processos
                imprimeTabelaProcessosRR(gerenciador);
                printf("\n");
                break;
            case 9:
                // Sair do programa
                printf("\nSaindo do menu de impressão...\n");
                break;

            default:
                // Opção inválida
                
                printf("\nOpção inválida! Por favor, escolha uma opção entre 1 e 8.\n");
                break;
        }
    }
}


// Função que imprime o estado do sistema em formato de arquivo
void impressaoArquivoRR(GerenciadorProcessosRR *gerenciador) {
    // Imprime o cabeçalho do estado do sistema
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                                        ESTADO DO SISTEMA - INFORMAÇÕES ATUAIS                                                  ║\n");
    printf("╠════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣\n");

    
    // Imprime o tempo de uso do sistema
    printf("\n\x1b[32m >> \x1b[0mTempo de uso atual do sistema em unidade de tempo: %d", gerenciador->tempo);
    // Imprime os processos bloqueados
    printf("\n\n\x1b[32m >> \x1b[0mProcessos em estado BLOQUEADO:\n\n");
    imprimeFila(gerenciador->estadoBloqueado);
    // Imprime os processos prontos
    printf("\n\n\x1b[32m >> \x1b[0mProcessos em estado PRONTO:\n");
    imprimeFila(gerenciador->filaRoundRobin);
    // Imprime a quantidade de processos executados
    printf("\n\x1b[32m >> \x1b[0mQuantidade de processos executados até o momento: %d\n\n", gerenciador->quantidadeProcessosIniciados);
    // Imprime informações das CPUs
    imprimeCPUsRR(gerenciador);
    // Imprime a tabela de processos
    imprimeTabelaProcessosRR(gerenciador);
    // Imprime o rodapé
    printf("╘═══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╛\n");
}

// Função que imprime informações de todas as CPUs
void imprimeCPUsRR(GerenciadorProcessosRR *gerenciador) {
    // Percorre todas as CPUs
    for (int i = 0; i < gerenciador->numCPUs; i++)
    {
        // Imprime o cabeçalho da CPU
        printf("╠════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣\n");
        printf("║                                                    CPU %2d                                                                      ║\n", i);
        printf("╠════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣\n");
        


        // Verifica se a CPU está livre
        if (cpuLivreRR(gerenciador->cpus[i])){
            printf("║ Status: CPU LIVRE                                                                                                              ║\n");
        }
        else{
            // Imprime informações da CPU ocupada
            imprimeCPURR(gerenciador->cpus[i]);
        }

        // Final da caixa de visualização da CPU
        ;
    }
}


// Função que imprime informações de uma CPU específica
void imprimeCPURR(CPU_RR *cpu) {
    // Imprime informações da CPU ocupada com formatação em caixa
    printf("║      Processo em execução - PID %3d                                                                                            ║\n",
         cpu->pidProcessoAtual); // Imprime o PID do processo atual
    printf("║      PC: %3d                                                                                                                   ║\n",
         cpu->pcProcessoAtual); // Imprime o PC do processo atual
    printf("║      Fatia do quantum já executado: %3d                                                                                        ║\n",
         cpu->fatiaQuantum); // Imprime a fatia do quantum executada
    
    printf("╚════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝\n");

    imprimeVariaveis(*(cpu->variaveisProcessoAtual), numeroVariaveis(cpu->programaProcessoAtual)); // Imprime as variáveis do processo
    printf("\n");
}


void imprimeTabelaProcessosRR(GerenciadorProcessosRR *gerenciador) {
    // Imprime o cabeçalho da tabela de processos
    
    printf("╔════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                        TABELA DE PROCESSOS                             ║\n");


    // Ponteiro auxiliar para percorrer a lista de processos
    CelulaPtr aux;
    aux = gerenciador->tabelaProcessos->Primeiro->Prox;

    printf("╠════════╦══════════╦══════╦════════════╦════════════════╦═══════════════╠\n");
    printf("║  PID   ║ PID PAI  ║  PC  ║  Estado    ║Tempo de Início ║ Tempo de CPU  ║\n");
    printf("╠════════╬══════════╬══════╬════════════╬════════════════╬═══════════════╣\n");
    

    while (aux != NULL) {
        // Chama a funcao para imprimir o processo
        imprimeProcessoRR(*(aux->processo), 1);
        aux = aux->Prox; // Avanca para a proxima celula
    }
}

void imprimePIDProcessosRR(GerenciadorProcessosRR *gerenciador) {
    // Imprime o cabeçalho da tabela de processos
    
    printf("╔═════════════════════════╗\n");
    printf("║  PROCESSOS EXISTENTES   ║\n");
    printf("╠═════════════════════════╣\n");

    // Ponteiro auxiliar para percorrer a lista de processos
    CelulaPtr aux;
    aux = gerenciador->tabelaProcessos->Primeiro->Prox;

    printf("║           PID           ║ \n");
    printf("╠═════════════════════════╣\n");

    while (aux != NULL) {
        // Chama a funcao para imprimir o processo
        imprimePID(*(aux->processo));
        aux = aux->Prox; // Avanca para a proxima celula
    }

    printf("╚═════════════════════════╝\n");
}


void imprimeTempoMedioRespostaRR(GerenciadorProcessosRR *gerenciador) {
    if (gerenciador->quantidadeProcessosIniciados > 0) {
        double tempoMedio = (double)gerenciador->tempo / gerenciador->quantidadeProcessosIniciados;

        printf("\n");
        printf("╔════════════════════════════════════════════════╗\n");
        printf("║          RELATÓRIO DE DESEMPENHO FINAL         ║\n");
        printf("╠════════════════════════════════════════════════╣\n");
        printf("║ Tempo médio de resposta     │ %10.2f       ║\n", tempoMedio);
        printf("╟─────────────────────────────┼──────────────────╢\n");
        printf("║ Tempo total de execução     │ %10d       ║\n", gerenciador->tempo);
        printf("╟─────────────────────────────┼──────────────────╢\n");
        printf("║ Processos finalizados       │ %10d       ║\n", gerenciador->quantidadeProcessosIniciados);
        printf("╚════════════════════════════════════════════════╝\n\n");

    } else {
        printf("\n");
        printf("╔════════════════════════════════════╗\n");
        printf("║   Nenhum processo foi executado.  ║\n");
        printf("╚════════════════════════════════════╝\n\n");
    }
}