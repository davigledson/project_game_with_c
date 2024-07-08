
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h> // Incluir a biblioteca para o tipo bool
#define MAX_INPUT_CHARS 15
#define screenWidth 1200
#define screenHeight 650


typedef enum
{
    STATE_TELA_MENU,
    STATE_TELA_GAMEPLAY,
    STATE_TELA_HISTORY,
    STATE_TELA_INPUT,
    STATE_GAMEOVER
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

typedef struct {
    char *name_person;
    char *DeadText;
    char *suspect_text
} Personagem;

//para todos os textos únicos para a tela de gameplay, (inclusive texto do gameplay)
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
    char text[500];
} TextForGUI;



// Função de atualização que retorna uma estrutura com as novas posições de rolagem
_Bool IsButtonClicked(Button button);
void DesenhatextoDinamico(const char *text, int posX, int posY, int fontSize, Color color, bool show);
ScrollingPositions UpdateScrolling(ScrollingPositions positions, float backgroundWidth, float midgroundWidth, float foregroundWidth);
static void DrawTextBoxed(Font font, const char *text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint);   // Draw text using font inside rectangle limits
static void DrawTextBoxedSelectable(Font font, const char *text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint, int selectStart, int selectLength, Color selectTint, Color selectBackTint);   
// Protótipo da função para desenhar texto com word wrap
void DrawTextWrapped(Font font, const char *text, Rectangle rec, float fontSize, float spacing, Color tint);

