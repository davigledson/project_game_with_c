
#include "raylib.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h> // Incluir a biblioteca para o tipo bool
#define MAX_INPUT_CHARS 15
#define screenWidth 1200
#define screenHeight 650
#define MAX_DICAS 5

typedef enum
{
    STATE_TELA_MENU,
    STATE_TELA_GAMEPLAY,
    STATE_TELA_HISTORY,
    STATE_TELA_INPUT,
    STATE_TELA_GAMEOVER,
    STATE_TELA_VITORY,
    STATE_TELA_RANKING

} GameState;
// Estrutura para representar um botão
typedef struct
{
    Rectangle bounds;
    const char *text;
    bool hovered;
    bool clicked;
    Color bounds_color;
    char msg_referente[300];

} Button;

typedef struct
{
    float scrollingBack;
    float scrollingMid;
    float scrollingFore;
} ScrollingPositions;

typedef struct
{
    char *name_person;
    char *DeadText;
    char *suspect_text[MAX_DICAS];
    int num_hints;
} Personagem;

typedef struct {
    int index;
    bool selected;
} SuspeitoSelecionado;

// para todos os textos únicos para a tela de gameplay, (inclusive texto do gameplay)
typedef struct
{
    char theDeadText[300];
    char *suspect_msg;
    int tentativas;
    int qtdPersonagens;
    bool showDeathText;
    int culpado_index;
    int victory;
    char victory_text[300];
    char *text;
    int index_botao_clicado;
    Button botoes[9];
    int sequencia_de_acertos;
} TextForGUI;

typedef struct
{
    char *history;

} History;

typedef struct 
{
    char caminho;
    Texture2D texture1;
    Texture2D texture2;
    Texture2D texture3;
    Vector2 position;
    Color tint;
    int width;
    int height;
    int currentTexture;
} Imagem;

typedef struct
{
    char name[20];
    int hours;
    int minutes;
    int seconds;

} Ranking;

Ranking Player_ranking;

