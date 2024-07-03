
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> // Incluir a biblioteca para o tipo bool
#define MAX_INPUT_CHARS 15
#define screenWidth 1200
#define screenHeight 650
#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

typedef enum
{
    STATE_TELA_MENU,
    STATE_TELA_GAMEPLAY,
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
    
} Button;

typedef struct
{
    float scrollingBack;
    float scrollingMid;
    float scrollingFore;
} ScrollingPositions;


// Função de atualização que retorna uma estrutura com as novas posições de rolagem
_Bool IsButtonClicked(Button button);
void DesenhatextoDinamico(const char *text, int posX, int posY, int fontSize, Color color, bool show);
ScrollingPositions UpdateScrolling(ScrollingPositions positions, float backgroundWidth, float midgroundWidth, float foregroundWidth);
int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------

    GameState currentState = STATE_TELA_GAMEPLAY;
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

    bool showText = false;
    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------
    Texture2D background = LoadTexture("resources/cyberpunk_street_background.png");
    Texture2D midground = LoadTexture("resources/cyberpunk_street_midground.png");
    Texture2D foreground = LoadTexture("resources/cyberpunk_street_foreground.png");
    // IMPLEMENTA A LOGICA DAS INTERFACES
    InitAudioDevice();
    Music music = LoadMusicStream("sounds/amoung_theme.mp3");
    PlayMusicStream(music); // Iniciar a reprodução da música

    // **Alteração:**  para não iniciarlizar a tela de menu com uma parte da tela preta
    positions.scrollingBack = -background.width; // Centralizar a textura de fundo
    positions.scrollingMid = -midground.width;
    positions.scrollingFore = -foreground.width;

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
            gameGUI(&currentState,&showText);
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

    DrawText("COLOQUE O MOUSE SOBRE A CAIXA DE ENTRADA!", 340, 140, 20, RED);

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
        *currentState = STATE_TELA_INPUT;
    }

    if (IsButtonClicked(btnExit))
    {
        // Lógica para quando o botão "Sair" é clicado

        CloseWindow(); // Fechar a janela do Raylib
    }
}

//Interface gráfica do gameplay
void gameGUI(GameState *currentState, bool *showText)
{
   


    ClearBackground(RAYWHITE);
    Vector2 mousePoint = GetMousePosition();
    Button buttons[9];
      char *buttonText[] = {
        "DAVI GLEDSON", "ALERRANDO", "PROFESSORA CERES", "REGINALDO BATISTA",
        "ELANIO", "DANIEL LIRA", "MARIANA",
        "THAFINE", "ANA CAROLYNE"
    };
            for (int i = 0; i < 9; i++)
            {
                
                buttons[i].bounds = (Rectangle){10, 10 + i * 50, 180, 40};
                buttons[i].text = buttonText[i];
                buttons[i].bounds_color = BLUE;
                buttons[i].hovered = 1;
               
            }
    for (int i = 0; i < 9; i++) {
      
              
                
                 if (CheckCollisionPointRec(mousePoint, buttons[i].bounds)) {   
                  DrawText("Clicado", 10, 10, 20, RED);
                }  

                DrawButton(buttons[i]);
         }
       
     //DrawText("Clicado", 10, 10, 20, RED);
    // DesenhatextoDinamico("haaaaaaaaaaaaaaaaaa", 10, 10, 20, RED,1);
        //DrawText(buttonText[j], buttons[j].x + 10, buttons[j].y + 10, 10, BLACK);


            // if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&CheckCollisionPointRec(mousePoint, buttons[j])) 
            //     {
            //           *showText = true;
            //     }

            //     if(*showText){
            //         DrawText("Clicadodsfhjksd", 100, 100, 40, RED);
            //     }
        
    }
//}


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
    if(button.hovered){
        Vector2 mousePoint = GetMousePosition();
         if (CheckCollisionPointRec(mousePoint, button.bounds)){
          DrawRectangleRec(button.bounds, DARKGRAY);
         } else {
        DrawRectangleRec(button.bounds, DARKBLUE);
    }

    } else {
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

void DesenhatextoDinamico(const char *text, int posX, int posY, int fontSize, Color color, bool show) {
    if (show) {
        DrawText(text, posX, posY, fontSize, color);
    }
}