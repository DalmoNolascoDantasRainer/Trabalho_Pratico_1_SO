#include "GerenciadorProcessos.h"

// Funcao que inicializa o gerenciador de processos
GerenciadorProcessos *inicializaGerenciador(int numCPUs)
{
    // Aloca memoria para o gerenciador de processos
    GerenciadorProcessos *gerenciador = (GerenciadorProcessos *)malloc(sizeof(GerenciadorProcessos));

    // Inicializa os atributos do gerenciador
    gerenciador->tempo = 0;
    gerenciador->quantidadeProcessosIniciados = 0;
    gerenciador->tempoTotalExecucao = 0;
    gerenciador->numCPUs = numCPUs;
    
    // Inicializa o tipo de escalonamento como filas multiplas (padrao)
    gerenciador->tipoEscalonamento = ESC_FILAS_MULTIPLAS;

    // Aloca memoria para as CPUs e seus estados de execucao
    gerenciador->cpus = (CPU **)malloc(numCPUs * sizeof(CPU *));
    gerenciador->estadoExecucao = (int *)malloc(numCPUs * sizeof(int));

    // Inicializa cada CPU e define o estado como vazio
    for (int i = 0; i < numCPUs; i++)
    {
        gerenciador->cpus[i] = inicializaCPU();
        gerenciador->estadoExecucao[i] = NUMEROVAZIO;
    }

    // Cria a tabela de processos
    gerenciador->tabelaProcessos = criaLista();

    // Aloca memoria para as filas de processos prontos, uma para cada classe de prioridade
    gerenciador->estadoPronto = (Fila **)malloc(sizeof(Fila *) * CLASSESPRIORIDADES);

    // Inicializa as filas de processos prontos
    for (int i = 0; i < CLASSESPRIORIDADES; i++)
    {
        gerenciador->estadoPronto[i] = criaFila();
    }

    // Inicializa a fila de processos bloqueados
    gerenciador->estadoBloqueado = criaFila();
    
    // Inicializa a fila do Round Robin (usa apenas uma fila)
    gerenciador->filaRoundRobin = criaFila();

    return gerenciador;
}

// Funcao para definir o tipo de escalonamento
void defineEscalonamento(GerenciadorProcessos *gerenciador, TipoEscalonamento tipo){
    gerenciador->tipoEscalonamento = tipo;
}

// Funcao que inicia o processo inicial (init) 
void iniciaProcessoInit(GerenciadorProcessos *gerenciador)
{
    // Cria o processo inicial
    ProcessoSimulado *processoInit = criaProcessoInit(gerenciador->tempo);

    // Verifica o tipo de escalonamento para decidir onde enfileirar
    if (gerenciador->tipoEscalonamento == ESC_ROBIN) {
        // Para Round Robin, enfileira na fila única
        enfileirar(processoInit->pid, NUMEROVAZIO, gerenciador->filaRoundRobin);
    } else {
        // Para filas múltiplas, enfileira na fila de prioridade mais alta
        enfileirar(processoInit->pid, NUMEROVAZIO, gerenciador->estadoPronto[0]);
    }

    // Insere o processo inicial na tabela de processos
    insereNaTabela(processoInit, gerenciador->tabelaProcessos);

    // Incrementa o contador de processos iniciados
    gerenciador->quantidadeProcessosIniciados += 1;
}

// Incrementa o tempo do sistema
void encerraUnidadeTempo(GerenciadorProcessos *gerenciador)
{
    gerenciador->tempo += 1;
}

// DA PARA JUNTAR COM O ROUND ROBIN
// Escalona um processo especifico para uma CPU (versao original - filas multiplas)
void escalonaProcesso(Lista *tabelaProcessos, CPU *cpu, int *estadoExecucao, Fila **estadoPronto)
{
    int pidProcesso = desenfileirarFilas(estadoPronto, CLASSESPRIORIDADES); // Obtem o proximo processo da fila

    if (pidProcesso >= 0)
    {
        *estadoExecucao = pidProcesso; // Atualiza o estado de execucao da CPU

        ProcessoSimulado *proximoProceso = buscaProcesso(tabelaProcessos, pidProcesso); // Busca o processo na tabela

        proximoProceso->estadoProcesso = EXECUCAO; // Define o estado do processo como em execucao

        insereProcessoCPU(cpu, proximoProceso); // Carrega o processo na CPU
    }
}

// Escalona um processo especifico para uma CPU (versao Round Robin)
/*void escalonaProcessoRR(Lista *tabelaProcessos, CPU *cpu, int *estadoExecucao, Fila *filaRR)
{
    int pidProcesso = desenfileirar(filaRR); // Obtem o proximo processo da fila Round Robin

    if (pidProcesso >= 0)
    {
        *estadoExecucao = pidProcesso; // Atualiza o estado de execucao da CPU

        ProcessoSimulado *proximoProcesso = buscaProcesso(tabelaProcessos, pidProcesso); // Busca o processo na tabela

        proximoProcesso->estadoProcesso = EXECUCAO; // Define o estado do processo como em execucao

        insereProcessoCPU(cpu, proximoProcesso); // Carrega o processo na CPU
    }
}*/

