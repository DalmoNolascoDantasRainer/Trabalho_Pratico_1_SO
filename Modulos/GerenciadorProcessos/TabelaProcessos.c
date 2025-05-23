#include "TabelaProcessos.h"

// Funcao para inserir um processo na lista
void insereNaTabela(ProcessoSimulado* processo, Lista* lista) {
  // Cria uma nova celula no final da lista
  lista->Ultimo->Prox = (CelulaPtr) malloc(sizeof(CelulaProcessoSimulado));
  lista->Ultimo = lista->Ultimo->Prox;

  // Armazena o processo na nova celula e ajusta o ponteiro
  lista->Ultimo->processo = processo;
  lista->Ultimo->Prox = NULL;
}

// Funcao para buscar um processo na lista pelo PID
ProcessoSimulado* buscaProcesso(Lista* lista, int PID) {
  // Percorre a lista a partir da primeira celula util (depois da celula cabeca)
  CelulaPtr atual;
  atual = lista->Primeiro->Prox;
  while (atual != NULL){
    // Verifica se o PID do processo atual é o procurado
    if(atual->processo->pid == PID) {
      return atual->processo; // Retorna o processo encontrado
    }
    atual = atual->Prox;
  }
  return NULL; // Retorna NULL se o processo não for encontrado
}

// Funcao para remover um processo da lista pelo PID
void removeDaTabela(Lista* lista, int PID) {
  // Verifica se a lista esta vazia
  if (listaEhVazia(lista)) {
    printf("Lista esta vazia, nao ha processo para remover.\n");
    return;
  }

  // Ponteiros para percorrer a lista e manter referencia ao anterior
  CelulaPtr anterior = lista->Primeiro; // Comeca da cabeca
  CelulaPtr atual = lista->Primeiro->Prox; // Comeca do primeiro processo


  while (atual != NULL) {
    
    if (atual->processo->pid == PID) { // Verifica se o PID do processo atual e o procurado
      anterior->Prox = atual->Prox; 
      
      // Se o processo removido for o ultimo, ajusta o ponteiro de fim
      if (atual == lista->Ultimo) {
        lista->Ultimo = anterior;
      }

      free(atual); // Libera a memoria da celula removida
      return;
    }

    anterior = atual; // Avanca o ponteiro anterior
    atual = atual->Prox; // Avanca o ponteiro atual
  }

  printf("O processo de PID %d não foi encontrado.\n", PID);
}

// Funcao para encontrar o maior PID na lista
int maiorPIDTabela(Lista* lista) {
  CelulaPtr atual = lista->Primeiro->Prox;
  int maiorPID = 0;

  while (atual != NULL)
  {
    
    if(atual->processo->pid >= maiorPID){ // Atualiza o maior PID encontrado 
      maiorPID = atual->processo->pid;
    }
    atual = atual->Prox; // Avança para a proxima celula
  }
  return maiorPID; // Retorna o maior PID encontrado
}

