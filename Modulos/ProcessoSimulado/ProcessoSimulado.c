#include "ProcessoSimulado.h"
#define BUFFER 100

// Funcao que cria o processo inicial (init)
ProcessoSimulado* criaProcessoInit(int tempoSistema) {

    ProcessoSimulado* processoInit = (ProcessoSimulado*) malloc(sizeof(ProcessoSimulado));

    processoInit->pid = 0; // PID do processo inicial e 0
    processoInit->pid_pai = 0; // Processo inicial nao tem pai

    processoInit->pc = (int*) malloc(sizeof(int)); 
    *(processoInit->pc) = 0; 

    processoInit->prioridade = 0; // Prioridade inicial e 0
    processoInit->estadoProcesso = PRONTO; // Estado inicial e PRONTO (pronto para executar)
    processoInit->tempoInicio = tempoSistema; // Tempo de inicio e o tempo atual do sistema
    processoInit->tempoCPU = 0; // Tempo de CPU inicial e 0 (ainda nao uso a CPU)
    processoInit->conjuntoInstrucoes = NULL;
    
    processoInit->conjuntoInstrucoes = (Instrucao**) malloc(sizeof(Instrucao));

    leInstrucoesArquivo("./arquivos/init", processoInit->conjuntoInstrucoes); 
    

    return processoInit; 
}

// Funcao que retora o estado como string
const char* estadoParaString(Estado estado) {
    switch (estado) {
        case PRONTO: return "PRONTO";
        case EXECUCAO: return "EXECUCAO";
        case BLOQUEADO: return "BLOQUEADO";
        default: return "DESCONHECIDO";
    }
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
    while (vetorBase[i-1].tipoInstrucao != 'T'  && i < MAXINSTRUCOES) { 
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
    filho->vetorVariaveis = (int*)malloc(numeroVariaveis(*processoPai.conjuntoInstrucoes) * sizeof(int));
    copiaVariaveis(processoPai.vetorVariaveis, filho->vetorVariaveis, numeroVariaveis(*processoPai.conjuntoInstrucoes));

    filho->prioridade = processoPai.prioridade;
    
    filho->estadoProcesso = PRONTO; // Estado inicial e PRONTO
    filho->tempoInicio = tempoAtualSistema; // Tempo de inicio e o tempo atual do sistema
    filho->tempoCPU = 0; // Tempo de CPU inicial e 0 (ainda nao uso a CPU)

    // Copia o conjunto de instrucoes do processo pai
    filho->conjuntoInstrucoes = (Instrucao**) malloc(sizeof(Instrucao));
    copiaConjuntoInstrucoes(filho->conjuntoInstrucoes, *(processoPai.conjuntoInstrucoes));
    
    return filho;
}


// Funcao que retorna o numero de variaveis no conjunto de instrucoes
int numeroVariaveis(Instrucao* conjuntoInstrucoes) {
    return conjuntoInstrucoes[0].parametroNum1; // Colocamos o numero de variaveis esta no primeiro parametro
}


// Funcao que imprime as informacoes de um processo
void imprimeProcesso(ProcessoSimulado processo, int opcao) {

    // Estado do processo 
    printf("║  %4d  ║   %4d   ║ %4d ║     %2d     ║  %-12s  ║      %4d      ║      %4d     ║\n",
        processo.pid,
        processo.pid_pai,
        *(processo.pc),
        processo.prioridade,
        estadoParaString(processo.estadoProcesso),
        processo.tempoInicio,
        processo.tempoCPU
    );

    // Imprime informacoes adicionais com base na opcao (debug)
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
    printf("╚════════╩══════════╩══════╩════════════╩════════════════╩════════════════╩═══════════════╝\n");
}

void imprimeProcessoRR(ProcessoSimulado processo, int opcao) {

    // Estado do processo 
    
        printf("║  %4d  ║   %4d   ║ %4d ║  %-10s║      %4d      ║      %4d     ║\n",
        processo.pid,
        processo.pid_pai,
        *(processo.pc),
        estadoParaString(processo.estadoProcesso),
        processo.tempoInicio,
        processo.tempoCPU
    );

    // Imprime informacoes adicionais com base na opcao (debug)
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
    printf("╚════════╩══════════╩══════╩════════════╩════════════════╩═══════════════╝\n");
    
}

void imprimePID(ProcessoSimulado processo) {
    printf("║            %d            ║ \n", processo.pid);
    
}

// Funcao que imprime as variaveis de um processo
void imprimeVariaveis(int* vetorVariaveis, int tamanho) {
    // Borda superior
    printf("                                                                                                                                \n");

    // Título + variáveis na mesma linha
    printf("       Variáveis: ");
    for (int i = 0; i < tamanho; i++) {
        printf("│ %d ", vetorVariaveis[i]);
    }

    
}