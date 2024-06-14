#include "funcoes.h"
//#define MAX_PLAYERS 100

char** primeiro_periodo(){
    
    static char* lista_nomes[MAX_PLAYERS] = {
        "Alerrandro Moura",
        "Alex Cristhian",
        "Alice Mariana",
        "Ana Caroliny",
        "Ana Kelry",
        "Anna Leticia",
        "Aretuza Raquely",
        "Bruno Eduardo",
        "Camille Vitoria",
        "Clara Cauane",
        "Daniel Holanda",
        "Davi Gledson",
        "Eduardo Estevam",
        "Edvan Leite",
        "Elânio Jonas",
        "Estela Viana",
        "Felipe Rezes",
        "Gabriel Arthur",
        "Gabriel Oliveira",
        "Gabriel Henrique",
        "Gabriel Oliveira",
        "Gustavo Morais",
        "Heitor Medeiros",
        "Igor Gaudencio",
        "Ivan Sousa",
        "Jaryson Simao",
        "Jeyson Felicio",
        "Joao Heitor",
        "Joao Lucas",
        "Jose Cardoso",
        "Joshuan Estevao",
        "Kayky Marcelo",
        "Lucas Nataniel",
        "Luiz Fernando",
        "Miguel Silvano",
        "Pamela Wallesca",
        "Reginaldo Batista",
        "Rennan Cristian",
        "Roque Crisostomo",
        "Rudson Vinicius",
        "Sthefanny Barbosa",
        "Tales Gabriel",
        "Thafine Jennifer",
        "Victor Felipe",
        "Vinicius Holanda",
        "Vinicius Samuel",
        "Vitoria Eloise",
        "Vitoria Regina",
    };

    return lista_nomes;
}

char** professores(){
    
    static char* lista_nomes[MAX_PLAYERS] = {
        "Ceres Germanna",
        "Allyson Menden",
        "Max Lopes",
        "Antonio Oliveira",
        "Henrique"
        
    };

    return lista_nomes;
}
int soma(int a){
    return a;
}

int tamanho_matriz_ponteiros(char** lista){
     int tamanho = 0;
    while (lista[tamanho] != 0) {
        tamanho++;
    }
    return tamanho ;
}
int tamanho_matriz_normal(char* **lista){
     int tamanho = 0;
    while (lista[tamanho] != 0) {
        tamanho++;
    }
    return tamanho ;
}