#include "ProcessoImpressao.h" // Inclui o cabeçalho com as definições necessárias

// Função que imprime o estado atual do sistema
void ImprimeEstadoAtualSistema(GerenciadorProcessos *gerenciador)
{
    // Imprime o cabeçalho do estado do sistema
    printf("\n\n╒══════════════════════════════════════════════════════╡ ESTADO DO SISTEMA ╞════════════════════════════════════════════════════════╕\n\n"); 
    // Imprime o tempo de uso do sistema
    printf("\n\nTempo de uso do sistema no momento atual: %d unidades de tempo\n", gerenciador->tempo);
    // Imprime informações das CPUs
    imprimeCPUs(gerenciador);
}

// Função que exibe o menu de impressão e executa as opções escolhidas
void ImprimeGerenciadorProcessos(GerenciadorProcessos *gerenciador) 
{
    int opcao = 0; // Variável para armazenar a opção do menu
    int PID; // Variável para armazenar o PID do processo
    int opcaoProcesso = 0; // Variável para opção de detalhes do processo
    ProcessoSimulado *processo; // Ponteiro para processo simulado

    // Loop até o usuário escolher sair (opção 8)
    while (opcao != 8)
    {
        // Imprime o menu de impressão
        printf("\n\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> MENU DE IMPRESSAO <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n\n");
        printf("1 - Estado atual do sistema\n");
        printf("2 - Tempo de uso atual do sistema\n");
        printf("3 - Processos em estado bloqueado\n");
        printf("4 - Processos em estado pronto\n");
        printf("5 - Quantidade de processos executados até o momento\n");
        printf("6 - Informações sobre a/as CPU no momento atual\n");
        printf("7 - Informações sobre a Tabela de Processos no momento atual\n");
        printf("8 - Sair da impressão\n");
        printf("=============================================================\n");
        printf("Escolha uma opção: ");
        // Lê a opção do usuário
        scanf(" %d%*[^\n]", &opcao);

        // Verifica qual opção foi escolhida
        if (opcao == 1)
        {
            // Imprime o estado atual do sistema
            ImprimeEstadoAtualSistema(gerenciador);
        }
        else if (opcao == 2)
        {
            // Imprime o tempo de uso do sistema
            printf("\n\nTempo de uso do sistema no momento atual: %d unidades de tempo\n", gerenciador->tempo);
        }
        else if (opcao == 3)
        {
            // Imprime os processos em estado bloqueado
            printf("\n\nProcessos em estado bloqueado:");

            // Verifica se a fila está vazia
            if (filaEhVazia(gerenciador->estadoBloqueado))
            {
                printf("\n   Fila Vazia!\n");
            }
            else
            {
                // Percorre a fila de bloqueados e imprime informações de cada processo
                CelulaPidTempo *celula = gerenciador->estadoBloqueado->Frente;
                while (celula != NULL)
                {
                    processo = buscaProcesso(gerenciador->tabelaProcessos, celula->pidTempo.pid);
                    printf("\n");
                    imprimeInfosGeraisProcesso(processo);
                    celula = celula->Prox;
                }
                putchar('\n');
            }
        }
        else if (opcao == 4)
        {
            // Imprime os processos em estado pronto
            printf("\n\nProcessos em estado pronto:");

            // Percorre todas as filas de prioridade
            for (int i = 0; i < CLASSESPRIORIDADES; i++)
            {
                Fila *fila = gerenciador->estadoPronto[i];

                printf("\nFila nº %d:", i);

                // Verifica se a fila está vazia
                if (filaEhVazia(fila))
                {
                    printf("\n   Fila Vazia!\n");
                }
                else
                {
                    // Percorre a fila e imprime informações de cada processo
                    CelulaPidTempo *celula = fila->Frente;
                    while (celula != NULL)
                    {
                        processo = buscaProcesso(gerenciador->tabelaProcessos, celula->pidTempo.pid);
                        printf("\n");
                        imprimeInfosGeraisProcesso(processo);

                        celula = celula->Prox;
                    }
                    putchar('\n');
                }
            }
        }
        else if (opcao == 5)
        {
            // Imprime a quantidade de processos executados até o momento
            printf("\n\nQuantidade de processos executados até o momento: %d\n\n", gerenciador->quantidadeProcessosIniciados);
        }
        else if (opcao == 6)
        {
            // Imprime informações das CPUs
            imprimeCPUs(gerenciador);
        }
        else if (opcao == 7)
        {
            // Imprime a tabela de processos
            imprimeTabelaProcessos(gerenciador);
            printf("\n");
            // Pergunta se deseja detalhes de algum processo
            printf("Deseja Imprime informações mais detalhadas sobre algum processo (0-NÃO/1-SIM): ");
            scanf(" %d%*[^\n]", &opcaoProcesso);

            if (opcaoProcesso == 1)
            {
                // Lê o PID do processo desejado
                printf("\nInforme o PID do processo: ");
                scanf(" %d%*[^\n]", &PID);
                processo = buscaProcesso(gerenciador->tabelaProcessos, PID);
                printf("\n\n");
                // Imprime informações detalhadas do processo
                imprimeInfosGeraisProcesso(processo);
                imprimeVariaveisProcesso(processo->vetorVariaveis, numeroVariaveis(*processo->conjuntoInstrucoes));
                imprimeVetorPrograma(*processo->conjuntoInstrucoes, *processo->pc);
            }
        }
    }
}