int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
     srand(time(NULL));
    GameState currentState = STATE_TELA_HISTORY;
    ScrollingPositions positions = {0.0f, 0.0f, 0.0f};
    
    // STATE = 0; TELA DO MENU
    // STATE = 1; TELA DO INPUT
    // STATE = 2; TELA DA GAMEPLAY
    // STATE = 4; TELA DA RANKING

    InitWindow(screenWidth, screenHeight, "Tela inicial do jogo");

    char name[MAX_INPUT_CHARS + 1] = "\0"; // NOTE: One extra space required for null terminator char '\0'
    int letterCount = 0;

    Rectangle textBox = {screenWidth / 2.0f - 200, 180, 380, 50};
    bool mouseOnText = false;

    int framesCounter = 0;

    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------
    Texture2D background = LoadTexture("resources/cyberpunk_street_background.png");
    Texture2D midground = LoadTexture("resources/cyberpunk_street_midground.png");
    Texture2D foreground = LoadTexture("resources/cyberpunk_street_foreground.png");
    Texture2D inter_room = LoadTexture("imgs/inter_room.png");
    Texture2D background_history = LoadTexture("imgs/cena_crime.png");
    // IMPLEMENTA A LOGICA DAS INTERFACES
    InitAudioDevice();
    Music music = LoadMusicStream("sounds/amoung_them.mp3");
    PlayMusicStream(music); // Iniciar a reprodução da música

    // **Alteração:**  para não iniciarlizar a tela de menu com uma parte da tela preta
    positions.scrollingBack = -background.width; // Centralizar a textura de fundo
    positions.scrollingMid = -midground.width;
    positions.scrollingFore = -foreground.width;
    
    //Variaveis == textos para a tela de gameplay
    TextForGUI textGameGUI;
    textGameGUI.tentativas =3;
    textGameGUI.qtdPersonagens =9;
    textGameGUI.showDeathText =false;
    
    textGameGUI.victory = 0;
    int buttonCount = textGameGUI.qtdPersonagens;
    int indice_do_culpado = rand() % buttonCount;
    textGameGUI.culpado_index = indice_do_culpado;
    Button buttons[buttonCount];
  
    

       Personagem persons[]  = {
        {"MAX", "Meu Vascooooosss","fez a faculdade no UFRN"},
        {"HENRIQUE", "esqueci o que escrever","Seu cargo anterior era de analista de sistemas"},
        {"CERES CREMENTINA", "como é o  de Talless?"," vai me dar um  a 10 nesse jogo"},
        {"REGINALDO BATISTA", "Quimica","Sua frase no status do whats \"é só sei que nada sei\""},
        {"ELANIO", "AEC","50 porcento economista"},
        {"DANIEL LIRA", "Matemática","A menor nota em matemática foi 10 até agora"},
        {"FRANKING", "Estudeeee","Durante sua época de faculdade, trancou alguns períodos para trabalha"},
        {"Dario", "meus Amigooosss","E o inimigo numero 1 do INSS"},
        {"Heitor", "DarkSoul","E fam de DarkSouls"}, 
    };

    //Variaveis == textos para a tela de historia
     TextForGUI textHistory;
     strcpy(textHistory.text, "Na UERN, alguém está tentando derrubar os pilares do setor da FANAT, o que se sabe sobre o suspeito é que:");
    textHistory.culpado_index = indice_do_culpado;
    textHistory.suspect_msg = persons[indice_do_culpado].suspect_text;
    for (int i = 0; i < buttonCount; i++)
    {
        
        buttons[i].bounds = (Rectangle){10, 80 + i * 50, 280, 40};
        buttons[i].text = persons[i].name_person;
        strcpy(buttons[i].msg_referente, persons[i].DeadText);
        buttons[i].bounds_color = BLUE;
        buttons[i].hovered = 1;

        
            
        
    }

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Atualizar a música
        UpdateMusicStream(music);
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
                // EScreve alno no arquivo
                DrawText("Cdsfsfsdfsdfsfsfdsafsf!", 240, 140, 20, GRAY);
                FILE *file = fopen("ranking.txt", "a"); // Usar "w" para sobrescrever ou "a" para anexar
                fprintf(file, "%s\n", name);
                fclose(file);
                DrawText(name, (screenWidth / 2) - 50, 10, 40, RED);
                currentState = STATE_TELA_GAMEPLAY;
            }

            break;
        case STATE_TELA_GAMEPLAY:

            break;
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
           gameGUI(&currentState, buttons, &buttonCount,inter_room,&framesCounter,&textGameGUI);
            break;
            case STATE_TELA_HISTORY:
           historyGUI(&textHistory,background_history,&framesCounter);
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

    DrawText(name, (screenWidth / 2) - 50, 10, 40, RED);

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
    Button btnPlay = {(Rectangle){screenWidth / 2 - 100, 200, 200, 50}, "Jogar", false, false};
    Button btnOptions = {(Rectangle){screenWidth / 2 - 100, 300, 200, 50}, "Ranking", false, false};
    Button btnExit = {(Rectangle){screenWidth / 2 - 100, 400, 200, 50}, "Sair", false, false};

    DrawButton(btnPlay);
    DrawButton(btnOptions);
    DrawButton(btnExit);

    /// Verificar cliques
    if (IsButtonClicked(btnPlay))
    {
        // Lógica para quando o botão "Jogar" é clicado
        *currentState = STATE_TELA_HISTORY;
    }

    if (IsButtonClicked(btnExit))
    {
        // Lógica para quando o botão "Sair" é clicado

        CloseWindow(); // Fechar a janela do Raylib
    }
}

