#include <stdio.h>
#include <math.h>
#include "SDL.h"
#include "SDL_ttf.h"

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
    char score[200];

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

            snprintf(score, 200, "Score %i:%i", scoreLeft, scoreRight);
            drawText(screenSurface, score, 18, 10, 10, textColor, black);
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
                scoreLeft += 1;
                game = false;
                rightY = centerY;
                leftY = centerY;
                ballX = ballCenterX;
                ballY = ballCenterY;
                SDL_Log("Game ended!");
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
                scoreRight += 1;
                game = false;
                rightY = centerY;
                leftY = centerY;
                ballX = ballCenterX;
                ballY = ballCenterY;
                SDL_Log("Game ended!");
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

        // If !game, game isnt live
        if (!game)
        {
            snprintf(score, 200, "Score %i:%i. Press SPACE to play. Q, A - left, P, L - right.", scoreLeft, scoreRight);
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
                // ESCAPE to exit the game
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    eQuit = true;
                }
                // SPACE to start the game
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

        if (game)
        {
            // Log every 10 frames
            if (iteration > 0 && iteration % 10 == 0)
            {
                SDL_Log("Ball x %f y %f, speedx %f speedy %f\n", ballX, ballY, ballSpeedX, ballSpeedY);
            }
            iteration++;
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    TTF_Quit();
    return 0;
}
