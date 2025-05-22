#include "ProcessoSimulado.h"
#define BUFFER 100

// Cria o processo inicial (init)
ProcessoSimulado* criaProcessoInit(int tempoSistema) {

    ProcessoSimulado* processo = (ProcessoSimulado*) malloc(sizeof(ProcessoSimulado));

    processo->pid = 0; // PID do processo inicial e 0
    processo->pid_pai = 0; // Processo inicial nao tem pai

    processo->pc = (int*) malloc(sizeof(int)); 
    *(processo->pc) = 0; 

    processo->prioridade = 0; // Prioridade inicial e 0
    processo->estadoProcesso = PRONTO; // Estado inicial é PRONTO (pronto para executar)
    processo->tempoInicio = tempoSistema; // Tempo de inicio e o tempo atual do sistema
    processo->tempoCPU = 0; // Tempo de CPU inicial e 0 (ainda nao uso a CPU)

    processo->conjuntoInstrucoes = (Instrucao**) malloc(sizeof(Instrucao));
    leInstrucoesArquivo("./data/init", processo->conjuntoInstrucoes); 

    return processo; 
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
    while (vetorBase[i-1].tipoInstrucao != 'T') {
        copiaInstrucao(&conjuntoInstrucoes[i], &vetorBase[i]);
        i++;
    }

    *vetorNovo = conjuntoInstrucoes; // Define o novo conjunto de instrucoes
}


// Funcao que cria uma copia de um processo (fork)
ProcessoSimulado* copiaProcesso(ProcessoSimulado processoPai, int tempoAtualSistema, int novoPid) {
    ProcessoSimulado* processo = (ProcessoSimulado*) malloc(sizeof(ProcessoSimulado));

    // Inicializa os atributos do novo processo
    processo->pid = novoPid; 
    processo->pid_pai = processoPai.pid; 

    // Inicializa o pc com a proxima instrucao
    processo->pc = (int*) malloc(sizeof(int));
    *(processo->pc) = *(processoPai.pc) + 1;

    // Copia os dados do processo pai para o novo processo pq cada processo precisa ter suas proprias variaveis, 
    // nao podemos apenas copiar o ponteiro, isso causaria conflitos
    processo->vetorVariaveis = (int*) malloc(numeroVariaveis(*processoPai.conjuntoInstrucoes) * sizeof(int));
    copiaVariaveis(processoPai.vetorVariaveis, processo->vetorVariaveis, numeroVariaveis(*processoPai.conjuntoInstrucoes));

    processo->prioridade = processoPai.prioridade;
    processo->estadoProcesso = PRONTO; // Estado inicial é PRONTO
    processo->tempoInicio = tempoAtualSistema; // Tempo de inicio e o tempo atual do sistema
    processo->tempoCPU = 0; // Tempo de CPU inicial e 0 (ainda nao uso a CPU)

    // Copia o conjunto de instrucoes do processo pai
    processo->conjuntoInstrucoes = (Instrucao**) malloc(sizeof(Instrucao));
    copiaConjuntoInstrucoes(processo->conjuntoInstrucoes, *(processoPai.conjuntoInstrucoes));

    return processo;
}


// Funcao que retorna o numero de variaveis no conjunto de instrucoes
int numeroVariaveis(Instrucao* conjuntoInstrucoes) {
    return conjuntoInstrucoes[0].paramNum1; // O numero de variaveis esta no primeiro parametro
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