// Interface gráfica d tela de historia (aonde se dar a dica do suspeito)
void historyGUI(TextForGUI *textHistory, Texture2D background_history,int *framesCounter){
       ClearBackground(RAYWHITE);
    //para centralizar imagem
    float scaleX = (float)screenWidth / background_history.width;
    float scaleY = (float)screenHeight / background_history.height;
    float scale = (scaleX > scaleY) ? scaleX : scaleY;
float backgroundOffsetX = (screenWidth - background_history.width * scale) / 2;
    float backgroundOffsetY = (screenHeight - background_history.height * scale) / 2;
DrawTextureEx(background_history, (Vector2){backgroundOffsetX, backgroundOffsetY}, 0.0f, scale, WHITE);

    //para criar quadrado para o texto
    Rectangle container = { 25.0f, 25.0f, screenWidth - 50.0f, screenHeight - 250.0f };
    Rectangle resizer = { container.x + container.width - 17, container.y + container.height - 17, 14, 14 };
    Font font = GetFontDefault(); 
       
       
        // Desenha o retângulo e a caixa de texto
    DrawRectangleLinesEx(container, 3, RED);
    
    // Lógica para exibir o texto progressivamente
    if (*framesCounter < strlen(textHistory->text) * 5) {
        int lengthToShow = (*framesCounter) / 5;
        if (lengthToShow > strlen(textHistory->text)) {
            lengthToShow = strlen(textHistory->text);
        }
        DrawTextBoxed(font, TextSubtext(textHistory->text, 0, lengthToShow), 
                      (Rectangle){ container.x + 4, container.y + 4, container.width - 4, container.height - 4 }, 
                      40.0f, 2.0f, true, RED);
                      
        (*framesCounter)++;
    } else {
        // Se já exibiu todo o texto, e completamente sem o efeito de digitação
        DrawTextBoxed(font, textHistory->text, 
                      (Rectangle){ container.x + 4, container.y + 4, container.width - 4, container.height - 4 }, 
                      40.0f, 2.0f, true, RED);
    }
   
   // Se já exibiu todo o texto, e completamente sem o efeito de digitação
   DrawText("dica:", 200, 200, 32, BLUE);
        DrawTextWrapped(font, textHistory->suspect_msg, 
                (Rectangle){ 300, 200, screenWidth - 350, 200 }, 30.0f, 2.0f, RED);

     //DrawText("Dica: o suspeito barabaaadsafsadfsadfsafbabasvbsavsv", 300, 200, 30, RED);
     DrawText("Historia...", 10, 10, 30, BLUE);
    // DrawText(, 500, 200, 30, BLUE);
}

// Interface gráfica do gameplay

