#include <stdlib.h>
#include <sys/time.h>
#include <stdio.h>
#define max 10

typedef struct Celula_str *Apontador;

typedef int TipoChave;

typedef struct TipoItem {
  TipoChave Chave;
  /* outros componentes */
} TipoItem;

typedef struct Celula_str {
  TipoItem Item;
  Apontador Prox;
} Celula;

typedef struct Fila {
  Apontador Frente, Tras;
} Fila;

void FFVazia(Fila *Fila)
{
  Fila->Frente = (Apontador) malloc(sizeof(Celula));
  Fila->Tras = Fila->Frente;
  Fila->Frente->Prox = NULL;
}  /* FFVazia */

int Vazia(Fila Fila)
{
  return (Fila.Frente == Fila.Tras);
}  /* Vazia */


void Enfileira(TipoItem x, Fila *Fila)
{
  Fila->Tras->Prox = (Apontador) malloc(sizeof(Celula));
  Fila->Tras = Fila->Tras->Prox;
  Fila->Tras->Item = x;
  Fila->Tras->Prox = NULL;
}  /* Enfileira */


void Desenfileira(Fila *Fila, TipoItem *Item)
{
  Apontador q;
  if (Vazia(*Fila)) 
  { printf(" Erro   fila est  a  vazia\n");
    return;
  }
  q = Fila->Frente;
  Fila->Frente = Fila->Frente->Prox;
  *Item = Fila->Frente->Item;
  free(q);
}  /* Desenfileira */

void Imprime(Fila Fila)
{
  Apontador Aux;
  Aux = Fila.Frente->Prox;
  while (Aux != NULL) 
    { printf("%12ld\n", Aux->Item.Chave);
      Aux = Aux->Prox;
    }
}


int main(int argc, char *argv[])
{ 
  struct timeval t;

  Fila fila;
  TipoItem item;
  int vetor[max];
  Apontador p;
  int i, j, k, n;

  gettimeofday(&t,NULL);
  srand((unsigned int)t.tv_usec);

  FFVazia(&fila);
  
  /*Gera uma permutacao aleatoria de chaves entre 1 e max*/
  for(i=0;i<max;i++) vetor[i] = i+1;
  for(i=0;i<max;i++)
    { k =  (int) (10.0*rand()/(RAND_MAX+1.0));

      j =  (int) (10.0*rand()/(RAND_MAX+1.0));
      n = vetor[k];
      vetor[k] = vetor[j];
      vetor[j] = n;
    }
  /*Insere cada chave na lista */
  for (i=0;i<max;i++)
    { item.Chave = vetor[i];
      Enfileira(item, &fila);
      printf("Enfileirou: %d \n", item.Chave);
    }
  /*Desenfieleira cada chave */
  for (i=0;i<max;i++)
    { Desenfileira(&fila, &item);
      printf("Desenfileirou: %d \n", item.Chave);
    }
  return 0;
}