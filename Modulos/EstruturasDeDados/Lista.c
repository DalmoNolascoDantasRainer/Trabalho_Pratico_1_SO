#include "Lista.h"

// Funcao para criar uma nova lista encadeada
Lista* criaLista(){
  Lista* lista = (Lista*) malloc(sizeof(Lista));

  // Cria a celula cabeca e ajusta os ponteiros
  lista->inicio = (Celula*) malloc (sizeof (Celula));
  lista->fim = lista->inicio;
  lista->inicio->proximo = NULL;

  return lista;
}

// Funcao para verificar se a lista está vazia
int listaVazia(Lista* lista){
  // A lista esta vazia se o ponteiro de inicio for igual ao de fim
  return (lista->inicio == lista->fim);
}

// Funcao para inserir um processo na lista
void insereTabela(Lista* lista, ProcessoSimulado* processo) {
  // Cria uma nova celula no final da lista
  lista->fim->proximo = (Celula*) malloc(sizeof(Celula));
  lista->fim = lista->fim->proximo;

  // Armazena o processo na nova celula e ajusta o ponteiro
  lista->fim->processo = processo;
  lista->fim->proximo = NULL;
}

// Funcao para buscar um processo na lista pelo PID
ProcessoSimulado* buscaProcesso(Lista* lista, int PID) {
  // Percorre a lista a partir da primeira celula util (depois da celula cabeca)
  Celula* percorre;
  percorre = lista->inicio->proximo;
  while (percorre != NULL){
    // Verifica se o PID do processo atual é o procurado
    if(percorre->processo->pid == PID) {
      return percorre->processo; // Retorna o processo encontrado
    }
    percorre = percorre->proximo;
  }
  return NULL; // Retorna NULL se o processo não for encontrado
}

// Funcao para remover um processo da lista pelo PID
void removeTabela(Lista* lista, int PID) {
  // Verifica se a lista esta vazia
  if (listaVazia(lista)) {
    printf("Lista vazia. Nenhum processo para remover.\n");
    return;
  }

  // Ponteiros para percorrer a lista e manter referencia ao anterior
  Celula* anterior = lista->inicio;
  Celula* atual = lista->inicio->proximo;

  while (atual != NULL) {
    
    if (atual->processo->pid == PID) { // Verifica se o PID do processo atual e o procurado
      anterior->proximo = atual->proximo; 
      
      // Se o processo removido for o ultimo, ajusta o ponteiro de fim
      if (atual == lista->fim) {
        lista->fim = anterior;
      }

      free(atual); // Libera a memoria da celula removida
      return;
    }

    anterior = atual; // Avanca o ponteiro anterior
    atual = atual->proximo; // Avanca o ponteiro atual
  }

  printf("Processo com PID %d não encontrado na lista.\n", PID);
}

// Funcao para encontrar o maior PID na lista
int maiorPIDTabela(Lista* lista) {
  Celula* percorre;
  int maiorPID = 0;

  percorre = lista->inicio->proximo;
  while (percorre != NULL)
  {
    
    if(percorre->processo->pid >= maiorPID){ // Atualiza o maior PID encontrado 
      maiorPID = percorre->processo->pid;
    }
    percorre = percorre->proximo; // Avança para a proxima celula
  }
  return maiorPID; // Retorna o maior PID encontrado
}


//// REMOVER ISSO E DEIXAR NO PROCESSO DE IMPRESSAO ////
// Funcao para imprimir todos os processos da lista
void imprimeTabela(Lista* lista) {
  Celula* aux;

  aux = lista->inicio->proximo;
  while (aux != NULL)
  {
    // Chama a funcao para imprimir o processo
    imprimeProcesso(*(aux->processo), 1);
    aux = aux->proximo; // Avanca para a proxima celula
  }
}