void gameGUI(GameState *currentState, Button buttons[], int *buttonCount, Texture2D inter_room, int *framesCounter, TextForGUI *textGameGUI)
{
    
   ClearBackground(RAYWHITE);
   // centralizar a imagem no centro e a escala prencher a tela
   float scaleX = (float)screenWidth / inter_room.width;
    float scaleY = (float)screenHeight / inter_room.height;
    float scale = (scaleX > scaleY) ? scaleX : scaleY;
float backgroundOffsetX = (screenWidth - inter_room.width * scale) / 2;
    float backgroundOffsetY = (screenHeight - inter_room.height * scale) / 2;
    
       DrawTextureEx(inter_room, (Vector2){backgroundOffsetX, backgroundOffsetY}, 0.0f, scale, WHITE);




    DrawText("Lista de Suspeitos", 10, 10, 30, BLUE);
    char tent_text[2] ;
    sprintf(tent_text, "%d", textGameGUI->tentativas);
    DrawText("Tentativas:", 800, 10, 30, BLUE);
    DrawText(tent_text, 990, 10, 30, BLUE);

    
    char index[2] ;
    sprintf(index, "%d", textGameGUI->culpado_index);
    DrawText("Culpado:", 800, 40, 30, RED);
    DrawText(index, 990, 40, 30, RED);
    
    for (int i = 0; i < *buttonCount; i++)
    {
        if (IsButtonClicked(buttons[i]))
        {
            
            if(i == textGameGUI->culpado_index){
                textGameGUI->victory = 1;
            }
            if(i < textGameGUI->culpado_index) textGameGUI->culpado_index--;
             textGameGUI->showDeathText = 1;

            strcpy(textGameGUI->theDeadText, buttons[i].msg_referente);
            removeItem(buttons, buttonCount, i);
           // reinicia o contador de frames (para o texto lento)
            *framesCounter = 0;
            //diminue as tentativas
            textGameGUI->tentativas--;
        }
        else
        {
            DrawButton(buttons[i]);
           
        } 
        
    }
    if(textGameGUI->showDeathText == 1){

         int lengthToShow = (*framesCounter) / 10;
        if (lengthToShow > strlen(textGameGUI->theDeadText))
        {
            lengthToShow = strlen(textGameGUI->theDeadText);
        }
        DrawText(TextSubtext(textGameGUI->theDeadText, 0, lengthToShow), 340, 100, 60, RED);
        (*framesCounter)++; 
        }

        if(textGameGUI->victory == 1){
           DrawText("VITORIAAA", 440 , 240, 60, GREEN);  
        }
        if(*buttonCount <=6){
           DrawText("OLA", 240 , 140, 60, RED);  
        }
    
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
            DrawRectangleRec(button.bounds, DARKBLUE);
        }
    }
    else
    {
        DrawRectangleRec(button.bounds, DARKBLUE);
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

void removeItem(Button buttons[], int *size, int index)
{
    if (index < 0 || index >= *size)
    {
        return;
    }

    for (int i = index; i < *size - 1; i++)
    {
        buttons[i] = buttons[i + 1];
    }

    (*size)--;
}


int escolher_vitima(int num_players, int impostor_index) {
    int victim_index;
    do {
        victim_index = rand() % (num_players);
    } while (victim_index == impostor_index);
    return victim_index;
}


void remover_jogador(char lista[][50], int *num_players, int index) {
    for (int j = index; j < *num_players - 1; j++) {
        strcpy(lista[j], lista[j + 1]);
    }
    (*num_players)--;
}

void verificar_palpite(char lista[][50], int *num_players, char *impostor) {
    int user_input = obter_palpite(*num_players);

    if (strcmp(lista[user_input], impostor) == 0) {
        imprimir_vencedor(lista[user_input]);
        *num_players = 0; // Encerra o jogo
    } else {
        printf("====================\n");
        printf("%s não era o impostor!\n", lista[user_input]);
        printf("====================\n");
        remover_jogador(lista, num_players, user_input);
    }
}

int obter_palpite(int num_players) {
    int user_input;
    printf("Quem é o impostor? [digite de 0 até %d]: ", num_players - 1);
    while (scanf("%d", &user_input) != 1 || user_input < 0 || user_input >= num_players) {
        while (getchar() != '\n');
        printf("Entrada inválida. Tente novamente. Quem é o impostor? [digite de 0 até %d]: ", num_players - 1);
    }
    return user_input;
}

void imprimir_sobreviventes(char lista[][50], int num_players) {
    printf("Os Sobreviventes são:\n");
    for (int j = 0; j < num_players; j++) {
        printf(" %s |", lista[j]);
    }
    printf("\n");
}

void imprimir_vencedor(char *impostor) {
    printf("====================\n");
    char frase_lenta[50] = " ";
    strcat(frase_lenta, impostor);
    strcat(frase_lenta, " era o impostor!");
    for (int i = 0; frase_lenta[i] != '\0'; i++) {
        printf("%c", frase_lenta[i]);
        fflush(stdout);
        usleep(100000); // Atraso de 100 milissegundos
    }
    printf("\n====================\n");
    printf("Parabéns! VOCÊ GANHOU!\n");
}

void imprimir_perdedor(char *impostor) {
    printf("====================\n");
    printf("%s era o Impostor\n", impostor);
    printf("====================\n");
    printf("VOCÊ PERDEU!\n");
}


// Draw text using font inside rectangle limits
static void DrawTextBoxed(Font font, const char *text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint)
{
    DrawTextBoxedSelectable(font, text, rec, fontSize, spacing, wordWrap, tint, 0, 0, WHITE, WHITE);
}

// Draw text using font inside rectangle limits with support for text selection
static void DrawTextBoxedSelectable(Font font, const char *text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint, int selectStart, int selectLength, Color selectTint, Color selectBackTint)
{
    int length = TextLength(text);  // Total length in bytes of the text, scanned by codepoints in loop

    float textOffsetY = 0;          // Offset between lines (on line break '\n')
    float textOffsetX = 0.0f;       // Offset X to next character to draw

    float scaleFactor = fontSize/(float)font.baseSize;     // Character rectangle scaling factor

    // Word/character wrapping mechanism variables
    enum { MEASURE_STATE = 0, DRAW_STATE = 1 };
    int state = wordWrap? MEASURE_STATE : DRAW_STATE;

    int startLine = -1;         // Index where to begin drawing (where a line begins)
    int endLine = -1;           // Index where to stop drawing (where a line ends)
    int lastk = -1;             // Holds last value of the character position

    for (int i = 0, k = 0; i < length; i++, k++)
    {
        // Get next codepoint from byte string and glyph index in font
        int codepointByteCount = 0;
        int codepoint = GetCodepoint(&text[i], &codepointByteCount);
        int index = GetGlyphIndex(font, codepoint);

        // NOTE: Normally we exit the decoding sequence as soon as a bad byte is found (and return 0x3f)
        // but we need to draw all of the bad bytes using the '?' symbol moving one byte
        if (codepoint == 0x3f) codepointByteCount = 1;
        i += (codepointByteCount - 1);

        float glyphWidth = 0;
        if (codepoint != '\n')
        {
            glyphWidth = (font.glyphs[index].advanceX == 0) ? font.recs[index].width*scaleFactor : font.glyphs[index].advanceX*scaleFactor;

            if (i + 1 < length) glyphWidth = glyphWidth + spacing;
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
            if ((codepoint == ' ') || (codepoint == '\t') || (codepoint == '\n')) endLine = i;

            if ((textOffsetX + glyphWidth) > rec.width)
            {
                endLine = (endLine < 1)? i : endLine;
                if (i == endLine) endLine -= codepointByteCount;
                if ((startLine + codepointByteCount) == endLine) endLine = (i - codepointByteCount);

                state = !state;
            }
            else if ((i + 1) == length)
            {
                endLine = i;
                state = !state;
            }
            else if (codepoint == '\n') state = !state;

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
                    textOffsetY += (font.baseSize + font.baseSize/2)*scaleFactor;
                    textOffsetX = 0;
                }
            }
            else
            {
                if (!wordWrap && ((textOffsetX + glyphWidth) > rec.width))
                {
                    textOffsetY += (font.baseSize + font.baseSize/2)*scaleFactor;
                    textOffsetX = 0;
                }

                // When text overflows rectangle height limit, just stop drawing
                if ((textOffsetY + font.baseSize*scaleFactor) > rec.height) break;

                // Draw selection background
                bool isGlyphSelected = false;
                if ((selectStart >= 0) && (k >= selectStart) && (k < (selectStart + selectLength)))
                {
                    DrawRectangleRec((Rectangle){ rec.x + textOffsetX - 1, rec.y + textOffsetY, glyphWidth, (float)font.baseSize*scaleFactor }, selectBackTint);
                    isGlyphSelected = true;
                    
                }

                // Draw current character glyph
                if ((codepoint != ' ') && (codepoint != '\t'))
                {
                    DrawTextCodepoint(font, codepoint, (Vector2){ rec.x + textOffsetX, rec.y + textOffsetY }, fontSize, isGlyphSelected? selectTint : tint);
                }
            }

            if (wordWrap && (i == endLine))
            {
                textOffsetY += (font.baseSize + font.baseSize/2)*scaleFactor;
                textOffsetX = 0;
                startLine = endLine;
                endLine = -1;
                glyphWidth = 0;
                selectStart += lastk - k;
                k = lastk;

                state = !state;
            }
        }

        if ((textOffsetX != 0) || (codepoint != ' ')) textOffsetX += glyphWidth;  // avoid leading spaces
    }
}
#include "raylib.h"

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

            DrawTextCodepoint(font, codepoint, (Vector2){ rec.x + textOffsetX, rec.y + textOffsetY }, fontSize, tint);

            textOffsetX += glyphWidth + spacing;
        }

        i += (codepointByteCount - 1);
    }
}
