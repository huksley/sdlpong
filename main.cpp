//---------------------------------------------------------------------
//  Name:       HelloSDL2.cpp
//  Author:     EAML
//  Date:       2021-05-16
//
//  Description:
//      A minimal PoC for producing a native SDL2 Windows app that can
//      be ran from either Windows Explorer or from Powershell console.
//      It's designed to use minimal command line, compiler options,
//      and dependencies... It will display a gray window for 2 sec's.
//
//  Dependencies:
//      [1] LLVM Clang++ compiler package
//      [2] SDL2 Libraries (DLL's) and Header files (*.h)
//      [3] TTF Libraries (DLL's) and Header files (*.h)
//
//  Notes:
//      There is a slight variation in the bahaviour, depending on:
//      (a) if you compile as a Windows GUI:  the text will not show.
//      (b) if you compile as a console CLI:  text will show in both terminal and/or in a 2nd new window
//      (c) You may need to use "main()" for console and "WinMain()" for GUI...
//      (c) to install on Linux, use packages:  clang, libsdl2-dev
//      (d) Someone said: #define SDL_MAIN_HANDLED ...
//
//  To Run:
//      cp .\SDL2\lib\x64\SDL2.dll C:\Windows\.     # For SDL2
//      cp .\SDL2_ttf\lib\x64\*.dll C:\Windows\.    # For SDL2 TTF
//      cp C:\Windows\Fonts\arial.ttf .             # Get a font...
//
//  For a CLI version, with console output in 2nd Window:
//  # clang++.exe -std=c++17 main.cpp -o main.exe -L .\SDL2\lib\x64\ -L .\SDL2_ttf\lib\x64\ -I .\SDL2_ttf\include\ -I .\SDL2\include\ -lShell32 -lSDL2main -lSDL2 -lSDL2_ttf -Wno-narrowing -Xlinker /subsystem:console
//
//  For a GUI version, without any console output:
//  # clang++.exe -std=c++17 main.cpp -o main.exe -L .\SDL2\lib\x64\ -L .\SDL2_ttf\lib\x64\ -I .\SDL2_ttf\include\ -I .\SDL2\include\ -lShell32 -lSDL2main -lSDL2 -lSDL2_ttf -Wno-narrowing -Xlinker /subsystem:windows
//
//  References:
//      [1] https://github.com/llvm/llvm-project/releases
//      [2] http://www.libsdl.org/release/SDL2-devel-2.0.14-VC.zip
//      [3] https://www.libsdl.org/projects/SDL_ttf/release/SDL2_ttf-devel-2.0.15-VC.zip
//      [4] https://www.libsdl.org/projects/SDL_ttf/docs/SDL_ttf.html
//      [5] http://www.sdltutorials.com/sdl-ttf
//      [6] https://www.libsdl.org/release/SDL-1.2.15/docs/html/sdlevent.html
//      [7] https://stackoverflow.com/q/67559556/
//---------------------------------------------------------------------
// Macos
// http://www.openscenegraph.org/projects/osg/wiki/Support/Tutorials/MacOSXTips
// https://gamedev.stackexchange.com/questions/4253/in-pong-how-do-you-calculate-the-balls-direction-when-it-bounces-off-the-paddl
//#include <SDL2/SDL.h>

#include <stdio.h>
#include <math.h>
#include "SDL.h"
#include "SDL_ttf.h"

// #include "SDL2_ttf/include/SDL_ttf.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define WINDOW_TITLE "Ping"
#define WINDOW_TEXT "Pong"

void drawText(SDL_Surface *screen, char *string, int size, int x, int y, SDL_Color fgC, SDL_Color bgC)
{
    // Remember to call TTF_Init(), TTF_Quit(), before/after using this function.
    TTF_Font *font = TTF_OpenFont("Ubuntu-R.ttf", size);
    if (!font)
    {
        SDL_Log("[ERROR] TTF_OpenFont() Failed with: %s\n", TTF_GetError());
        return;
    }

    TTF_SetFontStyle(font, TTF_STYLE_BOLD);
    //SDL_Surface* textSurface = TTF_RenderText_Solid(font, string, fgC);     // aliased glyphs
    SDL_Surface *textSurface = TTF_RenderText_Shaded(font, string, fgC, bgC); // anti-aliased glyphs
    SDL_Rect textLocation = {x, y, 0, 0};
    SDL_BlitSurface(textSurface, NULL, screen, &textLocation);
    SDL_FreeSurface(textSurface);
    TTF_CloseFont(font);
}