struct Update
{
    double firstTime;
    double currentTime;
    double elapsedTime; 
    int hours;
    int minutes;
    int seconds;
    bool startTime;
    bool showTime;
    bool ranking;

}Update = {0,0,0,0,0,0,true,true,true} ;

     
// Função de atualização que retorna uma estrutura com as novas posições de rolagem
_Bool IsButtonClicked(Button button);
void DesenhatextoDinamico(const char *text, int posX, int posY, int fontSize, Color color, bool show);
ScrollingPositions UpdateScrolling(ScrollingPositions positions, float backgroundWidth, float midgroundWidth, float foregroundWidth);
static void DrawTextBoxed(Font font, const char *text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint); // Draw text using font inside rectangle limits
static void DrawTextBoxedSelectable(Font font, const char *text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint, int selectStart, int selectLength, Color selectTint, Color selectBackTint);
// Protótipo da função para desenhar texto com word wrap
void DrawTextWrapped(Font font, const char *text, Rectangle rec, float fontSize, float spacing, Color tint);
void DesenhaImagemPRO(Imagem *img);
int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    srand(time(NULL));
    GameState currentState = STATE_TELA_MENU;
    ScrollingPositions positions = {0.0f, 0.0f, 0.0f};

    InitWindow(screenWidth, screenHeight, "Detetive");

    char name[MAX_INPUT_CHARS + 1] = "\0";
    int letterCount = 0;

    Rectangle textBox = {screenWidth / 2.0f - 200, 180, 380, 50};
    
    bool IsRankingState = false;
    bool mouseOnText = false;
    bool IsgameoverState = false;

    int framesCounter = 0;

    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------
    Texture2D background = LoadTexture("resources/cyberpunk_street_background.png");
    Texture2D midground = LoadTexture("resources/cyberpunk_street_midground.png");
    Texture2D foreground = LoadTexture("resources/cyberpunk_street_foreground.png");
    Texture2D inter_room = LoadTexture("imgs/cena_crime.png");
    Texture2D background_history = LoadTexture("imgs/historiatema.png");
    Texture2D gameoverTex = LoadTexture("imgs/ana.png");
    Texture2D background_victory = LoadTexture("imgs/background_gameover.png");
    Texture2D rankingTex;

    Imagem cats;
    cats.texture1 = LoadTexture("resources/cat_chocado.png");
    cats.texture2 = LoadTexture("resources/cat_victory.png");
    cats.texture3 = LoadTexture("resources/cat_serio.png");
    cats.position = (Vector2){ 800, 300 };
    cats.tint = WHITE;
   
    cats.width = 300;
    cats.height = 300;
    Font rankingFont = LoadFont("fonts/alagard.ttf");

    // IMPLEMENTA A LOGICA DAS INTERFACES
    InitAudioDevice();

    Music music = LoadMusicStream("sounds/amoung_theme.mp3");
    Music rankingMusic = LoadMusicStream("sounds/rankingMusic.mp3");
    Music gameovermusic = LoadMusicStream("sounds/gameoversong.mp3");

    PlayMusicStream(music); // Iniciar a reprodução da música

    // Alteração:  para não inicializar a tela de menu com uma parte da tela preta
    positions.scrollingBack = -background.width; // Centralizar a textura de fundo
    positions.scrollingMid = -midground.width;
    positions.scrollingFore = -foreground.width;

    TextForGUI textGameOver;
    TextForGUI textVictory;
    // Variaveis == textos para a tela de gameplay
    TextForGUI textGameGUI;
    textGameGUI.tentativas = 3;
    textGameGUI.qtdPersonagens = 9;
    textGameGUI.showDeathText = false;

    textGameGUI.victory = 0;
    int buttonCount = textGameGUI.qtdPersonagens;
    int indice_do_culpado = rand() % buttonCount;
    textGameGUI.culpado_index = indice_do_culpado;
    Button buttons[buttonCount];

    Personagem persons[] = {
        // nome -- frase ao ser selecionado -- dica do suspeito
        {"Prof. Maximiliano araujo", "fui de Vasco", {"fez a faculdade no UFRN","Teve um professor pagodeiro em sua época de faculdade"}, 2},
        {"Prof. HENRIQUE", "", {"Seu cargo anterior era de analista de sistemas","Sua esposa é professora no IFRN"}, 2},
        {"Prof. CERES", "como é o nome de Talless?", {"Recebeu 10 em sua apresentação de TCC, em sua época de faculdade"}, 1},
        {"REGINALDO BATISTA", "", {"do 1° Período, Sua frase no status do whats \"é só sei que nada sei\""}, 1},
        {"DANIEL LIRA", "", {"do 1° Período, A menor nota em matemática foi 10 até agora"}, 1},
        {"Prof. FRANKLIN", "Estudeeee", {"do departamento de Matemática,Durante sua época de faculdade, trancou alguns períodos para trabalhar"}, 1},
        {"Prof. DARIO", "Meus alunos, meus amigos", {"E o inimigo numero 1 do INSS","Usa o mesmo slide há mais de 10 anos"}, 2},
        {"Prof. HEITOR", "Cade o modo fácil do DarkSoul?", {"Tem Fascínio por jogos"}, 1},
        {"Prof. ALYSSON MENDES", "E culpa do Windows", {"Prefere Linux e gosta de Python"}, 1},
        {"MAGNUS", "Tranquem o Curso!", {"Provavelmente o próximo aluno laureado","Tem como nome um dos maiores jogadores de xadrez"}, 2},
        {"Prof. ANTÔNIO OLIVEIRA","Eu seria mais feliz no tempo antes da escrita", {"Inimigo do Python e da linguagem C", "Inimigo da Maçonaria", "Inimigo da NASA"}, 3},
        {"EDVAN LEITE", "", {"do 1° Período, Quem tem intolerância a lactose não é amigo dele"}, 1},
        {"GABRIEL ARTHUR", "Lá ele", {"Imitador do 1° Período de Ciência da Computação","Amante da NASA By Professor Antônio"}, 2},
        {"TALES GABRIEL", "Bora para o R.U", {"do 1° Período, Homem dos olhos de vidros By Professor Antônio"}, 1},
        {"ELANIO JONAS", "Que isso, meu parceiro", {"do 1° Período, 50% economista"}, 1},
        {"Alerrandro Martins","", {"Do 1° Período, e de Tibal e tem domínio em POG, \"Programação Orientada a Gambiarras\""}, 1}
        
        };
    // embaralhar  todo o array
    int totalCharacters = sizeof(persons) / sizeof(persons[0]);

    embaralhar(persons, totalCharacters);
    // Variaveis == textos para a tela de historia
    TextForGUI textHistory;
   History history_context[] = {
    
    {"No laboratório de informática da UERN, vários computadores estão com a foto de Kpops, o que se sabe sobre o culpado é que:"},
    {"Na UERN, alguém está tentando derrubar os pilares do setor da FANAT, o que se sabe sobre o culpado é que:"},
    {"Na UERN, alguém deixou a porta de aula aberta fazendo com que vários animais entre, o que se sabe sobre o culpado é que:"},
    {"Na UERN, alguém fez um gato na energia fazendo com que tivesse um apagão durante dois dias, o que se sabe sobre o culpado é que:"},
    {"Na UERN, alguém derrubou o Wi-Fi mais uma vez, o que se sabe sobre o culpado é que:"},
};

   int total_history_context = sizeof(history_context) / sizeof(history_context[0]);

    int indice_da_history = rand() % total_history_context;
    textHistory.text = history_context[indice_da_history].history;
    textHistory.culpado_index = indice_do_culpado;
    int indice_da_dica_do_culpado = rand() % persons[indice_do_culpado].num_hints;
    textHistory.suspect_msg = persons[indice_do_culpado].suspect_text[indice_da_dica_do_culpado];
    textGameGUI.suspect_msg = persons[indice_do_culpado].suspect_text[indice_da_dica_do_culpado];
    textGameGUI.sequencia_de_acertos =0;

    // // o for limita o  números de personagem
    // for (int i = 0; i < buttonCount; i++)
    // {

    //     buttons[i].bounds = (Rectangle){10, 80 + i * 50, 280, 40};
    //     buttons[i].text = persons[i].name_person;
    //     strcpy(buttons[i].msg_referente, persons[i].DeadText);
    //     buttons[i].bounds_color = BLUE;
    //     buttons[i].hovered = 1;
    // }
      for (int i = 0; i < buttonCount; i++)
    {

        textGameGUI.botoes[i].bounds = (Rectangle){10, 80 + i * 50, 280, 40};
        textGameGUI.botoes[i].text = persons[i].name_person;
        strcpy(textGameGUI.botoes[i].msg_referente, persons[i].DeadText);
        textGameGUI.botoes[i].bounds_color = BLUE;
        textGameGUI.botoes[i].hovered = 1;
    }
   // renderizarBotoesPersonagens(buttons, buttonCount, persons); 

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        if (currentState != STATE_TELA_RANKING)
        {
           if(IsRankingState)
           {
               StopMusicStream(rankingMusic);
               PlayMusicStream(music);
               UnloadTexture(rankingTex);
               ClearBackground(BLACK);
               IsRankingState = false; // Atualizar a variável para indicar que a música de ranking não está mais tocando

           }else{

                UpdateMusicStream(music);
           }
        }

        if (currentState != STATE_TELA_GAMEOVER)
        {
           if(IsgameoverState)
           {
               StopMusicStream(gameovermusic);
               PlayMusicStream(music);
               UnloadTexture(gameoverTex);
               ClearBackground(BLACK);
               IsgameoverState = false; // Atualizar a variável para indicar que a música de ranking não está mais tocando

           }else{

                UpdateMusicStream(music);
           }
        }
        positions = UpdateScrolling(positions, background.width, midground.width, foreground.width);
        switch (currentState)
        {
        case STATE_TELA_MENU:

            break;
        case STATE_TELA_INPUT:
            if (CheckCollisionPointRec(GetMousePosition(), textBox))
                mouseOnText = true;
            else
                mouseOnText = false;

            if (mouseOnText)
            {
                // Set the window's cursor to the I-Beam
                SetMouseCursor(MOUSE_CURSOR_IBEAM);

                // Get char pressed (unicode character) on the queue
                int key = GetCharPressed();

                // Check if more characters have been pressed on the same frame
                while (key > 0)
                {
                    // NOTE: Only allow keys in range [32..125]
                    if ((key >= 32) && (key <= 125) && (letterCount < MAX_INPUT_CHARS))
                    {
                        name[letterCount] = (char)key;
                        name[letterCount + 1] = '\0'; // Add null terminator at the end of the string.
                        letterCount++;
                    }

                    key = GetCharPressed(); // Check next character in the queue
                }

                if (IsKeyPressed(KEY_BACKSPACE))
                {

                    letterCount--;
                    if (letterCount < 0)
                        letterCount = 0;
                    name[letterCount] = '\0';
                }
            }
            else
                SetMouseCursor(MOUSE_CURSOR_DEFAULT);

            if (mouseOnText)
                framesCounter++;
            else
                framesCounter = 0;
            //----------------------------------------------------------------------------------

             if (IsKeyPressed(KEY_ENTER))
            {
                strcpy(Player_ranking.name, name);
                currentState = STATE_TELA_HISTORY;
            }

            break;

        case STATE_TELA_GAMEPLAY:

            if (Update.startTime)
            {
                Update.firstTime = GetTime();
                Update.startTime = false;
            }

            Update.currentTime = GetTime();
            Update.elapsedTime = Update.currentTime - Update.firstTime;

            Update.hours = (int)(Update.elapsedTime / 3600);
            Update.minutes = (int)(Update.elapsedTime / 60) - (Update.hours * 60);
            Update.seconds = (int)Update.elapsedTime - (Update.hours * 3600) - (Update.minutes * 60);
            

            break;
        
        case STATE_TELA_GAMEOVER:
        
            if (!IsgameoverState)
                {
                    StopMusicStream(music); 
                    PlayMusicStream(gameovermusic);
                    IsgameoverState = true; 

                }else{

                    UpdateMusicStream(gameovermusic); 
                }
            

            break;

        case STATE_TELA_RANKING:

            if (!IsRankingState)
                {
                    StopMusicStream(music); // Parar a música atual
                    PlayMusicStream(rankingMusic); // Iniciar a reprodução da música de ranking
                    rankingTex = LoadTexture("resources/ranking.png");
                    IsRankingState = true; // Marcar que a música de ranking está tocando
                }
                else
                {
                    UpdateMusicStream(rankingMusic); // Atualizar a stream da música de ranking
                }
            
        }
        // Update
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        switch (currentState)
        {
        case STATE_TELA_MENU:
            // DrawTexture(background, 0, 0, WHITE);
            menuGUI(background, midground, foreground, positions, &currentState);

            break;
        case STATE_TELA_INPUT:
            
            inputGUI(name, letterCount, textBox, mouseOnText, framesCounter, background, midground, foreground, positions, &currentState);
            break;
        case STATE_TELA_GAMEPLAY:
            
          gameGUI(&currentState, buttons, &buttonCount, inter_room, &framesCounter, &textGameGUI,&cats,&persons);
            break;
        case STATE_TELA_HISTORY:
            
            historyGUI(&textHistory, background_history, &framesCounter, &currentState);
            break;
            case STATE_TELA_GAMEOVER:
            
            gameOverGUI(textGameOver,gameoverTex ,&framesCounter, &currentState);
            break;

        case STATE_TELA_VITORY:
            
            gameOverGUI(textVictory,background_victory ,&framesCounter, &currentState);
            break;

        case STATE_TELA_RANKING:

            rankingGUI(&currentState, &rankingTex, rankingFont);
            break;
        }
       EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------

    UnloadTexture(background);
    UnloadTexture(midground);
    UnloadTexture(foreground);
    UnloadTexture(inter_room);
    UnloadTexture(background_history);
    UnloadTexture(gameoverTex);
    UnloadTexture(background_victory);
    UnloadTexture(cats.texture1);
    UnloadTexture(cats.texture2);
    UnloadTexture(cats.texture3);
    UnloadMusicStream(music); // Descarregar a música
    CloseAudioDevice();       // Fechar a biblioteca de áudio
    CloseWindow();            // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
    //----------------------------------------------------------------------------------
}

