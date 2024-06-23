/*******************************************************************************************
 *
 *   raylib [core] example - Basic 3d example
 *
 *   Welcome to raylib!
 *
 *   To compile example, just press F5.
 *   Note that compiled executable is placed in the same folder as .c file
 *
 *   You can find all basic examples on C:\raylib\raylib\examples folder or
 *   raylib official webpage: www.raylib.com
 *
 *   Enjoy using raylib. :)
 *
 *   This example has been created using raylib 1.0 (www.raylib.com)
 *   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
 *
 *   Copyright (c) 2013-2022 Ramon Santamaria (@raysan5)
 *
 ********************************************************************************************/

#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#define MAX_INPUT_CHARS 15
#define screenWidth 1200
#define screenHeight 650
#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

//----------------------------------------------------------------------------------
// Local Variables Definition (local to this module)

//----------------------------------------------------------------------------------
// Local Functions Declaration
//----------------------------------------------------------------------------------
// Update and draw one frame

//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------
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


int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
     
    GameState currentState = STATE_TELA_INPUT;
    
    
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

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
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
            menuGUI();

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

void menuGUI()
{
   Texture2D background = LoadTexture("imgs/background.png");

    ClearBackground(RAYWHITE);
    DrawTexture(background, 0, 0, WHITE);
     
    // Desenhar título
    // Declaração das variáveis globais
Button btnPlay = { (Rectangle){ screenWidth/2 - 100, 200, 200, 50 }, "Jogar", false, false };
Button btnOptions = { (Rectangle){ screenWidth/2 - 100, 300, 200, 50 }, "Opções", false, false };
Button btnExit = { (Rectangle){ screenWidth/2 - 100, 400, 200, 50 }, "Sair", false, false };

        DrawButton(btnPlay);
        DrawButton(btnOptions);
        DrawButton(btnExit);
        if(btnPlay.clicked == 1 ||  btnExit.clicked == 1){
            UnloadTexture(background);
        }
        
        
}


// Função para desenhar um botão
void DrawButton(Button button) {
    // Desenhar retângulo do botão
    DrawRectangleRec(button.bounds, button.hovered ? BLUE : DARKBLUE);

    // Desenhar texto centralizado no botão
    DrawText(button.text, button.bounds.x + button.bounds.width/2 - MeasureText(button.text, 20)/2,
             button.bounds.y + button.bounds.height/2 - 10, 20, WHITE);
}