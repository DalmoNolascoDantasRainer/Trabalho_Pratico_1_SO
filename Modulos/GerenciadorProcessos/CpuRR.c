#include "CpuRR.h"

// Inicializa a estrutura da CPU
void inicializaCPURR(CPU_RR** cpu){   
    *cpu = (CPU_RR*) malloc(sizeof(CPU_RR)); 

    (*cpu)->pidProcessoAtual = NUMEROVAZIO;
    (*cpu)->pcProcessoAtual = NUMEROVAZIO; 
    (*cpu)->programaProcessoAtual = NULL; // Inicializa o programa como NULL
    (*cpu)->variaveisProcessoAtual = NULL; // Inicializa as variaveis como NULL
    (*cpu)->fatiaQuantum = 0; // Inicializa o quantum como zero
}

int cpuLivreRR(CPU_RR* cpu){
    return cpu->pidProcessoAtual == NUMEROVAZIO; 
}

// Reseta a CPU, reinicializando seus campos
void zeraCPURR(CPU_RR* cpu){
    cpu->pidProcessoAtual = NUMEROVAZIO;
    cpu->pcProcessoAtual = NUMEROVAZIO;
    cpu->programaProcessoAtual = NULL;
    cpu->variaveisProcessoAtual = NULL;
    cpu->fatiaQuantum = 0;
}

// Carrega um processo na CPU, copiando os dados do processo atual
// Esta funcao faz com que a CPU passe a operar sobre o processo fornecido,
// copiando os valores dos campos do processo
void insereProcessoCPURR(CPU_RR* cpu, ProcessoSimulado* processoAtual){
    
    cpu->pidProcessoAtual = processoAtual->pid;

    cpu->pcProcessoAtual = *(processoAtual->pc);

    // Aponta para o conjunto de instrucoes do processo atual
    cpu->programaProcessoAtual = *(processoAtual->conjuntoInstrucoes);

    // Aponta para o vetor de variaveis do processo atual
    cpu->variaveisProcessoAtual = &(processoAtual->vetorVariaveis);

    cpu->fatiaQuantum = 0;// Reinicia o quantum da CPU
}