bool IsAnyKeyPressed()
{
    bool keyPressed = false;
    int key = GetKeyPressed();

    if ((key >= 32) && (key <= 126))
        keyPressed = true;

    return keyPressed;
}

void inputGUI(const char *name, int letterCount, Rectangle textBox, bool mouseOnText, int framesCounter, Texture2D background, Texture2D midground, Texture2D foreground, ScrollingPositions positions, GameState *currentState)
{
    // ClearBackground(RAYWHITE);
    // X - para os lados
    // - Y para cima e baixo

    // Calcular posição para centralizar a textura na tela
    float backgroundOffsetX = (screenWidth - background.width * 2) / 2;
    float backgroundOffsetY = (screenHeight - background.height * 2) / 2;

    // Desenhar imagem de fundo duas vezes para rolagem contínua
    DrawTextureEx(background, (Vector2){backgroundOffsetX + positions.scrollingBack, backgroundOffsetY}, 0.0f, 2.0f, WHITE);
    DrawTextureEx(background, (Vector2){backgroundOffsetX + background.width * 2 + positions.scrollingBack, backgroundOffsetY}, 0.0f, 2.0f, WHITE);

    // Desenhar imagem de meio fundo duas vezes para rolagem contínua
    DrawTextureEx(midground, (Vector2){backgroundOffsetX + positions.scrollingMid, backgroundOffsetY}, 0.0f, 2.0f, WHITE);
    DrawTextureEx(midground, (Vector2){backgroundOffsetX + midground.width * 2 + positions.scrollingMid, backgroundOffsetY}, 0.0f, 2.0f, WHITE);

    // Desenhar imagem de primeiro plano duas vezes para rolagem contínua
    DrawTextureEx(foreground, (Vector2){backgroundOffsetX + positions.scrollingFore, backgroundOffsetY + 50}, 0.0f, 2.0f, WHITE);
    DrawTextureEx(foreground, (Vector2){backgroundOffsetX + foreground.width * 2 + positions.scrollingFore, backgroundOffsetY + 50}, 0.0f, 2.0f, WHITE);


    DrawText("Detetive", 10, 10, 20, RED);
    DrawText("(c) Detetive Environment by PassaDisciplina ", screenWidth - 330, screenHeight - 20, 10, RAYWHITE);

    DrawText("SEU NICKNAME!", 420, 140, 20, RED);

    DrawRectangleRec(textBox, DARKBLUE);
    if (mouseOnText)
        DrawRectangleLines((int)textBox.x, (int)textBox.y, (int)textBox.width, (int)textBox.height, RED);
    else
        DrawRectangleLines((int)textBox.x, (int)textBox.y, (int)textBox.width, (int)textBox.height, DARKGRAY);

    DrawText(name, (int)textBox.x + 5, (int)textBox.y + 8, 40, RED);

    DrawText(TextFormat("MAX Letras: %i/%i", letterCount, MAX_INPUT_CHARS), (screenWidth / 2) - 50, 250, 20, RED);

    if (mouseOnText)
    {
        if (letterCount < MAX_INPUT_CHARS)
        {
            // Draw blinking underscore char
            if (((framesCounter / 20) % 2) == 0)
                DrawText("_", (int)textBox.x + 8 + MeasureText(name, 40), (int)textBox.y + 12, 40, MAROON);
        }
        else
            DrawText("Pressione BACKSPACE para deletar caracteres...", 230, 300, 20, RED);
    }
}

