#include "Threads.h"

void* thread_controle(void* arg) {
    DadosGlobais* dados = (DadosGlobais*)arg;
    char comando;
    
    while (dados->sistema_ativo) {
        // Lê comando
        if (dados->opcao == 2) {
            comando = controle(dados->arquivo, dados->opcao);
            if (comando == 'E') {
                comando = 'M';
            }
        } else {
            printf("Digite comando (U/I/M): ");
            fflush(stdout);
            scanf(" %c", &comando);
        }
        printf("Comando recebido: %c\n", comando);
        // Envia comando e espera processamento
        pthread_mutex_lock(&dados->buffer->mutex);
        
        dados->buffer->comando = comando;
        dados->buffer->comando_disponivel = 1;
        dados->buffer->comando_processado = 0;
        
        // Notifica que comando está disponível
        pthread_cond_signal(&dados->buffer->cond_comando);
        
        // Aguarda processamento completo
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
        // Espera comando
        pthread_mutex_lock(&dados->buffer->mutex);
        
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
        
        // Processa comando
        if (comando == 'U') {
            pthread_mutex_lock(&dados->mutex_gerenciador);
            gerenciadorProcessos(dados->gerenciador, comando);
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
            pthread_t tid_impressao;
            sem_wait(&dados->sem_impressao);
            
            if (pthread_create(&tid_impressao, NULL, thread_impressao, dados) == 0) {
                pthread_join(tid_impressao, NULL);
            }
            
            sem_post(&dados->sem_impressao);
            dados->sistema_ativo = 0;
        }
        
        // Notifica que processamento foi concluído
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
    
    if (dados->opcao == 2) {
        impressaoArquivo(dados->gerenciador);
    } else {
        ImprimeGerenciadorProcessos(dados->gerenciador);
    }
    
    pthread_mutex_unlock(&dados->mutex_gerenciador);
    
    return NULL;
}