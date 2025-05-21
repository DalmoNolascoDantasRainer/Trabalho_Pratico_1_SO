
#include "Cpu.h"

// Inicializa a estrutura da CPU, alocando memória para seus campos.
CPU* inicializaCPU(){   
    CPU* cpu = (CPU*) malloc(sizeof(CPU)); // Aloca memória para a estrutura CPU
    
    cpu->pidProcessoAtual = (int*) malloc(sizeof(int)); // Aloca memória para o PID do processo atual
    *cpu->pidProcessoAtual = NUMEROVAZIO; // Inicializa o PID como vazio
    cpu->pcProcessoAtual = (int*) malloc(sizeof(int)); // Aloca memória para o PC do processo atual

    cpu->programaProcessoAtual = (Instrucao**) malloc(sizeof(Instrucao)); // Aloca memória para o ponteiro do programa do processo atual
    cpu->variaveisProcessoAtual = (int**) malloc(sizeof(int)); // Aloca memória para o ponteiro das variáveis do processo atual

    cpu->fatiaQuantum = 0; // Inicializa o quantum como zero

    return cpu; // Retorna o ponteiro para a CPU inicializada
}

// Carrega um processo na CPU, apontando para os dados do processo atual.
// Esta função faz com que a CPU passe a operar sobre o processo fornecido,
// atualizando seus ponteiros para os campos do processo.

void carregaProcesso(CPU* cpu, ProcessoSimulado* processoAtual)
{
    // Atualiza o ponteiro do PID para o PID do processo atual
    cpu->pidProcessoAtual = &(processoAtual->pid);

    // Atualiza o ponteiro do PC para o PC do processo atual
    cpu->pcProcessoAtual = processoAtual->pc;

    // Atualiza o ponteiro para o conjunto de instruções do processo atual
    cpu->programaProcessoAtual = processoAtual->conjuntoInstrucoes;

    // Atualiza o ponteiro para o vetor de variáveis do processo atual
    cpu->variaveisProcessoAtual = &processoAtual->vetorVariaveis;
    
    cpu->fatiaQuantum = 0;// Reinicia o quantum da CPU
}

// Executa a próxima instrução do processo carregado na CPU.
void executaProxInstrucao(CPU* cpu, int tempoAtualSistema, Lista* tabelaProcessos,
                             int* quantidadeProcessosIniciados, Fila** estadoPronto, Fila* estadoBloqueado)
{   
    // Recupera os parâmetros da instrução atual.
    char tipo = ((*cpu->programaProcessoAtual)[*cpu->pcProcessoAtual]).tipoInstrucao;
    int paramNum1 = ((*cpu->programaProcessoAtual)[*cpu->pcProcessoAtual]).paramNum1;
    int paramNum2 = ((*cpu->programaProcessoAtual)[*cpu->pcProcessoAtual]).paramNum2;
    char paramTxt[BUFFER];
    strcpy(paramTxt, ((*cpu->programaProcessoAtual)[*cpu->pcProcessoAtual]).paramTexto);

    // Executa a instrução de acordo com o tipo.
    switch (tipo)
    {
    case 'N':
        *cpu->variaveisProcessoAtual = instrucaoTipoN(paramNum1);
        break;

    case 'D':
        instrucaoTipoD(paramNum1, *cpu->variaveisProcessoAtual);
        break;

    case 'V':
        instrucaoTipoV(paramNum1, paramNum2, *cpu->variaveisProcessoAtual);
        break;

    case 'A':
        instrucaoTipoA(paramNum1, paramNum2, *cpu->variaveisProcessoAtual);
        break;

    case 'S':
        instrucaoTipoS(paramNum1, paramNum2, *cpu->variaveisProcessoAtual);
        break;
    
    case 'B':
        instrucaoTipoB(paramNum1, cpu->pidProcessoAtual, tabelaProcessos, estadoBloqueado);
        break;

    case 'T':
        instrucaoTipoT(cpu->pidProcessoAtual, tabelaProcessos);
        break;
    
    case 'F':
        instrucaoTipoF(paramNum1, cpu->pidProcessoAtual, cpu->pcProcessoAtual, quantidadeProcessosIniciados, tempoAtualSistema, tabelaProcessos, estadoPronto);
        break;

    case 'R':
        instrucaoTipoR(paramTxt, cpu->programaProcessoAtual, cpu->pcProcessoAtual);
        break;

    default:
        break;
    }

    // Avança o PC e incrementa o quantum.
    *(cpu->pcProcessoAtual) += 1;
    cpu->fatiaQuantum++;
}