void menuGUI(Texture2D background, Texture2D midground, Texture2D foreground, ScrollingPositions positions, GameState *currentState)
{

    DrawRectangle(0 ,0 , screenWidth, screenHeight, BLACK);
    // Calcular posição para centralizar a textura na tela
    float backgroundOffsetX = (screenWidth - background.width * 2) / 2;
    float backgroundOffsetY = (screenHeight - background.height * 2) / 2;

    // Desenhar imagem de fundo duas vezes para rolagem contínua
    DrawTextureEx(background, (Vector2){backgroundOffsetX + positions.scrollingBack, backgroundOffsetY}, 0.0f, 2.0f, WHITE);
    DrawTextureEx(background, (Vector2){backgroundOffsetX + background.width * 2 + positions.scrollingBack, backgroundOffsetY}, 0.0f, 2.0f, WHITE);

    // Desenhar imagem de meio fundo duas vezes para rolagem contínua
    DrawTextureEx(midground, (Vector2){backgroundOffsetX + positions.scrollingMid, backgroundOffsetY}, 0.0f, 2.0f, WHITE);
    DrawTextureEx(midground, (Vector2){backgroundOffsetX + midground.width * 2 + positions.scrollingMid, backgroundOffsetY}, 0.0f, 2.0f, WHITE);

    // Desenhar imagem de primeiro plano duas vezes para rolagem contínua
    DrawTextureEx(foreground, (Vector2){backgroundOffsetX + positions.scrollingFore, backgroundOffsetY + 50}, 0.0f, 2.0f, WHITE);
    DrawTextureEx(foreground, (Vector2){backgroundOffsetX + foreground.width * 2 + positions.scrollingFore, backgroundOffsetY + 50}, 0.0f, 2.0f, WHITE);

    DrawText("Detetive", 10, 10, 20, RED);
    DrawText("(c) Detetive Environment by PassaDisciplina ", screenWidth - 330, screenHeight - 20, 10, RAYWHITE);

    // Desenhar título
    // Declaração das variáveis globais
    Button btnPlay = {(Rectangle){screenWidth / 2 - 100, 200, 200, 50}, "Jogar", false, false, DARKBLUE};
    Button btnOptions = {(Rectangle){screenWidth / 2 - 100, 300, 200, 50}, "Ranking", false, false, DARKBLUE};
    Button btnExit = {(Rectangle){screenWidth / 2 - 100, 400, 200, 50}, "Sair", false, false, DARKBLUE};

    DrawButton(btnPlay);
    DrawButton(btnOptions);
    DrawButton(btnExit);

    /// Verificar cliques
    if (IsButtonClicked(btnPlay))
    {
        // Lógica para quando o botão "Jogar" é clicado
        *currentState = STATE_TELA_INPUT;
    }

    if (IsButtonClicked(btnOptions))
    {
        // Lógica para quando o botão "Ranking" é clicado
        *currentState = STATE_TELA_RANKING;
    }

    if (IsButtonClicked(btnExit))
    {
        // Lógica para quando o botão "Sair" é clicado

        CloseWindow(); // Fechar a janela do Raylib
    }
}