void escalonaProcessoRR(Lista *tabelaProcessos, CPU *cpu, int *estadoExecucao, Fila *filaRR)
{
    PidTempo *pidTempo = desenfileirar(filaRR); // Obtem o proximo processo da fila Round Robin

    if (pidTempo != NULL)
    {
        int pidProcesso = pidTempo->pid;
        *estadoExecucao = pidProcesso; // Atualiza o estado de execucao da CPU

        ProcessoSimulado *proximoProcesso = buscaProcesso(tabelaProcessos, pidProcesso); // Busca o processo na tabela

        proximoProcesso->estadoProcesso = EXECUCAO; // Define o estado do processo como em execucao

        insereProcessoCPU(cpu, proximoProcesso); // Carrega o processo na CPU

        // Se pidTempo foi alocado dinamicamente, libere aqui:
        free(pidTempo);
    }
}

// Escalona processos para as CPUs disponiveis
void escalonaProcessosCPUs(GerenciadorProcessos *gerenciador)
{
    verificaBloqueados(gerenciador); // Verifica e desbloqueia processos, se necessario

    for (int i = 0; i < gerenciador->numCPUs; i++)
    {
        if (cpuLivre(gerenciador->cpus[i]) == 1)
        { // Verifica se a CPU esta livre
            
            if (gerenciador->tipoEscalonamento == ESC_ROBIN) {
                // Escalonamento Round Robin - usa uma única fila
                if (gerenciador->filaRoundRobin->Tamanho > 0) {
                    escalonaProcessoRR(gerenciador->tabelaProcessos, gerenciador->cpus[i], 
                                     gerenciador->estadoExecucao + i, gerenciador->filaRoundRobin);
                }
            } else {
                // Escalonamento com filas múltiplas (comportamento original)
                if (filasVazias(gerenciador->estadoPronto, CLASSESPRIORIDADES) == 0) { 
                    escalonaProcesso(gerenciador->tabelaProcessos, gerenciador->cpus[i], 
                                   gerenciador->estadoExecucao + i, gerenciador->estadoPronto);
                }
            }
        }
    }
}

// Executa os processos carregados nas CPUs
void executaCPUs(GerenciadorProcessos *gerenciador)
{
    for (int i = 0; i < gerenciador->numCPUs; i++)
    {
        if (!(cpuLivre(gerenciador->cpus[i])))
        { // Verifica se a CPU esta ocupada
            executaProxInstrucao(gerenciador->cpus[i], gerenciador->tempo, gerenciador->tabelaProcessos, &gerenciador->quantidadeProcessosIniciados, gerenciador->estadoPronto, gerenciador->estadoBloqueado);
        }
    }
}

// Realiza a troca de contexto nas CPUs
void trocaDeContexto(GerenciadorProcessos *gerenciador)
{
    for (int i = 0; i < gerenciador->numCPUs; i++)
    {
        if (!(cpuLivre(gerenciador->cpus[i])))
        { // Verifica se a CPU esta ocupado
            if (gerenciador->tipoEscalonamento == ESC_ROBIN) {
                // Para Round Robin, usa a versão específica da troca de contexto
                removeProcessoCPU_RR(gerenciador->cpus[i], gerenciador->tabelaProcessos, gerenciador->filaRoundRobin);
            } else {
                // Para filas múltiplas, usa a versão original
                removeProcessoCPU(gerenciador->cpus[i], gerenciador->tabelaProcessos, gerenciador->estadoPronto);
            }
        }
    }
}

// Funcao principal que gerencia os processos com base no comando recebido
void gerenciadorProcessos(GerenciadorProcessos *gerenciador, char comando){
    if (comando == 'U'){                                     // Comando para avancar uma unidade de tempo
        encerraUnidadeTempo(gerenciador); // Incrementa o tempo do sistema

        if (gerenciador->tempo == 1){                                       // Na primeira unidade de tempo
            iniciaProcessoInit(gerenciador);    // Inicia o processo inicial
            escalonaProcessosCPUs(gerenciador); // Escalona os processos para as CPUs
            executaCPUs(gerenciador);           // Executa os processos nas CPUs
            trocaDeContexto(gerenciador);       // Realiza troca de contexto, se necessario
        }
        else
        {
            escalonaProcessosCPUs(gerenciador);
            executaCPUs(gerenciador);
            trocaDeContexto(gerenciador);
        }
    }
}

