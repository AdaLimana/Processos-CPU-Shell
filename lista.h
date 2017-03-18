
#include "processo.h"
#include<stdio.h>

/*
  Implementacao de uma estrutura
  de dados do tipo lista encadeada
*/
typedef struct{
	processo *memoria;
	int *prox;
}no;

typedef struct{

	int *primeiro;
	int *ultimo;
	int tamanho;
}lista;

void inicializa(lista *l);

int tamanho(lista *l);

no* insereinicio(lista *l, processo *novo);

no* inserefim(lista *l, processo *novo);

no* insereem(lista *l, processo *novo, int posicao);

int excluiInicio(lista *l);

int excluiEm(lista *l, int posicao);

void liberalista(lista *l);

no* retornaNoPosicao(lista *l, int posicao);

int invertelista(lista *l);

void print(lista *l);