// Interface gráfica d tela de historia (aonde se dar a dica do suspeito)
void historyGUI(TextForGUI *textHistory, Texture2D background_history, int *framesCounter, GameState *currentState)
{
    ClearBackground(RAYWHITE);
    // para centralizar imagem
    float scaleX = (float)screenWidth / background_history.width;
    float scaleY = (float)screenHeight / background_history.height;
    float scale = (scaleX > scaleY) ? scaleX : scaleY;
    float backgroundOffsetX = (screenWidth - background_history.width * scale) / 3;
    float backgroundOffsetY = (screenHeight - background_history.height * scale) / 3;
    

    DrawTextureEx(background_history, (Vector2){backgroundOffsetX, backgroundOffsetY}, 0.0f, scale, WHITE);
  
    // para criar quadrado para o texto
    Rectangle container = {25.0f, 25.0f, screenWidth - 50.0f, screenHeight - 250.0f};
    Rectangle resizer = {container.x + container.width - 17, container.y + container.height - 17, 14, 14};
    Font font = GetFontDefault();

    //Button btn_dica = {(Rectangle){screenWidth / 4 -150 , 300, 800, 50},  textHistory->suspect_msg, 1, 1, DARKBLUE};
    //DrawButton(btn_dica);
    Rectangle containerDica = {screenWidth / 4 -150, 300, 900, 300};
    //DrawText(textHistory->suspect_msg,screenWidth / 4 -150 , 300, 30, WHITE);
    DrawTextBoxed(font, textHistory->suspect_msg,
                      (Rectangle){containerDica.x + 4, containerDica.y + 4, containerDica.width - 4, containerDica.height - 4},
                      30.0f, 2.0f, true, WHITE);
    // Desenha o retângulo e a caixa de texto

    // Lógica para exibir o texto progressivamente
    if (*framesCounter < strlen(textHistory->text) * 3)
    {
        int lengthToShow = (*framesCounter) / 3;
        if (lengthToShow > strlen(textHistory->text))
        {
            lengthToShow = strlen(textHistory->text);
        }
        DrawTextBoxed(font, TextSubtext(textHistory->text, 0, lengthToShow),
                      (Rectangle){container.x + 4, container.y + 4, container.width - 4, container.height - 4},
                      40.0f, 2.0f, true, WHITE);

        (*framesCounter)++;
    }
    else
    {
        // Se já exibiu todo o texto, e completamente sem o efeito de digitação
        DrawTextBoxed(font, textHistory->text,
                      (Rectangle){container.x + 4, container.y + 4, container.width - 4, container.height - 4},
                      40.0f, 2.0f, true, WHITE);
    }

    // Se já exibiu todo o texto, e completamente sem o efeito de digitação
    //DrawText("dica:", 200, 195, 35, BLUE);
   // DrawTextWrapped(font, textHistory->suspect_msg,
                    //(Rectangle){300, 200, screenWidth - 350, 200}, 30.0f, 2.0f, RED);

    Button btnBack = {(Rectangle){screenWidth / 2 - 100, 560, 200, 50}, "Voltar", false, false, MAROON};
    Button btnAvanca = {(Rectangle){screenWidth / 2 - 100, 500, 200, 50}, "Avançar", false, false,DARKBLUE};
    DrawButton(btnAvanca);
    DrawButton(btnBack);

    if (IsButtonClicked(btnAvanca))
    {
        *currentState = STATE_TELA_GAMEPLAY;
    }
    if (IsButtonClicked(btnBack))
    {
        *currentState = STATE_TELA_MENU;
    }
    // DrawText("Dica: o suspeito barabaaadsafsadfsadfsafbabasvbsavsv", 300, 200, 30, RED);
    // DrawText(, 500, 200, 30, BLUE);
}

void victoryGUI(TextForGUI textVictory, Texture2D background_victory,int framesCounter, GameState *currentState){
    ClearBackground(RAYWHITE);
DrawText("Vitoria", 240, 140, 60, GREEN);
}
void gameOverGUI(TextForGUI textGameOver, Texture2D gameoverTex,int framesCounter, GameState *currentState){
    ClearBackground(RAYWHITE);
    float scaleX = (float)screenWidth / gameoverTex.width;
    float scaleY = (float)screenHeight / gameoverTex.height;
    float scale = (scaleX > scaleY) ? scaleX : scaleY;
    float backgroundOffsetX = (screenWidth - gameoverTex.width * scale) / 3;
    float backgroundOffsetY = (screenHeight - gameoverTex.height * scale) / 3;

    Button btnBack = {(Rectangle){screenWidth / 2-300, 550, 200, 50}, "Jogar novamente", false, false, MAROON};
    Button fecharBtn = {(Rectangle){screenWidth / 2 + 100, 550, 200, 50}, "Sair do game", false, false, MAROON};
    

    DrawTextureEx(gameoverTex, (Vector2){backgroundOffsetX, backgroundOffsetY}, 0.0f, scale, WHITE);
    DrawButton(btnBack);
    DrawButton(fecharBtn);
    if(IsButtonClicked(btnBack)){
        
        *currentState = STATE_TELA_GAMEPLAY;
    }
    if(IsButtonClicked(fecharBtn)){
        CloseWindow();
    }
}


void rankingGUI(GameState *currentState, Texture2D *rankingTex, Font rankingFont)
{
    ClearBackground(RAYWHITE);
    DrawTexture(*rankingTex, 0, 0, WHITE);

    Button btnBack = {(Rectangle){screenWidth / 2 - 100, 590, 200, 50}, "Voltar", false, false, MAROON};
    FILE *rankingFILE = fopen("ranking.txt", "rb");

    if (rankingFILE != NULL)
    {
        Ranking ranking[10];
        size_t readCount = fread(ranking, sizeof(Ranking), 10, rankingFILE);

        for(int Z = 0; Z < readCount; Z++)
        {
            char displayText[100];
            sprintf(displayText, "#%d.%s - %02d:%02d:%02d", Z+1, ranking[Z].name, ranking[Z].hours, ranking[Z].minutes, ranking[Z].seconds);
            DrawTextEx(rankingFont, displayText, (Vector2){screenWidth/3.0f, 82 + Z * 35 + 4}, 40, 2, BLACK);
            DrawTextEx(rankingFont, displayText, (Vector2){screenWidth/3.0f, 82 + Z * 35}, 40, 2, ORANGE);
        }

        fclose(rankingFILE);
    }

    DrawButton(btnBack);
    if(IsButtonClicked(btnBack))
    {
        *currentState = STATE_TELA_MENU;
    }
}

// Interface gráfica do gameplay

