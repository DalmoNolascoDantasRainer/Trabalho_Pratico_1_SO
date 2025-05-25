#include "Cpu.h"

// Inicializa a estrutura da CPU
void inicializaCPU(CPU** cpu){   
    *cpu = (CPU*) malloc(sizeof(CPU)); 
    
    (*cpu)->pidProcessoAtual = NUMEROVAZIO; // Inicializa o PID como vazio
    (*cpu)->pcProcessoAtual = NUMEROVAZIO; // Inicializa o PC como vazio
    (*cpu)->programaProcessoAtual = NULL; // Inicializa o programa como NULL
    (*cpu)->variaveisProcessoAtual = NULL; // Inicializa as variaveis como NULL
    (*cpu)->fatiaQuantum = 0; // Inicializa o quantum como zero
}

int cpuLivre(CPU* cpu){
    return cpu->pidProcessoAtual == NUMEROVAZIO; 
}

// Reseta a CPU, reinicializando seus campos
void zeraCPU(CPU* cpu){
    cpu->pidProcessoAtual = NUMEROVAZIO;
    cpu->pcProcessoAtual = NUMEROVAZIO;
    cpu->programaProcessoAtual = NULL;
    cpu->variaveisProcessoAtual = NULL;
    cpu->fatiaQuantum = 0;
}

// Carrega um processo na CPU, copiando os dados do processo atual
void insereProcessoCPU(CPU* cpu, ProcessoSimulado* processoAtual){
    cpu->pidProcessoAtual = processoAtual->pid;
    cpu->pcProcessoAtual = *(processoAtual->pc);

    // Aponta para o conjunto de instrucoes do processo atual
    cpu->programaProcessoAtual = *(processoAtual->conjuntoInstrucoes);
    
    // Aponta para o vetor de variaveis do processo atual
    cpu->variaveisProcessoAtual = &(processoAtual->vetorVariaveis);
    
    cpu->fatiaQuantum = 0;// Reinicia o quantum da CPU
}


// Executa a proxima instrucao do processo carregado na CPU.
void executaProxInstrucao(CPU* cpu, int tempoAtualSistema, Lista* tabelaProcessos,
                          int* quantidadeProcessosIniciados, Fila** estadoPronto, Fila* estadoBloqueado){   

    
    // Recupera os parametros da instrucao atual.
    char tipo = (cpu->programaProcessoAtual[cpu->pcProcessoAtual]).tipoInstrucao;
    int parametroNum1 = (cpu->programaProcessoAtual[cpu->pcProcessoAtual]).parametroNum1;
    int parametroNum2 = (cpu->programaProcessoAtual[cpu->pcProcessoAtual]).parametroNum2;
    char paramTxt[BUFFER];
    strcpy(paramTxt, (cpu->programaProcessoAtual[cpu->pcProcessoAtual]).paramTexto);

    // Executa a instrucao de acordo com o tipo.
    switch (tipo){
        case 'N':
            *cpu->variaveisProcessoAtual = instrucaoTipoN(parametroNum1);
            break;

        case 'D':
            instrucaoTipoD(parametroNum1, *cpu->variaveisProcessoAtual);
            break;

        case 'V':
            instrucaoTipoV(parametroNum1, parametroNum2, *cpu->variaveisProcessoAtual);
            break;

        case 'A':
            instrucaoTipoA(parametroNum1, parametroNum2, *cpu->variaveisProcessoAtual);
            break;

        case 'S':
            instrucaoTipoS(parametroNum1, parametroNum2, *cpu->variaveisProcessoAtual);
            break;
        
        case 'B':
            instrucaoTipoB(parametroNum1, &(cpu->pidProcessoAtual), tabelaProcessos, estadoBloqueado);
            break;

        case 'T':
            instrucaoTipoT(&(cpu->pidProcessoAtual), tabelaProcessos);
            break;
        
        case 'F':
            instrucaoTipoF(parametroNum1, &(cpu->pidProcessoAtual), &(cpu->pcProcessoAtual), quantidadeProcessosIniciados, tempoAtualSistema, tabelaProcessos, estadoPronto);
            break;

        case 'R':
            instrucaoTipoR(paramTxt, &(cpu->programaProcessoAtual), &(cpu->pcProcessoAtual));
            break;

        default:
            break;
    }

    // Avanca o PC e incrementa o quantum
    cpu->pcProcessoAtual += 1;
    cpu->fatiaQuantum++;
    
    // Atualiza o PC no processo original
    ProcessoSimulado* processo = buscaProcesso(tabelaProcessos, cpu->pidProcessoAtual);
    if(processo != NULL) {
        *(processo->pc) = cpu->pcProcessoAtual;
    }
}

