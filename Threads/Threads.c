#include "Threads.h"

void* thread_controle(void* arg) {
    DadosGlobais* dados = (DadosGlobais*)arg;
    char comando; 
    
    while (dados->sistema_ativo) {
        // Le comando do arquivo se opcao == 2, senao le do usuario
        if (dados->opcao == 2) {
            comando = controle(dados->arquivo, dados->opcao);
        } else {
            fflush(stdout);
            scanf(" %c", &comando); 
        }

        // Envia comando para o buffer compartilhado e espera processamento
        pthread_mutex_lock(&dados->buffer->mutex);
        
        dados->buffer->comando = comando;
        dados->buffer->comando_disponivel = 1;
        dados->buffer->comando_processado = 0;
        
        pthread_cond_signal(&dados->buffer->cond_comando);
        
        // Aguarda ate que o comando seja processado
        while (!dados->buffer->comando_processado && dados->sistema_ativo) {
            pthread_cond_wait(&dados->buffer->cond_processado, &dados->buffer->mutex);
        }
        
        pthread_mutex_unlock(&dados->buffer->mutex);
        
        if (comando == 'M') {
            dados->sistema_ativo = 0;
            break;
        }
    }
    
    return NULL;
}

void* thread_gerenciador(void* arg) {
    DadosGlobais* dados = (DadosGlobais*)arg; 
    char comando; 
    
    while (dados->sistema_ativo) { 
        pthread_mutex_lock(&dados->buffer->mutex);
        
        // Aguarda ate que um comando esteja disponivel
        while (!dados->buffer->comando_disponivel && dados->sistema_ativo) {
            pthread_cond_wait(&dados->buffer->cond_comando, &dados->buffer->mutex);
        }
        
        if (!dados->sistema_ativo) {
            pthread_mutex_unlock(&dados->buffer->mutex);
            break;
        }
        
        comando = dados->buffer->comando; 
        dados->buffer->comando_disponivel = 0;
        
        pthread_mutex_unlock(&dados->buffer->mutex);
        
        // Processa o comando recebido baseado no algoritmo escolhido
        if (comando == 'U') {
            pthread_mutex_lock(&dados->mutex_gerenciador);
            
            if (dados->opcaoEscalonamento == 1) {
                // Filas Múltiplas
                gerenciadorProcessos(dados->gerenciador, comando);
                printf("⏰ [FM] Tempo: %d\n", dados->gerenciador->tempo);
            } else {
                // Round Robin
                gerenciadorProcessosRR(dados->gerenciadorRR, comando);
                printf("⏰ [RR] Tempo: %d\n", dados->gerenciadorRR->tempo);
            }
            
            pthread_mutex_unlock(&dados->mutex_gerenciador);
        }
        else if (comando == 'I') {
            pthread_t tid_impressao;
            
            sem_wait(&dados->sem_impressao);
            
            if (pthread_create(&tid_impressao, NULL, thread_impressao, dados) == 0) {
                pthread_join(tid_impressao, NULL);
            } else {
                printf("ERRO: Falha ao criar thread de impressão\n"); 
            }
            
            sem_post(&dados->sem_impressao);
        }
        else if (comando == 'M') {
            sem_wait(&dados->sem_impressao);
            
            // Imprime tempo medio baseado no algoritmo
            pthread_mutex_lock(&dados->mutex_gerenciador);
            if (dados->opcaoEscalonamento == 1) {
                imprimeTempoMedioResposta(dados->gerenciador);
            } else {
                imprimeTempoMedioRespostaRR(dados->gerenciadorRR);
            }
            pthread_mutex_unlock(&dados->mutex_gerenciador);
            
            sem_post(&dados->sem_impressao);
            dados->sistema_ativo = 0;
        }
        
        // Notifica que o comando foi processado
        pthread_mutex_lock(&dados->buffer->mutex);
        dados->buffer->comando_processado = 1;
        pthread_cond_signal(&dados->buffer->cond_processado);
        pthread_mutex_unlock(&dados->buffer->mutex);
        
        if (comando == 'M') {
            break;
        }
    }
    
    return NULL; 
}

void* thread_impressao(void* arg) {
    DadosGlobais* dados = (DadosGlobais*)arg; 
    
    pthread_mutex_lock(&dados->mutex_gerenciador);
    
    printf("\n📋 Gerando relatório do sistema...\n");
    
    if (dados->opcaoEscalonamento == 1) {
        // Filas Multiplas
        if (dados->opcao == 2) { 
            impressaoArquivo(dados->gerenciador);
        } else { 
            ImprimeGerenciadorProcessos(dados->gerenciador);
        }
    } else {
        // Round Robin
        if (dados->opcao == 2) { 
            impressaoArquivoRR(dados->gerenciadorRR);
        } else { 
            ImprimeGerenciadorProcessosRR(dados->gerenciadorRR);
        }
    }
    
    pthread_mutex_unlock(&dados->mutex_gerenciador);
    
    return NULL; 
}