void gameGUI(GameState *currentState, Button buttons[], int *buttonCount, Texture2D inter_room, int *framesCounter, TextForGUI *textGameGUI, Imagem *cats, Personagem *persons)
{
    Color textColor = YELLOW;
    Color textColor2 = ORANGE;
    ClearBackground(RAYWHITE);
    // centralizar a imagem no centro e a escala prencher a tela
    float scaleX = (float)screenWidth / inter_room.width;
    float scaleY = (float)screenHeight / inter_room.height;
    float scale = (scaleX > scaleY) ? scaleX : scaleY;
    float backgroundOffsetX = (screenWidth - inter_room.width * scale) / 2;
    float backgroundOffsetY = (screenHeight - inter_room.height * scale) / 2;

    DrawTextureEx(inter_room, (Vector2){backgroundOffsetX, backgroundOffsetY}, 0.0f, scale, WHITE);

    if (Update.showTime)
    {
        DrawText(TextFormat("Tempo: %02d:%02d:%02d", Update.hours, Update.minutes, Update.seconds), screenWidth/3, 10, 30, YELLOW);
    } 

    DrawText("Lista de Suspeitos", 10, 10, 30, textColor2);
    DrawText("Dica:", 10, 50, 30, textColor2);

    char tent_text[2];
    sprintf(tent_text, "%d", textGameGUI->tentativas);
    DrawText("Tentativas:", 800, 10, 30, textColor2);
    DrawText(tent_text, 990, 10, 30, textColor2);

    char acertos[2];
    sprintf(acertos, "%d", textGameGUI->sequencia_de_acertos);
    DrawText("Sequencia de acertos:", 700, screenHeight -50, 30, ORANGE);
    DrawText(acertos, 1050, screenHeight -50, 30, ORANGE);
    DrawText("/10", 1070, screenHeight -50, 30, ORANGE);

    // char index[2];
    // sprintf(index, "%d", textGameGUI->culpado_index);
    // DrawText("Culpado:", 800, 170, 30, RED);
    // DrawText(index, 990, 170, 30, RED);
    //DrawText(index, 990, 170, 30, RED);
    

    Rectangle container = {600, 40,600,600};
    //DrawText(textGameGUI->suspect_msg, 600, 40, 30, RED);
    

    DrawTextBoxed(GetFontDefault(), textGameGUI->suspect_msg,
                      (Rectangle){container.x + 4, container.y + 4, container.width - 4, container.height - 4},
                      30.0f, 2.0f, true, textColor2);
        
    for (int i = 0; i < *buttonCount; i++)
    {
        if (IsButtonClicked(textGameGUI->botoes[i]))
        {
            textGameGUI->index_botao_clicado = i;
            if (i == textGameGUI->culpado_index)
            {
                textGameGUI->victory = 1;
            } else {
                textGameGUI->tentativas--;
                
            }
            // if (i < textGameGUI->culpado_index)
            //     textGameGUI->culpado_index--;
            textGameGUI->showDeathText = 1;

            strcpy(textGameGUI->theDeadText, textGameGUI->botoes[i].msg_referente);
            //removeItem(textGameGUI, buttonCount, i);
            // reinicia o contador de frames (para o texto lento)
            *framesCounter = 0;
            // diminue as tentativas
           
        }
        else
        {
            DrawButton(textGameGUI->botoes[i]);
        }
    }
     
    if (textGameGUI->showDeathText == 1)
    {
        Button btnContinua = {(Rectangle){screenWidth / 2 - 100, 400, 200, 50}, "Continua", false, false, ORANGE};
            //DrawText("GATO", 440, 240, 60, GREEN);
            if(textGameGUI->tentativas==0){
                // DERROTA
                 cats->currentTexture = 3;
                  *currentState = STATE_TELA_GAMEOVER;
                   
               DesenhaImagemPRO(cats);
            } else if(textGameGUI->victory ==1) {
                //EM CASO DE ACERTO
                cats->currentTexture =2;
               DesenhaImagemPRO(cats);

               DrawButton(btnContinua);
               if(IsButtonClicked(btnContinua)){
       
                GerarNovoSuspeito(textGameGUI, persons, *buttonCount);
                    textGameGUI->victory = 0; 
                    textGameGUI->showDeathText = 0;
                    textGameGUI->sequencia_de_acertos++;
                

                //*currentState = STATE_TELA_GAMEPLAY;
                //DrawText("CLICADO:", 400, 200, 30, ORANGE);
               }

            } else if(textGameGUI->victory != 1){
                //PADRAO
                cats->currentTexture =1;
               DesenhaImagemPRO(cats);
                
            }
             
        int lengthToShow = (*framesCounter) / 2;
        if (lengthToShow > strlen(textGameGUI->theDeadText))
        {
            lengthToShow = strlen(textGameGUI->theDeadText);
        }
        // DrawText(TextSubtext(textGameGUI->theDeadText, 0, lengthToShow), screenWidth/2, 100, 30, RED);
        Rectangle container2 = {screenWidth /2, 200,600,400};
        DrawTextBoxed(GetFontDefault(), TextSubtext(textGameGUI->theDeadText, 0, lengthToShow),
                      (Rectangle){container2.x + 4, container2.y + 4, container2.width - 4, container2.height - 4},
                      30.0f, 2.0f, true,  GOLD);
        (*framesCounter)++;
    }

    if(textGameGUI->tentativas==0){
                // DERROTA
                 cats->currentTexture = 3;     
                  GerarNovoSuspeito(textGameGUI, persons, *buttonCount);
                    textGameGUI->victory = 0; 
                    textGameGUI->showDeathText = 0;
                    textGameGUI->sequencia_de_acertos = 0;
                    textGameGUI->tentativas = 3;
                     //Reneiciar todos dados essenciais para o sistema de tempo e ranking
                    Update = (struct Update){0 ,0 , 0, 0, 0, 0, true, true, true};
                    //Reiniciar os paramêtros do jogo            
                    //muda o estado para gameover

                    *currentState = STATE_TELA_GAMEOVER;

               DesenhaImagemPRO(cats);
            }
    if (textGameGUI->sequencia_de_acertos == 3)
    { 
        
        Update.showTime = false;

        Player_ranking.hours = Update.hours;
        Player_ranking.minutes = Update.minutes;
        Player_ranking.seconds = Update.seconds;

        
         if (Update.ranking)
        {
            updateRanking();
        }
    }  
    
}
int updateRanking()
{
    Ranking ranking[10];
    FILE *rankingFILE;

    if((rankingFILE = fopen("ranking.txt", "r+b")) == NULL)
    {
        return -1;
    }

    size_t readCount = fread(ranking, sizeof(Ranking), 10, rankingFILE);
    if (readCount < 10)
    {
        for (size_t i = readCount; i < 10; i++) {
        ranking[i] = (Ranking){"NULL", 99, 99, 99};
        }
    }

    if(Player_ranking.hours < ranking[9].hours || Player_ranking.hours == ranking[9].hours)
    {
         if(Player_ranking.minutes < ranking[9].minutes ||Player_ranking.minutes == ranking[9].minutes)
         {
                if(Player_ranking.seconds < ranking[9].seconds)
                {
                    strcpy(ranking[9].name, Player_ranking.name);
                    ranking[9].hours = Player_ranking.hours;
                    ranking[9].minutes = Player_ranking.minutes;
                    ranking[9].seconds = Player_ranking.seconds;
                }
         }
           
    }else{
        
        fclose(rankingFILE);
        return 1;
    }

    int inserted = 0;
    for(int i = 0; i < 9; i++)
    {
        if(Player_ranking.hours < ranking[i].hours || Player_ranking.hours == ranking[i].hours)
        {
            if(Player_ranking.minutes < ranking[i].minutes ||Player_ranking.minutes == ranking[i].minutes)
            {  
                if(Player_ranking.seconds < ranking[i].seconds)
                {
                    for (int j = 9; j > i; j--) {
                        ranking[j] = ranking[j-1];
                    }
                    strcpy(ranking[i].name, Player_ranking.name);
                    ranking[i].hours = Player_ranking.hours;
                    ranking[i].minutes = Player_ranking.minutes;
                    ranking[i].seconds = Player_ranking.seconds;
                    inserted = 1;
                    break;
                }
            }
        }
    }

    if (inserted)
    {
        fseek(rankingFILE, 0, SEEK_SET);
        fwrite(ranking, sizeof(Ranking), 10, rankingFILE);

    }

    Update.ranking = false;

    fclose(rankingFILE);

    return 0;
}
// Função de atualização da tela do menu
ScrollingPositions UpdateScrolling(ScrollingPositions positions, float backgroundWidth, float midgroundWidth, float foregroundWidth)
{
    positions.scrollingBack -= 0.1f;
    positions.scrollingMid -= 0.5f;
    positions.scrollingFore -= 1.0f;

    // Ajustar a posição de rolagem para o efeito contínuo
    if (positions.scrollingBack <= -backgroundWidth * 2)
        positions.scrollingBack = 0;
    if (positions.scrollingMid <= -midgroundWidth * 2)
        positions.scrollingMid = 0;
    if (positions.scrollingFore <= -foregroundWidth * 2)
        positions.scrollingFore = 0;

    return positions;
}

