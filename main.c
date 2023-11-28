#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "config.h"
#include "font.h"
#include "graphics.h"
#include "structures.h"

#define MENU_SCREEN_MAIN -1
#define MENU_SCREEN_HIGH_SCORE -2
#define MENU_SCREEN_PAUSE -3

/* This function updates the scene every frame */
static Uint32 FrameUpdate(Uint32 ms, void *param) {
    SDL_Event ev;

    /* Set a USEREVENT which later is read by the game loop */
    ev.type = SDL_USEREVENT;
    
    /* Although param is NULL, use it to send the event */
    if (param == NULL)
        SDL_PushEvent(&ev);
    
    return ms;
}

/* Initialize SDL for a given image size */
static void SDL_StartApplication(App *game, Vec2 FrameSize) {
    int renderer_flags = 0;

    renderer_flags = SDL_RENDERER_ACCELERATED;

    /* Init video, sound and timer */
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Failed to initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }

    game->window = SDL_CreateWindow(GAME_TITLE, SDL_WINDOWPOS_CENTERED,
                    SDL_WINDOWPOS_CENTERED, FrameSize.x, FrameSize.y, 0);
    
    if (game->window == NULL) {
        printf("Failed to create %d x %d window: %s\n",
                SCREEN_WIDTH * UPSCALE_FACTOR, SCREEN_HEIGHT * UPSCALE_FACTOR,
                SDL_GetError());

        exit(1);
    }
    
    /* Prevent blurry images, specially for pixel art */
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

    game->renderer = SDL_CreateRenderer(game->window, -1, renderer_flags);

    if (game->renderer == NULL) {
        printf("Failed to create renderer: %s\n", SDL_GetError());
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    static App game;
    game.window = NULL;
    game.renderer = NULL;

    SDL_Event e;

    /* Set window size */
    Vec2 FrameSize = {SCREEN_WIDTH * UPSCALE_FACTOR, SCREEN_HEIGHT * UPSCALE_FACTOR};

    SDL_StartApplication(&game, FrameSize);

    /* Frame timer */
    SDL_TimerID t;

    /* Flag to exit game */
    Uint8 run = 1;

    /* Iterator variables */
    int i, j, k;

    /* Previous and current pixel buffers, pixels that don't match in both
     * buffers must be redrawn
     */
    Uint8 OldPixelMap[84 * 48], PixelMap[84 * 48];

    /* Remaining frames from the introductory animation */
    Uint8 IntroPhase = 12;

    /* Wait x frames for introduction animation */
    Uint8 FrameHold = 3;

    /* Start timer to update scene
     * The timer calls the function every x frames to update
     * the scene.
     */
    t = SDL_AddTimer(1000 / FRAMERATE, FrameUpdate, NULL);

    /* Intialize pixel map
     * The old pixel map must by fully active (= black) to fill the first frame
     * with the background.
     */
    for (i = 0; i < 84 * 48; ++i)
        OldPixelMap[i] = 0;

    /* Load pixels for fonts */
    UncompressFont();

    /* Load pixels for introduction animation */
    UncompressObjects();

    /* Game loop */
    srand(time(NULL));

    while (run) {
        SDL_WaitEvent(&e);

        switch (e.type) {
            
            /* Update scene after timeout */
            case SDL_USEREVENT:
                memset(PixelMap, 0, sizeof(PixelMap));

                /* Animate game intro */
                if (IntroPhase) {
                    
                    DrawObject(PixelMap, GetObject(gSpace), NewVec2(8, 12 - IntroPhase)); /* The text "Space" should float in from the top */
                    DrawObject(PixelMap, GetObject(gImpact), NewVec2(4, 24 + IntroPhase)); /* Impact will swim in from below */
                    DrawOutlinedObject(PixelMap, GetObject(gIntro), NewVec2(56 - IntroPhase * 4, 20)); /* The ships are swimming in the middle */
                    
                    /* Switch to next intro scene after x frames */
                    if (FrameHold > 0) {
                        --FrameHold;
                        if (FrameHold != 0) {
                            /* Wait for one second after last frame */
                            FrameHold = (--IntroPhase == 1) ? FRAMERATE : 2;
                        }
                    }
                }

                /******** Draw scene ********/

                /* Update scene: */
                /* Set background color */
                SDL_SetRenderDrawColor(game.renderer, 178, 189, 8, 255);
                SDL_RenderClear(game.renderer);

                /* Set front color */
                SDL_SetRenderDrawColor(game.renderer, 0, 0, 0, 255);

                /* Based on the original screen dimensions */
                for (i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {

                    /* Draw changed pixels only */
                    if (PixelMap[i] != OldPixelMap[i]) {
                        
                        int x = (i % SCREEN_WIDTH) * UPSCALE_FACTOR;
                        int y = (i / SCREEN_WIDTH) * UPSCALE_FACTOR;

                        for (k = 0; k < UPSCALE_FACTOR; k++) {
                            for (j = 0; j < UPSCALE_FACTOR; j++) {
                                if (PixelMap[i]) {
                                    SDL_RenderDrawPoint(game.renderer, x+j, y+k);
                                }
                            }
                        }

                        /* Copy the current scene in the previous one */
                        /* Note: Is possible that this step is not necessary because it caused issues when uncommented */
                        // OldPixelMap[i] = PixelMap[i];
                    }
                }

                SDL_RenderPresent(game.renderer);

                break;
            
            case SDL_QUIT:
                run = 0;
                break;
        }
    }

    SDL_RemoveTimer(t);
    SDL_Quit();
}