#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#define MAX 10

typedef int TipoChave;

typedef struct {
  int Chave;
  /* outros componentes */
} TipoItem;

typedef struct TipoCelula *TipoApontador;

typedef struct TipoCelula {
  TipoItem Item;
  TipoApontador Prox;
} TipoCelula;

typedef struct {
  TipoApontador Primeiro, Ultimo;
} TipoLista;

/* ========================================================================= */

void FLVazia(TipoLista *Lista)
{ Lista -> Primeiro = (TipoApontador) malloc(sizeof(TipoCelula));
  Lista -> Ultimo = Lista -> Primeiro;
  Lista -> Primeiro -> Prox = NULL;
}

int Vazia(TipoLista Lista)
{ return (Lista.Primeiro == Lista.Ultimo);
}

void Insere(TipoItem x, TipoLista *Lista)
{ Lista -> Ultimo -> Prox = (TipoApontador) malloc(sizeof(TipoCelula));
  Lista -> Ultimo = Lista -> Ultimo -> Prox;
  Lista -> Ultimo -> Item = x;
  Lista -> Ultimo -> Prox = NULL;
}

void Retira(TipoApontador p, TipoLista *Lista, TipoItem *Item)
{ /*  ---   Obs.: o item a ser retirado e  o seguinte ao apontado por  p --- */
  TipoApontador q;
  if (Vazia(*Lista) || p == NULL || p -> Prox == NULL) 
  { printf(" Erro   Lista vazia ou posi  c   a o n  a o existe\n");
    return;
  }
  q = p -> Prox;
  *Item = q -> Item;
  p -> Prox = q -> Prox;
  if (p -> Prox == NULL) Lista -> Ultimo = p;
  free(q);
}

void Imprime(TipoLista Lista)
{ TipoApontador Aux;
  Aux = Lista.Primeiro -> Prox;
  while (Aux != NULL) 
    { printf("%d\n", Aux -> Item.Chave);
      Aux = Aux -> Prox;
    }
}

/* ========================================================================== */

int main(int argc, char *argv[])
{ struct timeval t;

  TipoLista lista;
  TipoItem item;
  int vetor[MAX];
  TipoApontador p;
  int i, j, k, n;
  float  tamanho=0;
  gettimeofday(&t,NULL);
  srand((unsigned int)t.tv_usec);
  FLVazia(&lista);

  
  /*Gera uma permutacao aleatoria de chaves entre 1 e MAX*/
  for(i = 0; i < MAX; i++) vetor[i] = i + 1;
  for(i = 0; i < MAX; i++)
    { k =  (int) (10.0 * rand()/(RAND_MAX + 1.0));
      j =  (int) (10.0 * rand()/(RAND_MAX + 1.0));
      n = vetor[k];
      vetor[k] = vetor[j];
      vetor[j] = n;
    }
  /*Insere cada chave na lista */
  for (i = 0; i < MAX; i++)
    { item.Chave = vetor[i];
      Insere(item, &lista);
      tamanho++;
      printf("Inseriu: %d \n", item.Chave);
    }
  Imprime(lista);

  /*Retira cada chave da lista */
  for(i = 0; i < MAX; i++)
    { /*escolhe uma chave aleatoriamente */
      k = (int) ((tamanho) * rand() / (RAND_MAX + 1.0));
      p = lista.Primeiro;
      /*retira chave apontada */
      Retira(p, &lista, &item);
      tamanho--;
      printf("Retirou: %d\n", item.Chave);
    }
  Imprime (lista);
  return(0);
}