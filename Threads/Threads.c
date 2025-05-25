#include "Threads.h"


void* thread_controle(void* arg) {
    DadosGlobais* dados = (DadosGlobais*)arg; // Converte argumento para ponteiro de DadosGlobais
    char comando; 
    
    while (dados->sistema_ativo) {
        // Le comando do arquivo se opcao == 2, senao le do usuario
        if (dados->opcao == 2) {
            comando = controle(dados->arquivo, dados->opcao); // Le comando do arquivo
            /*if (comando == 'E') { // Se comando for 'E', converte para 'M'
                comando = 'M';
            }*/
        } else {
        
            fflush(stdout); // Garante que a mensagem seja exibida
            scanf(" %c", &comando); 
        }

        // Envia comando para o buffer compartilhado e espera processamento
        pthread_mutex_lock(&dados->buffer->mutex); // Bloqueia mutex do buffer
        
        dados->buffer->comando = comando; // Armazena comando no buffer
        dados->buffer->comando_disponivel = 1; // Indica que comando esta disponivel
        dados->buffer->comando_processado = 0; // Indica que ainda nao foi processado
        
        pthread_cond_signal(&dados->buffer->cond_comando); // Notifica que comando esta disponivel
        
        // Aguarda ate que o comando seja processado
        while (!dados->buffer->comando_processado && dados->sistema_ativo) {
            pthread_cond_wait(&dados->buffer->cond_processado, &dados->buffer->mutex);
        }
        
        pthread_mutex_unlock(&dados->buffer->mutex); // Libera mutex do buffer
        
        if (comando == 'M') { // Se comando for 'M', encerra o sistema
            dados->sistema_ativo = 0;
            break;
        }
    }
    
    return NULL; // Finaliza a thread
}

// Funcao responsavel por processar comandos recebidos
void* thread_gerenciador(void* arg) {
    DadosGlobais* dados = (DadosGlobais*)arg; 
    char comando; 
    
    while (dados->sistema_ativo) { 
        pthread_mutex_lock(&dados->buffer->mutex); // Bloqueia mutex do buffer
        
        // Aguarda ate que um comando esteja disponivel
        while (!dados->buffer->comando_disponivel && dados->sistema_ativo) {
            pthread_cond_wait(&dados->buffer->cond_comando, &dados->buffer->mutex);
        }
        
        if (!dados->sistema_ativo) { // Se sistema nao esta mais ativo, libera mutex e sai
            pthread_mutex_unlock(&dados->buffer->mutex);
            break;
        }
        
        comando = dados->buffer->comando; 
        dados->buffer->comando_disponivel = 0; // Marca comando como nao disponivel
        
        pthread_mutex_unlock(&dados->buffer->mutex); // Libera mutex do buffer
        
        // Processa o comando recebido
        if (comando == 'U') {
            pthread_mutex_lock(&dados->mutex_gerenciador); // Bloqueia mutex do gerenciador
            gerenciadorProcessos(dados->gerenciador, comando); 
            pthread_mutex_unlock(&dados->mutex_gerenciador); // Libera mutex do gerenciador
        }

        else if (comando == 'I') {
            pthread_t tid_impressao; // Identificador da thread de impressao
            
            sem_wait(&dados->sem_impressao); // Aguarda semaforo de impressao (Funçao interna)
            
            // Cria thread para impressao
            if (pthread_create(&tid_impressao, NULL, thread_impressao, dados) == 0) {
                pthread_join(tid_impressao, NULL);
            } else {
                printf("ERRO: Falha ao criar thread de impressão\n"); 
            }
            
            sem_post(&dados->sem_impressao); // Libera semaforo de impressao
        }
        else if (comando == 'M') {
            sem_wait(&dados->sem_impressao);  // Aguarda semaforo de impressao (Funçao interna)
            
            sem_post(&dados->sem_impressao); // Libera semaforo de impressao
            dados->sistema_ativo = 0; // Marca sistema como inativo
        }
        
        // Notifica que o comando foi processado
        pthread_mutex_lock(&dados->buffer->mutex); // Bloqueia mutex do buffer
        dados->buffer->comando_processado = 1; // Marca comando como processado
        pthread_cond_signal(&dados->buffer->cond_processado); // Sinaliza processamento concluido
        pthread_mutex_unlock(&dados->buffer->mutex); // Libera mutex do buffer
        
        if (comando == 'M') { // Se comando for 'M', encerra o loop
            break;
        }
    }
    
    return NULL; 
}

// Funcao da thread de impressao, responsavel por imprimir o estado do gerenciador
void* thread_impressao(void* arg) {
    DadosGlobais* dados = (DadosGlobais*)arg; 
    
    pthread_mutex_lock(&dados->mutex_gerenciador); // Bloqueia mutex do gerenciador
    
    if (dados->opcao == 2) { 
        impressaoArquivo(dados->gerenciador);
    } else { 
        ImprimeGerenciadorProcessos(dados->gerenciador);
    }
    
    pthread_mutex_unlock(&dados->mutex_gerenciador); // Libera mutex do gerenciador
    
    return NULL; 
}