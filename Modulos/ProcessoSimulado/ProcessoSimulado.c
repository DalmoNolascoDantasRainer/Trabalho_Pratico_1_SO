#include "ProcessoSimulado.h"
#define BUFFER 100

// Cria o processo inicial (init)
ProcessoSimulado* criaProcessoInit(int tempoSistema) {

    ProcessoSimulado* processoInit = (ProcessoSimulado*) malloc(sizeof(ProcessoSimulado));

    processoInit->pid = 0; // PID do processo inicial e 0
    processoInit->pid_pai = 0; // Processo inicial nao tem pai

    processoInit->pc = (int*) malloc(sizeof(int)); 
    *(processoInit->pc) = 0; 

    processoInit->prioridade = 0; // Prioridade inicial e 0
    processoInit->estadoProcesso = PRONTO; // Estado inicial é PRONTO (pronto para executar)
    processoInit->tempoInicio = tempoSistema; // Tempo de inicio e o tempo atual do sistema
    processoInit->tempoCPU = 0; // Tempo de CPU inicial e 0 (ainda nao uso a CPU)

    processoInit->conjuntoInstrucoes = (Instrucao**) malloc(sizeof(Instrucao));
    leInstrucoesArquivo("./data/init", processoInit->conjuntoInstrucoes); 

    return processoInit; 
}

// Funcao que copia variaveis de um vetor para outro
void copiaVariaveis(int* vetorVariaveisBase, int* vetorVariaveisNovo, int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        vetorVariaveisNovo[i] = vetorVariaveisBase[i]; 
    }
}

// Funcao que copia o conjunto de instrucoes de um processo para outro
void copiaConjuntoInstrucoes(Instrucao** vetorNovo, Instrucao* vetorBase) {
    Instrucao* conjuntoInstrucoes = (Instrucao*) malloc(MAXINSTRUCOES * sizeof(Instrucao));
    int i = 0;

    // Copia instrucoes ate encontrar a instrucao de termino (T)
    while (vetorBase[i-1].tipoInstrucao != 'T'  && i < MAXINSTRUCOES) {  //ALTEREI AQ caiouuuuu
        copiaInstrucao(&conjuntoInstrucoes[i], &vetorBase[i]);
        i++;
    }

    *vetorNovo = conjuntoInstrucoes; // Define o novo conjunto de instrucoes
}


// Funcao que cria uma copia de um processo (fork)
ProcessoSimulado* copiaProcesso(ProcessoSimulado processoPai, int tempoAtualSistema, int novoPid) {

    ProcessoSimulado* filho = (ProcessoSimulado*) malloc(sizeof(ProcessoSimulado));

    if (!filho) return NULL;

    // Inicializa os atributos do novo processo
    filho->pid = novoPid; 
    filho->pid_pai = processoPai.pid; 

    // Inicializa o pc com a proxima instrucao
    filho->pc = (int*) malloc(sizeof(int));
    *(filho->pc) = *(processoPai.pc) + 1;

    // Copia os dados do processo pai para o novo processo pq cada processo precisa ter suas proprias variaveis, 
    // nao podemos apenas copiar o ponteiro, isso causaria conflitos
    filho->vetorVariaveis = (int*) malloc(numeroVariaveis(*processoPai.conjuntoInstrucoes) * sizeof(int));
    copiaVariaveis(processoPai.vetorVariaveis, filho->vetorVariaveis, numeroVariaveis(*processoPai.conjuntoInstrucoes));

    filho->prioridade = processoPai.prioridade;
    filho->estadoProcesso = PRONTO; // Estado inicial é PRONTO
    filho->tempoInicio = tempoAtualSistema; // Tempo de inicio e o tempo atual do sistema
    filho->tempoCPU = 0; // Tempo de CPU inicial e 0 (ainda nao uso a CPU)

    // Copia o conjunto de instrucoes do processo pai
    filho->conjuntoInstrucoes = (Instrucao**) malloc(sizeof(Instrucao));
    copiaConjuntoInstrucoes(filho->conjuntoInstrucoes, *(processoPai.conjuntoInstrucoes));

    return filho;
}


// Funcao que retorna o numero de variaveis no conjunto de instrucoes
int numeroVariaveis(Instrucao* conjuntoInstrucoes) {
    return conjuntoInstrucoes[0].parametroNum1; // O numero de variaveis esta no primeiro parametro
}



/////////////////////////////////////////////////////////////
//// REMOVER ISSO E DEIXAR NO PROCESSO DE IMPRESSAO /////
// Funcao que imprime as informacoes de um processo
void imprimeProcesso(ProcessoSimulado processo, int opcao) {
    // Imprime os atributos basicos do processo
    printf("-> Processo - PID %2d | ", processo.pid);
    printf("pid_pai %2d | ", processo.pid_pai);
    printf("PC %2d | ", *(processo.pc));
    printf("Prioridade %d | ", processo.prioridade);
    imprimeEstadoProcessoSimulado(processo.estadoProcesso);
    printf("Tempo de inicio %2d | ", processo.tempoInicio);
    printf("Tempo de CPU %2d\n", processo.tempoCPU);
    
    // Imprime informacoes adicionais com base na opcao
    switch (opcao) {
        case 1:
            break; // Nao imprime nada adicional
        case 2:
            imprimeVariaveis(processo.vetorVariaveis, numeroVariaveis(*processo.conjuntoInstrucoes));
            break; // Imprime as variaveis
        case 3:
            imprimeVetorPrograma(*(processo.conjuntoInstrucoes), *processo.pc);
            break; // Imprime o conjunto de instrucoes
        case 4:
            imprimeVariaveis(processo.vetorVariaveis, numeroVariaveis(*processo.conjuntoInstrucoes));
            imprimeVetorPrograma(*(processo.conjuntoInstrucoes), *processo.pc);
            break; // Imprime variaveis e conjunto de instrues
        default:
            break;
    }
    putchar('\n'); 
}


//// REMOVER ISSO E DEIXAR NO PROCESSO DE IMPRESSAO /////
// Funcao que imprime as variaveis de um processo
void imprimeVariaveis(int* vetorVariaveis, int tamanho) {
    printf("  |Variáveis: ");
    for (int i = 0; i < tamanho; i++) {
        printf("%d ", vetorVariaveis[i]); // Imprime cada variável
    }
    putchar('\n'); 
}


///// REMOVER ISSO E DEIXAR NO PROCESSO DE IMPRESSAO /////
// Funcao que imprime o estado de um processo
void imprimeEstadoProcessoSimulado(Estado estadoProcesso) {
    switch (estadoProcesso) {
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