// Aloca um vetor de variaveis inteiras para o processo
int *instrucaoTipoN(int n){
    int *vetorVariaveis;
    vetorVariaveis = malloc(n * sizeof(int));
    if (vetorVariaveis == NULL) {
        printf("Erro ao alocar memoria");
        exit(1);
    }

    return vetorVariaveis;
}

// Define o valor da variavel x como 0
void instrucaoTipoD(int x, int *vetorVariaveis){
    vetorVariaveis[x] = 0;
}

// Define o valor da variavel x como n
void instrucaoTipoV(int x, int n, int *vetorVariaveis){
    vetorVariaveis[x] = n;
}

// Soma n a variavel x
void instrucaoTipoA(int x, int n, int *vetorVariaveis){
    vetorVariaveis[x] += n;
}

// Subtrai n da variavel x
void instrucaoTipoS(int x, int n, int *vetorVariaveis){
    vetorVariaveis[x] -= n;
}

// Bloqueia o processo atual, colocando ele na fila de bloqueados
void instrucaoTipoB(int n, int* pidProcessoAtual, Lista* tabelaProcessos, Fila* estadoBloqueado){
    enfileirar(*pidProcessoAtual, n, estadoBloqueado);
    ProcessoSimulado* processo = buscaProcesso(tabelaProcessos, *pidProcessoAtual);
    processo->estadoProcesso = BLOQUEADO;
}

// Finaliza o processo atual, liberando memoria e alterando seu estado
void instrucaoTipoT(int* pidProcessoAtual, Lista* tabelaProcessos){
    ProcessoSimulado* processoEncerrado = buscaProcesso(tabelaProcessos, *pidProcessoAtual);
    free(processoEncerrado->conjuntoInstrucoes);
    free(processoEncerrado->vetorVariaveis);

    *processoEncerrado->pc = NUMEROVAZIO-1;
    processoEncerrado->estadoProcesso = BLOQUEADO;
}

// Cria um novo processo (fork), insere na tabela e fila de prontos
void instrucaoTipoF(int n, int* pidProcessoAtual, int* pcProcessoAtual, int* quantidadeProcessosIniciados, int tempoAtualSistema, Lista* tabelaProcessos, Fila** estadoPronto){
    ProcessoSimulado* processoPai = buscaProcesso(tabelaProcessos, *pidProcessoAtual);
    
    ProcessoSimulado* processoFilho = copiaProcesso(*processoPai, tempoAtualSistema, encontraMaiorPIDTabela(tabelaProcessos)+1);

    insereNaTabela(processoFilho, tabelaProcessos);
    enfileirar(processoFilho->pid, NUMEROVAZIO, estadoPronto[processoFilho->prioridade]);
    *quantidadeProcessosIniciados += 1;
    *pcProcessoAtual += n; // PULA INSTRUCOES QUE NAO SAO DESTINADAS A ELE
}

// Le instrucoes de um arquivo e carrega no processo
void instrucaoTipoR(char *nomeDoArquivo, Instrucao** vetorPrograma, int* pcProcessoAtual){   
    char caminhoArquivo[BUFFER] = "./arquivos/";
    strcat(caminhoArquivo, nomeDoArquivo);
    leInstrucoesArquivo(caminhoArquivo, vetorPrograma);
    *pcProcessoAtual = -1;
}