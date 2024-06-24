
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
typedef struct {
    Rectangle bounds;
    const char *text;
    bool hovered;
    bool clicked;
} Button;

typedef struct {
    float scrollingBack;
    float scrollingMid;
    float scrollingFore;
} ScrollingPositions;

_Bool IsButtonClicked(Button button);
// Função de atualização que retorna uma estrutura com as novas posições de rolagem
ScrollingPositions UpdateScrolling(ScrollingPositions positions, float backgroundWidth, float midgroundWidth, float foregroundWidth);
int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
     
    GameState currentState = STATE_TELA_MENU;
    ScrollingPositions positions = {0.0f, 0.0f, 0.0f};
    
    // STATE = 0; TELA DO MENU
    // STATE = 1; TELA DO INPUT
    // STATE = 2; TELA DA GAMEPLAY
    // STATE = 4; TELA DA RANKING
    

    InitWindow(screenWidth, screenHeight, "Tela inicial do jogo");

    char name[MAX_INPUT_CHARS + 1] = "\0"; // NOTE: One extra space required for null terminator char '\0'
    int letterCount = 0;

    Rectangle textBox = {screenWidth / 2.0f - 100, 180, 225, 50};
    bool mouseOnText = false;

    int framesCounter = 0;

    SetTargetFPS(60); // Set our game to run at 10 frames-per-second
    //--------------------------------------------------------------------------------------
    Texture2D background = LoadTexture("resources/cyberpunk_street_background.png");
    Texture2D midground = LoadTexture("resources/cyberpunk_street_midground.png");
    Texture2D foreground = LoadTexture("resources/cyberpunk_street_foreground.png");
    //IMPLEMENTA A LOGICA DAS INTERFACES
    
    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        switch (currentState)
        {
        case STATE_TELA_MENU:
       
            positions = UpdateScrolling(positions, background.width, midground.width, foreground.width);
        

                
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
                currentState = STATE_TELA_MENU;
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
             menuGUI(background, midground, foreground, positions,&currentState);

            break;
        case STATE_TELA_INPUT:
            inputGUI(name, letterCount, textBox, mouseOnText, framesCounter);
            break;
        case STATE_TELA_GAMEPLAY:

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
    CloseWindow(); // Close window and OpenGL context
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

void inputGUI(const char *name, int letterCount, Rectangle textBox, bool mouseOnText, int framesCounter)
{
    ClearBackground(RAYWHITE);
    // X - para os lados
    // - Y para cima e baixo
    DrawText(name, 380, 10, 40, RED);

    DrawText("COLOQUE O MOUSE SOBRE A CAIXA DE ENTRADA!", 340, 140, 20, GRAY);

    DrawRectangleRec(textBox, LIGHTGRAY);
    if (mouseOnText)
        DrawRectangleLines((int)textBox.x, (int)textBox.y, (int)textBox.width, (int)textBox.height, RED);
    else
        DrawRectangleLines((int)textBox.x, (int)textBox.y, (int)textBox.width, (int)textBox.height, DARKGRAY);

    DrawText(name, (int)textBox.x + 5, (int)textBox.y + 8, 40, MAROON);

    DrawText(TextFormat("INPUT CHARS: %i/%i", letterCount, MAX_INPUT_CHARS), 340, 250, 20, DARKGRAY);

    if (mouseOnText)
    {
        if (letterCount < MAX_INPUT_CHARS)
        {
            // Draw blinking underscore char
            if (((framesCounter / 20) % 2) == 0)
                DrawText("_", (int)textBox.x + 8 + MeasureText(name, 40), (int)textBox.y + 12, 40, MAROON);
        }
        else
            DrawText("Press BACKSPACE to delete chars...", 230, 300, 20, GRAY);
    }
}

void menuGUI(Texture2D background, Texture2D midground, Texture2D foreground, ScrollingPositions positions,GameState *currentState)
{
  
     ClearBackground(GetColor(0x052c46ff));

               ClearBackground(GetColor(0x052c46ff));

    // Desenhar imagem de fundo duas vezes
    DrawTextureEx(background, (Vector2){ positions.scrollingBack, 20 }, 0.0f, 2.0f, WHITE);
    DrawTextureEx(background, (Vector2){ background.width*2 + positions.scrollingBack, 20 }, 0.0f, 2.0f, WHITE);

    // Desenhar imagem de meio fundo duas vezes
    DrawTextureEx(midground, (Vector2){ positions.scrollingMid, 20 }, 0.0f, 2.0f, WHITE);
    DrawTextureEx(midground, (Vector2){ midground.width*2 + positions.scrollingMid, 20 }, 0.0f, 2.0f, WHITE);

    // Desenhar imagem de primeiro plano duas vezes
    DrawTextureEx(foreground, (Vector2){ positions.scrollingFore, 70 }, 0.0f, 2.0f, WHITE);
    DrawTextureEx(foreground, (Vector2){ foreground.width*2 + positions.scrollingFore, 70 }, 0.0f, 2.0f, WHITE);

    DrawText("BACKGROUND SCROLLING & PARALLAX", 10, 10, 20, RED);
    DrawText("(c) Cyberpunk Street Environment by Luis Zuno (@ansimuz)", screenWidth - 330, screenHeight - 20, 10, RAYWHITE);
    
     
    // Desenhar título
    // Declaração das variáveis globais
Button btnPlay = { (Rectangle){ screenWidth/2 - 100, 200, 200, 50 }, "Jogar", false, false };
Button btnOptions = { (Rectangle){ screenWidth/2 - 100, 300, 200, 50 }, "Opções", false, false };
Button btnExit = { (Rectangle){ screenWidth/2 - 100, 400, 200, 50 }, "Sair", false, false };

        DrawButton(btnPlay);
        DrawButton(btnOptions);
        DrawButton(btnExit);

        
    /// Verificar cliques
    if (IsButtonClicked(btnPlay)) {
        // Lógica para quando o botão "Jogar" é clicado
        *currentState = STATE_TELA_INPUT;
         
    }
   
    if (IsButtonClicked(btnExit)) {
        // Lógica para quando o botão "Sair" é clicado
 
        CloseWindow();  // Fechar a janela do Raylib
    }
     
        
        
}
// Função de atualização da tela do menu
ScrollingPositions UpdateScrolling(ScrollingPositions positions, float backgroundWidth, float midgroundWidth, float foregroundWidth)
{
    positions.scrollingBack -= 0.1f;
    positions.scrollingMid -= 0.5f;
    positions.scrollingFore -= 1.0f;

    // Ajustar a posição de rolagem para o efeito contínuo
    if (positions.scrollingBack <= -backgroundWidth*2) positions.scrollingBack = 0;
    if (positions.scrollingMid <= -midgroundWidth*2) positions.scrollingMid = 0;
    if (positions.scrollingFore <= -foregroundWidth*2) positions.scrollingFore = 0;

    return positions;
}

// Função para desenhar um botão
void DrawButton(Button button) {
    // Desenhar retângulo do botão
    DrawRectangleRec(button.bounds, button.hovered ? BLUE : DARKBLUE);

    // Desenhar texto centralizado no botão
    DrawText(button.text, button.bounds.x + button.bounds.width/2 - MeasureText(button.text, 20)/2,
             button.bounds.y + button.bounds.height/2 - 10, 20, WHITE);
}

_Bool IsButtonClicked(Button button)
{
    return (CheckCollisionPointRec(GetMousePosition(), button.bounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON));
}