// Verifica se a CPU está livre (sem processo carregado).
int cpuLivre(CPU* cpu)
{
    return *(cpu->pidProcessoAtual) == NUMEROVAZIO; 
}

// Reseta a CPU, desalocando e reinicializando seus campos.
void zeraCPU(CPU* cpu)
{
    cpu->pidProcessoAtual = (int*) malloc(sizeof(int));
    *cpu->pidProcessoAtual = NUMEROVAZIO;
    cpu->pcProcessoAtual = (int*) malloc(sizeof(int));
    *(cpu->pcProcessoAtual) = NUMEROVAZIO;

    cpu->programaProcessoAtual = NULL;
    cpu->variaveisProcessoAtual = NULL;

    cpu->fatiaQuantum = 0;
}


/* -------------- Instruçẽos de programa que são processadas na CPU -------------- */

// Aloca um vetor de variáveis inteiras para o processo.
int *instrucaoTipoN(int n)
{
    int *vetorVariaveis;
    vetorVariaveis = (int *)malloc(n * sizeof(int));
    if (vetorVariaveis == NULL)
    {
        printf("Erro ao alocar memória");
        exit(1);
    }

    return vetorVariaveis;
}

// Define o valor da variável x como 0.
void instrucaoTipoD(int x, int *vetorVariaveis)
{
    vetorVariaveis[x] = 0;
}

// Define o valor da variável x como n.
void instrucaoTipoV(int x, int n, int *vetorVariaveis)
{
    vetorVariaveis[x] = n;
}

// Soma n à variável x.
void instrucaoTipoA(int x, int n, int *vetorVariaveis)
{
    vetorVariaveis[x] += n;
}

// Subtrai n da variável x.
void instrucaoTipoS(int x, int n, int *vetorVariaveis){
    vetorVariaveis[x] -= n;
}

// Bloqueia o processo atual, colocando-o na fila de bloqueados.
void instrucaoTipoB(int n, int* pidProcessoAtual, Lista* tabelaProcessos, Fila* estadoBloqueado)
{
    enfileirar(*pidProcessoAtual, n, estadoBloqueado);
    ProcessoSimulado* processo = buscaProcesso(tabelaProcessos, *pidProcessoAtual);
    processo->estadoProcesso = BLOQUEADO;
}

// Finaliza o processo atual, liberando memória e alterando seu estado.
void instrucaoTipoT(int* pidProcessoAtual, Lista* tabelaProcessos)
{
    ProcessoSimulado* processoEncerrado = buscaProcesso(tabelaProcessos, *pidProcessoAtual);

    free(processoEncerrado->conjuntoInstrucoes);
    free(processoEncerrado->vetorVariaveis);

    *processoEncerrado->pc = NUMEROVAZIO-1;
    processoEncerrado->estadoProcesso = BLOQUEADO;
}

// Cria um novo processo (fork), insere na tabela e fila de prontos.
void instrucaoTipoF(int n, int* pidProcessoAtual, int* pcProcessoAtual, int* quantidadeProcessosIniciados, int tempoAtualSistema, Lista* tabelaProcessos, Fila** estadoPronto)
{
    ProcessoSimulado* processoPai = buscaProcesso(tabelaProcessos, *pidProcessoAtual);
    ProcessoSimulado* processoFilho = copiaProcesso(*processoPai, tempoAtualSistema, maiorPIDTabela(tabelaProcessos)+1);

    insereTabela(tabelaProcessos, processoFilho);
    enfileirar(processoFilho->pid, NUMEROVAZIO, estadoPronto[processoFilho->prioridade]);
    quantidadeProcessosIniciados += 1;

    *pcProcessoAtual += n;
}

// Lê instruções de um arquivo e carrega no processo.
void instrucaoTipoR(char *nomeDoArquivo, Instrucao** arrPrograma, int* pcProcessoAtual)
{   
    char caminhoArquivo[BUFFER] = "./data/";
    strcat(caminhoArquivo, nomeDoArquivo);
    leInstrucoesArquivo(caminhoArquivo, arrPrograma);

    *pcProcessoAtual = -1;
}