//
// Remove um processo da CPU e o coloca na fila apropriada (versao original - filas multiplas)
void removeProcessoCPU(CPU *cpu, Lista *tabelaProcessos, Fila **estadoPronto){
    ProcessoSimulado *processoNaCPU = buscaProcesso(tabelaProcessos, *(cpu->pidProcessoAtual)); // Busca o processo na CPU

    if (processoNaCPU != NULL)
    {
        if (cpu->fatiaQuantum >= (calcularPotencia(2, processoNaCPU->prioridade)))// Verifica se o quantum foi excedido por deslocamento
        {                                                          // Verifica se o quantum foi excedido

            processoNaCPU->estadoProcesso = PRONTO; // Define o estado como pronto

            if (processoNaCPU->prioridade < CLASSESPRIORIDADES - 1) // Ajusta a prioridade, se necessario
            {
                processoNaCPU->prioridade++;
            }
            processoNaCPU->tempoCPU += cpu->fatiaQuantum; // Atualiza o tempo de CPU do processo

            enfileirar(processoNaCPU->pid, NUMEROVAZIO, estadoPronto[processoNaCPU->prioridade]); // Reenfileira o processo
            zeraCPU(cpu);                                                                         // Libera a CPU
        }
        else if (processoNaCPU->estadoProcesso == BLOQUEADO) // Caso o processo esteja bloqueado
        {
            processoNaCPU->tempoCPU += cpu->fatiaQuantum;
            zeraCPU(cpu);

            if (*processoNaCPU->pc == NUMEROVAZIO) // Remove o processo se ele terminou
            {
                removeDaTabela(tabelaProcessos, processoNaCPU->pid);
            }
        }
    }
}

// DA PARA JUNTAR COM O ROUND ROBIN
// Remove um processo da CPU e o coloca na fila Round Robin (nova implementacao)
void removeProcessoCPU_RR(CPU *cpu, Lista *tabelaProcessos, Fila *filaRR)
{
    ProcessoSimulado *processoNaCPU = buscaProcesso(tabelaProcessos, *(cpu->pidProcessoAtual)); // Busca o processo na CPU

    if (processoNaCPU != NULL)
    {
        // No Round Robin, o quantum é fixo (por exemplo, 3 unidades de tempo)
        const int QUANTUM_RR = 3;
        
        if (cpu->fatiaQuantum >= QUANTUM_RR) // Verifica se o quantum foi excedido
        {
            processoNaCPU->estadoProcesso = PRONTO; // Define o estado como pronto
            processoNaCPU->tempoCPU += cpu->fatiaQuantum; // Atualiza o tempo de CPU do processo

            // No Round Robin, não há alteração de prioridade - apenas reenfileira no final
            enfileirar(processoNaCPU->pid, NUMEROVAZIO, filaRR); // Reenfileira o processo no final da fila RR
            zeraCPU(cpu); // Libera a CPU
        }
        else if (processoNaCPU->estadoProcesso == BLOQUEADO) // Caso o processo esteja bloqueado
        {
            processoNaCPU->tempoCPU += cpu->fatiaQuantum;
            zeraCPU(cpu);

            if (*processoNaCPU->pc == NUMEROVAZIO) // Remove o processo se ele terminou
            {
                removeDaTabela(tabelaProcessos, processoNaCPU->pid);
            }
        }
    }
}

// Verifica e desbloqueia processos bloqueados, se necessario
void verificaBloqueados(GerenciadorProcessos *gerenciador){
    for (int i = 0; i < gerenciador->estadoBloqueado->Tamanho; i++)
    {
        PidTempo *pidTempo = desenfileirar(gerenciador->estadoBloqueado); // Obtem o proximo processo bloqueado

        pidTempo->tempoExecutado--; // Decrementa o tempo de bloqueio

        if (pidTempo->tempoExecutado <= 0) // Se o tempo de bloqueio acabou
        {
            ProcessoSimulado *processo = buscaProcesso(gerenciador->tabelaProcessos, pidTempo->pid);
            
            // Verifica o tipo de escalonamento para decidir onde reenfileirar
            if (gerenciador->tipoEscalonamento == ESC_ROBIN) {
                // Para Round Robin, reenfileira na fila única
                enfileirar(pidTempo->pid, NUMEROVAZIO, gerenciador->filaRoundRobin);
            } else {
                // Para filas múltiplas, reenfileira na fila de prioridade correspondente
                enfileirar(pidTempo->pid, NUMEROVAZIO, gerenciador->estadoPronto[processo->prioridade]);
            }
        }
        else
        {
            enfileirar(pidTempo->pid, pidTempo->tempoExecutado, gerenciador->estadoBloqueado); // Reenfileira o processo bloqueado
        }
    }
}

//NAO ENTENDI ISSO DQ 
// Funcao para adicionar um processo ao escalonamento correto
void adicionaProcessoEscalonamento(GerenciadorProcessos *gerenciador, ProcessoSimulado *processo)
{
    if (gerenciador->tipoEscalonamento == ESC_ROBIN) {
        // Para Round Robin, adiciona na fila única
        enfileirar(processo->pid, NUMEROVAZIO, gerenciador->filaRoundRobin);
    } else {
        // Para filas múltiplas, adiciona na fila de prioridade correspondente
        enfileirar(processo->pid, NUMEROVAZIO, gerenciador->estadoPronto[processo->prioridade]);
    }
}


// Calcula a potencia de um numero
double calcularPotencia(double base, int expoente)
{
    double resultado = 1.0;

    for (int i = 0; i < expoente; i++)
    {
        resultado = resultado * base;
    }

    return resultado;
}