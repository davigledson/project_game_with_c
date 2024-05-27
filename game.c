#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>
#define MAX_PLAYERS 100
int main(){
	
setlocale(LC_ALL,"Portuguese_Brazil");
    // Conhecimentos necess�rios

    // LISTAS (ADICAO,REMO��O,CONTAGEM DE COMPONENTES)
    // ESTRUTURAS DE REPETI��O
    // FUN��O RAND�MICA
    // FUN��O DE TIME.SLEEP (FAZER O PRINTF APARECER DEVAGAR)

    //Obrigat�rio ter 

    //MENU
    //PAUSE
    //PONTUACAO

     

      char lista[MAX_PLAYERS][50] = {"Davi Gledson", "Aleandro Martins", "Segundo",
                                  "Professora Ceres", "Professor Max", "Eleanio",
                                  "Professor Antonio", "Carol", "Shefany barbosa"};

    int num_players = 9;
    int impostor_index;
    char impostor[50];
    int player_index;
    char dead_player[50];
    int user_input;
    int j=0;
    
    char *boneco_among_us =
        "⠀⠀⠀⠀⠀⢀⣴⣶⠿⠟⠻⠿⢷⣦⣄⠀⠀⠀\n"
        "⠀⠀⠀⠀⣾⠏⠀⠀⣠⣤⣤⣤⣬⣿⣷⣄⡀\n"
        "⠀⢀⣀⣸⡿⠀⠀⣼⡟⠁⠀⠀⠀⠀⠀⠙⣷\n"
        "⢸⡟⠉⣽⡇⠀⠀⣿⡇⠀⠀⠀⠀⠀⠀⢀⣿\n"
        "⣾⠇⠀⣿⡇⠀⠀⠘⠿⢶⣶⣤⣤⣶⡶⣿⠋\n"
        "⣿⠂⠀⣿⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⠃\n"
        "⣿⡆⠀⣿⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⠀\n"
        "⢿⡇⠀⣿⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⣿⠀\n"
        "⠘⠻⠷⢿⡇⠀⠀⠀⣴⣶⣶⠶⠖⠀⢸⡟⠀\n"
        "⠀⠀⠀⢸⣇⠀⠀⠀⣿⡇⣿⡄⠀⢀⣿⠇⠀\n"
        "⠀⠀⠀⠘⣿⣤⣤⣴⡿⠃⠙⠛⠛⠛⠋⠀⠀\n";

    
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

        // Escolhe uma v?tima aleat?ria
        int victim_index;
        do {
            victim_index = numero_aleatorio(num_players);
        } while (victim_index == player_index);
        strcpy(dead_player, lista[victim_index]);

        printf("O impostor matou %s\n", dead_player);
        printf("====================\n");

        // Remove a v?tima da lista
        for (j = victim_index; j < num_players - 1; j++) {
            strcpy(lista[j], lista[j + 1]);
        }
        num_players--;

        printf("Os Sobreviventes são:");
        for (j = 0; j < num_players; j++) {
            printf(" %s", lista[j]);
        }
        printf("\n");

        // Imprime os jogadores restantes
        for (j = 0; j < num_players; j++) {
            printf("[%d] - %s\n", j, lista[j]);
        }

        // Obt?m o palpite do usu?rio
        printf("Quem ? o impostor? [digite de 0 até %d]: ", num_players - 1);
        //printf("%s",boneco_among_us);
        while (scanf("%d", &user_input) != 1 || user_input < 0 || user_input >= num_players) {
            while (getchar() != '\n'); // Limpa o buffer de entrada
            printf("Entrada inv?lida. Tente novamente. Quem é o impostor? [digite de 0 até %d]: ", num_players - 1);
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