#if defined(_WIN32) && defined(_MINGW32)
#include <windows.h>
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
#else
int main(int argc, char *args[])
#endif
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("[ERROR] SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        WINDOW_TITLE,
        SDL_WINDOWPOS_CENTERED_DISPLAY(1),
        SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS);

    if (window == NULL)
    {
        printf("[ERROR] Window could not be created! SDL Error: %s\n", SDL_GetError());
        return 1;
    }

    if (TTF_Init() == -1)
    {
        printf("[ERROR] TTF_Init() Failed with: %s\n", TTF_GetError());
        return 1;
    }

    int height = 65;
    int width = 15;
    int moveDelta = 30;

    int leftX = 20;
    int centerY = SCREEN_HEIGHT / 2 - height / 2;
    int leftY = centerY;
    int leftUp = SDLK_q;
    int leftDown = SDLK_a;

    int rightX = SCREEN_WIDTH - 20 - width;
    int rightY = centerY;
    int rightUp = SDLK_p;
    int rightDown = SDLK_l;

    int ballW = 10;
    int ballH = 10;
    double ballCenterX = SCREEN_WIDTH / 2 - ballW / 2;
    double ballCenterY = SCREEN_HEIGHT / 2 - ballH / 2;
    double ballSpeedX = 5;
    double ballSpeedY = 1;
    double ballX = ballCenterX;
    double ballY = ballCenterY;
    int scoreLeft = 0;
    int scoreRight = 0;

    SDL_Surface *screenSurface = SDL_GetWindowSurface(window);
    Uint32 color = SDL_MapRGB(screenSurface->format, 240, 240, 240);
    Uint32 backgroundColor = SDL_MapRGB(screenSurface->format, 0, 0, 0);
    SDL_Color textColor = {0xff, 0xff, 0xff};
    SDL_Color black = {0, 0, 0};
    SDL_FillRect(screenSurface, NULL, backgroundColor);
    SDL_UpdateWindowSurface(window);
    bool game = false;

    /*
    Control: Q,A left paddle, P,L right paddle
    Space: Start game
    ===================


    |        *        |

     
    ===================
    */

    SDL_Event event;
    bool eQuit = false;
    long iteration = 0;
    while (!eQuit)
    {
        SDL_FillRect(screenSurface, NULL, backgroundColor);

        // Draw left paddle
        SDL_Rect left;
        left.x = leftX;
        left.y = leftY;
        left.w = width;
        left.h = height;
        SDL_FillRect(screenSurface, &left, color);

        // Draw right paddle
        SDL_Rect right;
        right.x = rightX;
        right.y = rightY;
        right.w = width;
        right.h = height;
        SDL_FillRect(screenSurface, &right, color);

        // Draw ball
        SDL_Rect ball;
        ball.x = round(ballX - ballW / 2);
        ball.y = round(ballY - ballH / 2);
        ball.w = ballW;
        ball.h = ballH;
        SDL_FillRect(screenSurface, &ball, color);

        if (game)
        {
            // Update ball position
            ballX += ballSpeedX;
            ballY += ballSpeedY;
        }

        // Check ball collision right wall / paddle
        if (ballX >= right.x)
        {
            if (ball.y >= right.y && ball.y <= right.y + right.h)
            {
                ballSpeedX *= -1;
            }
            else
            {
                game = false;
                rightY = centerY;
                leftY = centerY;
                ballX = ballCenterX;
                ballY = ballCenterY;
            }
        }

        // Check ball collision left wall / paddle
        if (ballX <= left.x + width)
        {
            if (ball.y >= left.y && ball.y <= left.y + left.h)
            {
                ballSpeedX *= -1;
            }
            else
            {
                game = false;
                rightY = centerY;
                leftY = centerY;
                ballX = ballCenterX;
                ballY = ballCenterY;
            }
        }

        // Check ball collision top wall
        if (ballY <= 0)
        {
            ballSpeedY *= -1;
        }

        // Check ball collision bottom wall
        if (ballY >= SCREEN_HEIGHT)
        {
            ballSpeedY *= -1;
        }

        if (!game)
        {
            char score[200];
            snprintf(score, 200, "Score %i:%i. Press SPACE to play. Q,A - left, P,L - right.", scoreLeft, scoreRight);
            drawText(screenSurface, score, 18, 10, 10, textColor, black);
        }

        SDL_UpdateWindowSurface(window);

        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                eQuit = true;
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    eQuit = true;
                }
                if (event.key.keysym.sym == SDLK_SPACE)
                {
                    game = !game;
                }
                if (game)
                {
                    // When you press a key, change coord up LEFT
                    if (event.key.keysym.sym == leftUp)
                    {
                        if (leftY >= 0)
                        {
                            leftY -= moveDelta;
                        }
                    }
                    // When you press a key, change coord down LEFT
                    if (event.key.keysym.sym == leftDown)
                    {
                        if (leftY <= SCREEN_HEIGHT - left.h)
                        {
                            leftY += moveDelta;
                        }
                    }
                    // When you press a key, change coord up RIGHT
                    if (event.key.keysym.sym == rightUp)
                    {
                        if (rightY >= 0)
                        {
                            rightY -= moveDelta;
                        }
                    }
                    // When you press a key, change coord down RIGHT
                    if (event.key.keysym.sym == rightDown)
                    {
                        if (rightY <= SCREEN_HEIGHT - right.h)
                        {
                            rightY += moveDelta;
                        }
                    }
                }
                break;
            case SDL_WINDOWEVENT_CLOSE:
                eQuit = true;
                break;
            default:
                //SDL_Log("Got unknown event type %i, id %d\n", event.type, event.window.event);
                break;
            }
        }

        SDL_Delay(15); // Keep < 500 [ms]

        // Log every 10 frames
        if (iteration > 0 && iteration % 10 == 0)
        {
            SDL_Log("Ball x %f y %f, speedx %f speedy %f\n", ballX, ballY, ballSpeedX, ballSpeedY);
        }
        iteration++;
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    TTF_Quit();
    return 0;
}