// Função para desenhar um botão
void DrawButton(Button button)
{
    // Desenhar retângulo do botão
    if (button.hovered)
    {
        Vector2 mousePoint = GetMousePosition();
        if (CheckCollisionPointRec(mousePoint, button.bounds))
        {
            DrawRectangleRec(button.bounds, DARKGRAY);
        }
        else
        {
            DrawRectangleRec(button.bounds, button.bounds_color);
        }
    }
    else
    {
        DrawRectangleRec(button.bounds, button.bounds_color);
    }

    // Desenhar texto centralizado no botão
    DrawText(button.text, button.bounds.x + button.bounds.width / 2 - MeasureText(button.text, 20) / 2,
             button.bounds.y + button.bounds.height / 2 - 10, 20, WHITE);
}

_Bool IsButtonClicked(Button button)
{
    return (CheckCollisionPointRec(GetMousePosition(), button.bounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON));
}

void DesenhatextoDinamico(const char *text, int posX, int posY, int fontSize, Color color, bool show)
{
    if (show)
    {
        DrawText(text, posX, posY, fontSize, color);
    }
}

void removeItem(TextForGUI *text, int *size, int index)
{
    if (index < 0 || index >= *size)
    {
        return;
    }

    for (int i = index; i < *size - 1; i++)
    {
        text->botoes[i] = text->botoes[i + 1];
    }

    (*size)--;
}

// Draw text using font inside rectangle limits
static void DrawTextBoxed(Font font, const char *text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint)
{
    DrawTextBoxedSelectable(font, text, rec, fontSize, spacing, wordWrap, tint, 0, 0, WHITE, WHITE);
}