// Função que imprime informações gerais de um processo
void imprimeInfosGeraisProcesso(ProcessoSimulado *processo)
{
    printf("-> Processo - PID %d | ", processo->pid); // Imprime o PID
    printf("PPID %d | ", processo->pid); // Imprime o PPID
    printf("PC %d | ", *(processo->pc)); // Imprime o PC
    printf("Prioridade %d | ", processo->prioridade); // Imprime a prioridade
    imprimeEstadoProcesso(processo->estadoProcesso); // Imprime o estado do processo
    printf("Tempo de inicio %d | ", processo->tempoInicio); // Imprime o tempo de início
    printf("Tempo de CPU %d\n", processo->tempoCPU); // Imprime o tempo de CPU
}

// Função que imprime os valores atuais das variáveis do processo
void imprimeVariaveisProcesso(int *arrVariaveis, int tamanho)
{
    printf("| Valores atuais das variáveis: ");

    // Percorre o array de variáveis e imprime cada valor
    for (int i = 0; i < tamanho; i++)
    {
        printf(" %d ", arrVariaveis[i]);
    }
    printf("\n");
}

// Função que retorna o número de variáveis do processo
int numeroVariaveisProcesso(Instrucao *arrPrograma)
{
    return arrPrograma[0].paramNum1; // Retorna o valor do primeiro parâmetro numérico
}

// Função que imprime o estado do processo em formato textual
void imprimeEstadoProcesso(Estado estado){
    switch (estado)
    {
    case BLOQUEADO:
        printf("Estado: BLOQUEADO | ");
        break;

    case EXECUCAO:
        printf("Estado: EXECUCAO  | ");
        break;

    case PRONTO:
        printf("Estado: PRONTO    | ");
        break;

    default:
        break;
    }
}

// Função que imprime o estado do sistema em formato de arquivo
void impressaoArquivo(GerenciadorProcessos *gerenciador)
{
    // Imprime o cabeçalho do estado do sistema
    printf("\n╒══════════════════════════════════════════════════════╡ ESTADO DO SISTEMA ╞════════════════════════════════════════════════════════╕"); 
    printf("\n\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> GERENCIADOR DE PROCESSOS <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n\n");
    // Imprime o tempo de uso do sistema
    printf("\n°° Tempo de uso do sistema no momento atual: %d unidades de tempo", gerenciador->tempo);
    // Imprime os processos bloqueados
    printf("\n\n°° Processos em estado bloqueado:\n");
    imprimeFila(gerenciador->estadoBloqueado);
    // Imprime os processos prontos
    printf("\n\n°° Processos em estado pronto:\n");
    imprimeFilas(gerenciador->estadoPronto, CLASSESPRIORIDADES);
    // Imprime a quantidade de processos executados
    printf("\n\n°° Quantidade de processos executados até o momento: %d\n", gerenciador->quantidadeProcessosIniciados);
    // Imprime informações das CPUs
    imprimeCPUs(gerenciador);
    // Imprime a tabela de processos
    imprimeTabelaProcessos(gerenciador);
    // Imprime o rodapé
    printf("╘═══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╛\n");
}

// Função que imprime informações de todas as CPUs
void imprimeCPUs(GerenciadorProcessos *gerenciador)
{
    // Percorre todas as CPUs
    for (int i = 0; i < gerenciador->numCPUs; i++)
    {
        // Imprime o cabeçalho da CPU
        printf("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> CPU %d <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n", i);
        // Verifica se a CPU está livre
        if (cpuLivre(gerenciador->cpus[i]))
        {
            printf("\n->> CPU LIVRE\n");
        }
        else
        {
            // Imprime informações da CPU ocupada
            imprimeCPU(gerenciador->cpus[i]);
        }
    }
}

// Função que imprime informações de uma CPU específica
void imprimeCPU(CPU *cpu)
{
    printf("\n->> Processo em execução - PID %d | ", (*cpu->pidProcessoAtual)); // Imprime o PID do processo atual
    printf("PC %d |", (*cpu->pcProcessoAtual)); // Imprime o PC do processo atual
    printf(" Fatia do quantum já executado: %d ", cpu->fatiaQuantum); // Imprime a fatia do quantum executada
    imprimeVariaveisProcesso((*cpu->variaveisProcessoAtual), numeroVariaveis((*cpu->programaProcessoAtual))); // Imprime as variáveis do processo
    printf("\n");
}

// Função que imprime a tabela de processos
void imprimeTabelaProcessos(GerenciadorProcessos *gerenciador)
{
    // Imprime o cabeçalho da tabela de processos
    printf("\n\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> TABELA DE PROCESSOS <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");
    // Imprime a tabela de processos propriamente dita
    imprimeTabela(gerenciador->tabelaProcessos);
}