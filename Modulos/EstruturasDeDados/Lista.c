#include "Lista.h"

// Funcao para criar uma nova lista encadeada
Lista* criaLista(){
  Lista* lista = (Lista*) malloc(sizeof(Lista));

  // Cria a celula cabeca e ajusta os ponteiros
  lista->Primeiro = (CelulaPtr) malloc (sizeof (CelulaProcessoSimulado));
  lista->Ultimo = lista->Primeiro;
  lista->Primeiro->Prox = NULL;

  return lista;
}

// Funcao para verificar se a lista está vazia
int listaEhVazia(Lista* lista){
  // A lista esta vazia se o ponteiro de inicio for igual ao de fim
  return (lista->Primeiro == lista->Ultimo);
}