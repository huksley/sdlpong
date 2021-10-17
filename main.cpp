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
//#include <SDL2/SDL.h>
#include "SDL.h"
// #include "SDL2_ttf/include/SDL_ttf.h"

#include <stdio.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define WINDOW_TITLE "Ping"
#define WINDOW_TEXT "Pong"

int main(int argc, char *args[])
{
    SDL_Window *window = NULL;         // The window we are rendering to
    SDL_Surface *screenSurface = NULL; // The surface contained by the window

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("[ERROR] SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED_DISPLAY(1), SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL)
    {
        printf("[ERROR] Window could not be created! SDL Error: %s\n", SDL_GetError());
        return 1;
    }

    int height = 65;
    int width = 15;
    int moveDelta = 30;

    int leftX = 20;
    int leftY = SCREEN_HEIGHT / 2 - height / 2;
    int leftUp = SDLK_q;
    int leftDown = SDLK_a;

    int rightX = SCREEN_WIDTH - 20 - width;
    int rightY = SCREEN_HEIGHT / 2 - height / 2;
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

    screenSurface = SDL_GetWindowSurface(window);
    Uint32 color = SDL_MapRGB(screenSurface->format, 240, 240, 240);
    Uint32 backgroundColor = SDL_MapRGB(screenSurface->format, 0, 0, 0);
    SDL_FillRect(screenSurface, NULL, backgroundColor);
    SDL_UpdateWindowSurface(window);

    /*
    Control: Q,A left paddle, P,L right paddle
    Space: Start game
    ==================


     |       *      |

     
    ==================
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

        // Update ball position
        ballX += ballSpeedX;
        ballY += ballSpeedY;

        // Check ball collision right wall / paddle
        if (ballX >= right.x)
        {
            if (ball.y >= right.y && ball.y <= right.y + right.h)
            {
                ballSpeedX *= -1;
            }
            else
            {
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
                // When you press a key, change coord up LEFT
                if (event.key.keysym.sym == leftUp)
                {
                    leftY -= moveDelta;
                }
                // When you press a key, change coord down LEFT
                if (event.key.keysym.sym == leftDown)
                {
                    leftY += moveDelta;
                }
                // When you press a key, change coord up RIGHT
                if (event.key.keysym.sym == rightUp)
                {
                    rightY -= moveDelta;
                }
                // When you press a key, change coord down RIGHT
                if (event.key.keysym.sym == rightDown)
                {
                    rightY += moveDelta;
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
    return 0;
}
