#include <stdio.h>

#include "funcoes.h"
#include "funcoes.c"

int main(){
	
    //char lista;
    int i;
  char **lista =  primeiro_periodo();
  char lista2[50][20];
  for(i = 0; i <tamanho_matriz_ponteiros(lista); i++){

    //printf("%d -  %s\n",i, lista[i]);
    strcpy(lista2[i], lista[i]);
  }

for(i = 0; i <47; i++){

    printf("%d -  %s\n",i, lista2[i]);
    
  }
printf("tamanho: %d",tamanho_matriz_normal(lista2));

}