// Draw text using font inside rectangle limits with support for text selection
static void DrawTextBoxedSelectable(Font font, const char *text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint, int selectStart, int selectLength, Color selectTint, Color selectBackTint)
{
    int length = TextLength(text); // Total length in bytes of the text, scanned by codepoints in loop

    float textOffsetY = 0;    // Offset between lines (on line break '\n')
    float textOffsetX = 0.0f; // Offset X to next character to draw

    float scaleFactor = fontSize / (float)font.baseSize; // Character rectangle scaling factor

    // Word/character wrapping mechanism variables
    enum
    {
        MEASURE_STATE = 0,
        DRAW_STATE = 1
    };
    int state = wordWrap ? MEASURE_STATE : DRAW_STATE;

    int startLine = -1; // Index where to begin drawing (where a line begins)
    int endLine = -1;   // Index where to stop drawing (where a line ends)
    int lastk = -1;     // Holds last value of the character position

    for (int i = 0, k = 0; i < length; i++, k++)
    {
        // Get next codepoint from byte string and glyph index in font
        int codepointByteCount = 0;
        int codepoint = GetCodepoint(&text[i], &codepointByteCount);
        int index = GetGlyphIndex(font, codepoint);

        // NOTE: Normally we exit the decoding sequence as soon as a bad byte is found (and return 0x3f)
        // but we need to draw all of the bad bytes using the '?' symbol moving one byte
        if (codepoint == 0x3f)
            codepointByteCount = 1;
        i += (codepointByteCount - 1);

        float glyphWidth = 0;
        if (codepoint != '\n')
        {
            glyphWidth = (font.glyphs[index].advanceX == 0) ? font.recs[index].width * scaleFactor : font.glyphs[index].advanceX * scaleFactor;

            if (i + 1 < length)
                glyphWidth = glyphWidth + spacing;
        }

        // NOTE: When wordWrap is ON we first measure how much of the text we can draw before going outside of the rec container
        // We store this info in startLine and endLine, then we change states, draw the text between those two variables
        // and change states again and again recursively until the end of the text (or until we get outside of the container).
        // When wordWrap is OFF we don't need the measure state so we go to the drawing state immediately
        // and begin drawing on the next line before we can get outside the container.
        if (state == MEASURE_STATE)
        {
            // TODO: There are multiple types of spaces in UNICODE, maybe it's a good idea to add support for more
            // Ref: http://jkorpela.fi/chars/spaces.html
            if ((codepoint == ' ') || (codepoint == '\t') || (codepoint == '\n'))
                endLine = i;

            if ((textOffsetX + glyphWidth) > rec.width)
            {
                endLine = (endLine < 1) ? i : endLine;
                if (i == endLine)
                    endLine -= codepointByteCount;
                if ((startLine + codepointByteCount) == endLine)
                    endLine = (i - codepointByteCount);

                state = !state;
            }
            else if ((i + 1) == length)
            {
                endLine = i;
                state = !state;
            }
            else if (codepoint == '\n')
                state = !state;

            if (state == DRAW_STATE)
            {
                textOffsetX = 0;
                i = startLine;
                glyphWidth = 0;

                // Save character position when we switch states
                int tmp = lastk;
                lastk = k - 1;
                k = tmp;
            }
        }
        else
        {
            if (codepoint == '\n')
            {
                if (!wordWrap)
                {
                    textOffsetY += (font.baseSize + font.baseSize / 2) * scaleFactor;
                    textOffsetX = 0;
                }
            }
            else
            {
                if (!wordWrap && ((textOffsetX + glyphWidth) > rec.width))
                {
                    textOffsetY += (font.baseSize + font.baseSize / 2) * scaleFactor;
                    textOffsetX = 0;
                }

                // When text overflows rectangle height limit, just stop drawing
                if ((textOffsetY + font.baseSize * scaleFactor) > rec.height)
                    break;

                // Draw selection background
                bool isGlyphSelected = false;
                if ((selectStart >= 0) && (k >= selectStart) && (k < (selectStart + selectLength)))
                {
                    DrawRectangleRec((Rectangle){rec.x + textOffsetX - 1, rec.y + textOffsetY, glyphWidth, (float)font.baseSize * scaleFactor}, selectBackTint);
                    isGlyphSelected = true;
                }

                // Draw current character glyph
                if ((codepoint != ' ') && (codepoint != '\t'))
                {
                    DrawTextCodepoint(font, codepoint, (Vector2){rec.x + textOffsetX, rec.y + textOffsetY}, fontSize, isGlyphSelected ? selectTint : tint);
                }
            }

            if (wordWrap && (i == endLine))
            {
                textOffsetY += (font.baseSize + font.baseSize / 2) * scaleFactor;
                textOffsetX = 0;
                startLine = endLine;
                endLine = -1;
                glyphWidth = 0;
                selectStart += lastk - k;
                k = lastk;

                state = !state;
            }
        }

        if ((textOffsetX != 0) || (codepoint != ' '))
            textOffsetX += glyphWidth; // avoid leading spaces
    }
}
//#include "raylib.h"

// Função para desenhar texto com word wrap dentro dos limites do retângulo
void DrawTextWrapped(Font font, const char *text, Rectangle rec, float fontSize, float spacing, Color tint)
{
    int length = TextLength(text);
    float textOffsetY = 0;
    float textOffsetX = 0.0f;
    float scaleFactor = fontSize / (float)font.baseSize;

    for (int i = 0; i < length; i++)
    {
        int codepointByteCount = 0;
        int codepoint = GetCodepoint(&text[i], &codepointByteCount);
        int index = GetGlyphIndex(font, codepoint);

        if (codepoint == '\n')
        {
            textOffsetY += (font.baseSize + font.baseSize / 2) * scaleFactor;
            textOffsetX = 0;
        }
        else
        {
            float glyphWidth = (font.glyphs[index].advanceX == 0) ? font.recs[index].width * scaleFactor : font.glyphs[index].advanceX * scaleFactor;

            if ((textOffsetX + glyphWidth) > rec.width)
            {
                textOffsetY += (font.baseSize + font.baseSize / 2) * scaleFactor;
                textOffsetX = 0;
            }

            if ((textOffsetY + font.baseSize * scaleFactor) > rec.height)
                break;

            DrawTextCodepoint(font, codepoint, (Vector2){rec.x + textOffsetX, rec.y + textOffsetY}, fontSize, tint);

            textOffsetX += glyphWidth + spacing;
        }

        i += (codepointByteCount - 1);
    }
}

void embaralhar(Personagem *array, int n)
{
    for (int i = n - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);
        Personagem temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}


void DesenhaImagemPRO(Imagem *img) {
    Texture2D texture;
    switch (img->currentTexture) {
        case 1:
            texture = img->texture1;
            break;
        case 2:
            texture = img->texture2;
            break;
        case 3:
            texture = img->texture3;
            break;
        default:
            texture = img->texture1;
            break;
    }
    
    Rectangle sourceRect = { 0.0f, 0.0f, (float)texture.width, (float)texture.height };
    Rectangle destRect = { img->position.x, img->position.y, (float)img->width, (float)img->height };
    Vector2 origin = { 0.0f, 0.0f };
    DrawTexturePro(texture, sourceRect, destRect, origin, 0.0f, img->tint);
}

void GerarNovoSuspeito(TextForGUI *textGameGUI, Personagem *persons, int numPersons) {
    int indice_do_culpado = rand() % numPersons;
    int indice_da_dica_do_culpado = rand() % persons[indice_do_culpado].num_hints;

    textGameGUI->suspect_msg = persons[indice_do_culpado].suspect_text[indice_da_dica_do_culpado];
    textGameGUI->culpado_index = indice_do_culpado;
    
    
}
void renderizarBotoesPersonagens(Button buttons[], int buttonCount, Personagem persons[]) {
    int totalCharacters = sizeof(persons) / sizeof(persons[0]);
    embaralhar(persons, totalCharacters);
    for (int i = 0; i < buttonCount; i++) {
        buttons[i].bounds = (Rectangle){10, 80 + i * 50, 280, 40};
        buttons[i].text = persons[i].name_person;
        strcpy(buttons[i].msg_referente, persons[i].DeadText);
        buttons[i].bounds_color = BLUE;
        buttons[i].hovered = 1;
    }
}

void RenderizarBotoes(TextForGUI *textGameGUI, int *buttonCount) {
    for (int i = 0; i < *buttonCount; i++) {
        DrawButton(textGameGUI->botoes[i]);
    }
}