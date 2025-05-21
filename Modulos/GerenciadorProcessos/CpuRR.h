
// COLOQUEI O ROUND ROBIN SÓ PRA GNT TER UMA IDEIA DO ALG. DPS

// Função que implementa o algoritmo de escalonamento Round Robin
void roundRobin(Fila* fila, int quantum) {
    
    // Enquanto houver processos na fila
    while (fila->frente != NULL) {
        
        // Remove o processo da frente da fila
        Processo* p = desenfileira(fila);

        // Define o tempo de execução: é o menor entre o tempo restante e o quantum
        int tempoExec = (p->tempoRestante < quantum) ? p->tempoRestante : quantum;

        // Mostra na tela qual processo está sendo executado e por quanto tempo
        printf("Executando processo %d por %d unidades de tempo.\n", p->pid, tempoExec);

        // Reduz o tempo restante do processo pelo tempo que ele acabou de executar
        p->tempoRestante -= tempoExec;

        // Se o processo ainda não terminou
        if (p->tempoRestante > 0) {
            // Coloca o processo de volta no final da fila, com o tempo restante atualizado
            enfileira(fila, p->pid, p->tempoRestante);
            
            // Libera a memória da struct temporária, já que enfileiramos uma nova
            free(p);
        } else {
            // Se o processo terminou, imprime uma mensagem
            printf("Processo %d finalizado.\n", p->pid);
            
            // Libera a memória do processo
            free(p);
        }
    }
} */