// Executa a proxima instrucao do processo carregado na CPU
void executaProxInstrucaoCPURR(CPU_RR* cpu, int tempoAtualSistema, Lista* tabelaProcessos,
                          int* quantidadeProcessosIniciados, Fila* filaRoundRobin, Fila* estadoBloqueado){
    if (!filaRoundRobin) {
        printf("Erro: filaRoundRobin é NULL\n");
        return;
    }                     
    // Recupera os parametros da instrucao atual
    char tipo = (cpu->programaProcessoAtual[cpu->pcProcessoAtual]).tipoInstrucao;
    int parametroNum1 = (cpu->programaProcessoAtual[cpu->pcProcessoAtual]).parametroNum1;
    int parametroNum2 = (cpu->programaProcessoAtual[cpu->pcProcessoAtual]).parametroNum2;
    char paramTxt[BUFFER];
    strcpy(paramTxt, (cpu->programaProcessoAtual[cpu->pcProcessoAtual]).paramTexto);
    
    // Executa a instrucao de acordo com o tipo
    switch (tipo){
        case 'N':
            *cpu->variaveisProcessoAtual = instrucaoTipoNRR(parametroNum1);
            break;

        case 'D':
            instrucaoTipoDRR(parametroNum1, *cpu->variaveisProcessoAtual);
            break;

        case 'V':
            instrucaoTipoVRR(parametroNum1, parametroNum2, *cpu->variaveisProcessoAtual);
            break;

        case 'A':
            instrucaoTipoARR(parametroNum1, parametroNum2, *cpu->variaveisProcessoAtual);
            break;

        case 'S':
            instrucaoTipoSRR(parametroNum1, parametroNum2, *cpu->variaveisProcessoAtual);
            break;

        case 'B':
            instrucaoTipoBRR(parametroNum1, &(cpu->pidProcessoAtual), tabelaProcessos, estadoBloqueado);
            break;

        case 'T':
            instrucaoTipoTRR(&(cpu->pidProcessoAtual), tabelaProcessos);
            break;

        case 'F':
            instrucaoTipoFRR(parametroNum1, &(cpu->pidProcessoAtual), &(cpu->pcProcessoAtual),
                          quantidadeProcessosIniciados, tempoAtualSistema, tabelaProcessos, filaRoundRobin);
            break;

        case 'R':
            instrucaoTipoRRR(paramTxt, &(cpu->programaProcessoAtual), &(cpu->pcProcessoAtual));
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


/* Instrucoes processadas na CPU */

// Aloca um vetor de variaveis para o processo
int *instrucaoTipoNRR(int n){
    int *vetorVariaveis;
    vetorVariaveis = malloc(n * sizeof(int));
    if (vetorVariaveis == NULL) {
        printf("Erro ao alocar memoria");
        exit(1);
    }

    return vetorVariaveis;
}

// Define o valor da variavel x como 0
void instrucaoTipoDRR(int x, int *vetorVariaveis){
    vetorVariaveis[x] = 0;
}

// Define o valor da variavel x como n
void instrucaoTipoVRR(int x, int n, int *vetorVariaveis){
    vetorVariaveis[x] = n;
}

// Soma n a variavel x
void instrucaoTipoARR(int x, int n, int *vetorVariaveis){
    vetorVariaveis[x] += n;
}

// Subtrai n da variavel x
void instrucaoTipoSRR(int x, int n, int *vetorVariaveis){
    vetorVariaveis[x] -= n;
}

// Bloqueia o processo atual, colocando ele na fila de bloqueados
void instrucaoTipoBRR(int n, int* pidProcessoAtual, Lista* tabelaProcessos, Fila* estadoBloqueado){
    enfileirar(*pidProcessoAtual, n, estadoBloqueado);
    ProcessoSimulado* processo = buscaProcesso(tabelaProcessos, *pidProcessoAtual);
    processo->estadoProcesso = BLOQUEADO;
}

// Finaliza o processo atual, liberando memoria e alterando seu estado
void instrucaoTipoTRR(int* pidProcessoAtual, Lista* tabelaProcessos){
    ProcessoSimulado* processoEncerrado = buscaProcesso(tabelaProcessos, *pidProcessoAtual);

    free(processoEncerrado->conjuntoInstrucoes);
    free(processoEncerrado->vetorVariaveis);

    *processoEncerrado->pc = NUMEROVAZIO-1;
    processoEncerrado->estadoProcesso = BLOQUEADO;
}
// Cria um novo processo (fork), insere na tabela e na fila Round Robin
void instrucaoTipoFRR(int n, int* pidProcessoAtual, int* pcProcessoAtual, int* quantidadeProcessosIniciados, 
                    int tempoAtualSistema, Lista* tabelaProcessos, Fila* filaRoundRobin){

    ProcessoSimulado* processoPai = buscaProcesso(tabelaProcessos, *pidProcessoAtual);
    
    ProcessoSimulado* processoFilho = copiaProcesso(*processoPai, tempoAtualSistema, maiorPIDTabela(tabelaProcessos)+1);

    insereNaTabela(processoFilho, tabelaProcessos);
    enfileirar(processoFilho->pid, NUMEROVAZIO, filaRoundRobin);
    *quantidadeProcessosIniciados += 1;

    *pcProcessoAtual += n; // PULA INSTRUCOES QUE NAO SAO DESTINADAS A ELE
    
}

// Le instrucoes de um arquivo e carrega no processo
void instrucaoTipoRRR(char *nomeDoArquivo, Instrucao** vetorPrograma, int* pcProcessoAtual){   
    char caminhoArquivo[BUFFER] = "./arquivos/";
    strcat(caminhoArquivo, nomeDoArquivo);
    leInstrucoesArquivo(caminhoArquivo, vetorPrograma);

    *pcProcessoAtual = -1;
}