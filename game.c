#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>

#include "funcoes.h"
#include "funcoes.c"

//#define MAX_PLAYERS 100
int main(){
	
setlocale(LC_ALL,"Portuguese_Brazil");
    // Conhecimentos necessarios

    // LISTAS (ADICAO,REMOÇÃO,CONTAGEM DE COMPONENTES)
    // ESTRUTURAS DE REPETIÇÃO
    // FUNÇÃO RAND?MICA
    // FUNÇÃO DE TIME.SLEEP (FAZER O PRINTF APARECER DEVAGAR)

    //Obrigatório ter 

    //MENU
    //PAUSE
    //PONTUAÇÃO

     /*
            strcpy: Copia uma string para outra. (Biblioteca: <string.h>)

            strcmp: Compara duas strings. (Biblioteca: <string.h>)

            strcat: Concatena duas strings. (Biblioteca: <string.h>)

            srand: Inicializa o gerador de números aleatórios. (Biblioteca: <stdlib.h>)

            rand: Retorna um número inteiro aleatório. (Biblioteca: <stdlib.h>)

            time: Retorna o tempo atual. (Biblioteca: <time.h>)

            usleep: Suspende a execução por um número especificado de microssegundos. Biblioteca: <unistd.h>; 
     
     */

    //   char lista[MAX_PLAYERS][50] = {"Davi Gledson", "Aleandro Martins", "Segundo",
    //                               "Professora Ceres", "Professor Max", "Eleanio",
    //                               "Professor Antonio", "Carol", "Shefany barbosa"};
    char lista[50][20] = {};
   char **lista_de_ponteiros = primeiro_periodo();
   for(int i = 0; i <48; i++){

    //copia o conteudo da lista de ponteiros para lista de declaração normal
    strcpy(lista[i], lista_de_ponteiros[i]);
  }
    //TEM QUE CRIAR UMA FUNCAO PARA CONTAR O O CONTEUDO DA LISTA
    int num_players = 47;
    
    int impostor_index;
    char impostor[50];
    int player_index;
    char dead_player[50];
    int user_input;
    int j=0;
    
    char *boneco_among_us =
        "??????????????????\n"
        "?????????????????\n"
        "?????????????????\n"
        "?????????????????\n"
        "?????????????????\n"
        "?????????????????\n"
        "?????????????????\n"
        "?????????????????\n"
        "?????????????????\n"
        "?????????????????\n"
        "?????????????????\n";

    
    strcpy(impostor, lista[numero_aleatorio(num_players)]);

    printf("===========================\n");
    printf("Ha um impostor entre nos\n");
    printf("===========================\n");

  while (1) {
        // Encontra a posição do impostor
        for (j = 0; j < num_players; j++) {
            if (strcmp(lista[j], impostor) == 0) {
                player_index = j;
                break;
            }
        }

        // Escolhe uma vitima aleatoria
        int victim_index;
        do {
            victim_index = numero_aleatorio(num_players);
             //printf("------------%d --------\n",victim_index);
        } while (victim_index == player_index);
        strcpy(dead_player, lista[victim_index]);

       
        printf("O impostor matou %s ------ %s\n",impostor ,dead_player);
        printf("====================\n");
        
         

        // Remove a vitima da lista
        for (j = victim_index; j < num_players - 1; j++) {
            strcpy(lista[j], lista[j + 1]);
        }
        num_players--;
    
        printf("Os Sobreviventes são:");
        for (j = 0; j < num_players; j++) {
            printf(" %s", lista[j]);
            printf(" |");
        }
        printf("\n");

        // Imprime os jogadores restantes
        for (j = 0; j < num_players; j++) {
            printf("[%d] - %s\n", j, lista[j]);
        }

        // Obtom o palpite do usu?rio
        printf("Quem é o impostor? [digite de 0 até %d]: ", num_players - 1);
        //printf("%s",boneco_among_us);
        while (scanf("%d", &user_input) != 1 || user_input < 0 || user_input >= num_players) {
            while (getchar() != '\n'); // Limpa o buffer de entrada
            printf("Entrada invalida. Tente novamente. Quem é o impostor? [digite de 0 até %d]: ", num_players - 1);
        }

        // Verifica se o palpite do usuario esta correto
        if (strcmp(lista[user_input], impostor) == 0) {

            printf("====================\n");
           char frase_lenta[] = " ";
            strcat(frase_lenta,lista[user_input]);
            strcat(frase_lenta," era o impostor!");
            //printf("%s era o Impostor\n", impostor);
             for (int i = 0; frase_lenta[i] != '\0'; i++) {
            printf("%c", frase_lenta[i]);
            fflush(stdout);
            usleep(100000); // Atraso de 100 milissegundos
        }
             printf("\n");
            printf("====================\n");
            printf("Parabéns! VOCÊ GANHOU!\n");
            break;
        } else {
            printf("====================\n");
            char frase_lenta[] = " ";
            strcat(frase_lenta,lista[user_input]);
            strcat(frase_lenta," não era o impostor!");

              for (int i = 0; frase_lenta[i] != '\0'; i++) {
            printf("%c", frase_lenta[i]);
            fflush(stdout);
            usleep(100000); // Atraso de 100 milissegundos
        }
            //printf("%s não era o impostor!\n", lista[user_input]);
            printf("\n");
            printf("====================\n");
            // Remove o palpite incorreto da lista
            for (j = user_input; j < num_players - 1; j++) {
                strcpy(lista[j], lista[j + 1]);
            }
            num_players--;
        }

        // Verifica se o jogo acabou
        if (num_players <= 2) {
            printf("====================\n");
            printf("%s era o Impostor\n", impostor);
            printf("====================\n");
            printf("VOCÊ PERDEU!\n");
            break;
        }
}

}

//escolhe um numero aleatorio até o  parametro limite
int numero_aleatorio(int limite){
 srand(time(NULL));
  
    return rand() % limite;
}

