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

    PlayerObject Player;

    /* Level ID, main menu == -1 */
    Sint8 Level = -1;

    /* Option selected in the menu */
    Uint8 MenuItem = 1;

    Uint8 SavedLevel = 0;

    /* Player flags, toggled when corresponding button is pressed */
    Uint8 PlayerUp, PlayerDown, PlayerLeft, PlayerRight, PlayerShooting = 0;

    /* Wait 5 frames until next player's shoot */
    Uint8 PlayerShootTimer = 0;

    /* How many levels does the game data folder has? */
    Uint8 LevelCount = 0;

    /* Flag to enable landscape scrolling
     * Set to false after the end of each level
     */
    Uint8 MoveScene;

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
            
            case SDL_KEYDOWN:
                /* If any key is pressed stop intro animation */
                if (IntroPhase) {
                    IntroPhase = 0;
                    // AudioFlags |= SOUND_MENUBTN;
                
                #ifdef PAUSE
                /* Pause game */
                } else if (Level == MENU_SCREEN_PAUSE) {
                    if (e.key.keysym.sym == SDLK_RETURN) {
                        Level = MenuItem == 1 ? SavedLevel : MENU_SCREEN_MAIN;
                    } else if (e.key.keysym.sym == SDLK_UP || e.key.keysym.sym == SDLK_DOWN)
                        MenuItem = 3 - MenuItem;
                    else if (e.key.keysym.sym == SDLK_ESCAPE)
                        Level = MENU_SCREEN_MAIN;
                #endif /* PAUSE */
                
                /** Record and game end screen **/
                } else if (Level == MENU_SCREEN_HIGH_SCORE || Level == LevelCount) {
                    if (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_ESCAPE) /* Returns to the main menu with Enter or Esc */
                        Level = -1;
                    // AudioFlags |= SOUND_MENUBTN; /* Make any button sound, even if it does nothing */
                
                /* Main menu */
                } else if (Level == MENU_SCREEN_MAIN) {
                    // AudioFlags |= SOUND_MENUBTN; /* Make any button sound, even if it does nothing */
                    if (e.key.keysym.sym == SDLK_RETURN) { /* Enter */
                        if (SavedLevel == 0)
                            MenuItem++; /* Relates the menu items to the menu that starts with the resume (new game instead of 1st, 2nd, etc.) */
                        if (MenuItem == 3) {
                            Level = MENU_SCREEN_HIGH_SCORE; /* This is the ID of the record screen */
                            #ifdef LEGACY_TOP_SCORE
                            TimeInScores = 0; /* Start the animation of the record screen from the beginning */
                            #endif /* LEGACY_TOP_SCORE */
                            if (SavedLevel == 0) /* Because the menu item ID has increased for the screen without continue, it must be reseti */
                                MenuItem = 2;
                        } else {

                            /* An option to load a level has been selected */
                            
                            if (MenuItem == 1) /* Select Continue */
                                Level = SavedLevel;
                            else /* Select New Game */
                                Level = 0;
                            
                            /* Clear shot list if there's any left from the previous game */
                            // EmptyShotList(&Shots);

                            /* Reset player properties */
                            Player.Lives = 3;
                            Player.Score = 0;
                            /* In a new game player always begins with three missiles */
                            Player.Bonus = 3;
                            Player.Weapon = Missile;
                            /* Player's starting position */
                            Player.Pos = NewVec2(3, 20);
                            Player.Protection = 50;
                            PlayerShootTimer = 0;
                            
                            
                            PlayerUp = PlayerDown = PlayerLeft = PlayerRight = PlayerShooting = 0;
                            // LevelSpawner(&Enemies, Level);
                            // EmptyScenery(&Scene);
                            MoveScene = 1;
                        }
                    }
                    else if (e.key.keysym.sym == SDLK_ESCAPE) /* Also exit the Esc key */
                        run = 0;
                    else if (e.key.keysym.sym == SDLK_UP) /* Up arrow: previous menu item, circular */
                        MenuItem = MenuItem == 1 ? (SavedLevel ? 3 : 2) : (MenuItem - 1);
                    else if (e.key.keysym.sym == SDLK_DOWN) /* Down arrow: Next menu item, in a circle */
                        MenuItem = MenuItem % (SavedLevel ? 3 : 2) + 1;
                
                }
                break;
            
            case SDL_KEYUP: /* When the keys are released, the variables recording the hold are reset */
                /* These are only in-game, but you don't need to check this, because the values have no effect elsewhere */
                /* By the way, this was a switch, only that would increase the generated code by 4 bytes */
                if (e.key.keysym.sym == SDLK_UP)
                    PlayerUp = 0;
                if (e.key.keysym.sym == SDLK_DOWN)
                    PlayerDown = 0;
                if (e.key.keysym.sym == SDLK_LEFT)
                    PlayerLeft = 0;
                if (e.key.keysym.sym == SDLK_RIGHT)
                    PlayerRight = 0;
                if (e.key.keysym.sym == SDLK_SPACE)
                    PlayerShooting = 0;
                break;
            
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
                /********* Main menu *********/
                } else if (Level == MENU_SCREEN_MAIN) {
                    /* The beginning of the menu item indicator, on Nokia 3310 it is 8-2- */
                    DrawSmallNumber(PixelMap, 8, 1, NewVec2(65, 0)); /* Calling the number writer costs fewer bytes */
                    DrawObject(PixelMap, GetObject(gShot), NewVec2(69, 2));
                    DrawSmallNumber(PixelMap, 2, 1, NewVec2(73, 0));
                    DrawObject(PixelMap, GetObject(gShot), NewVec2(77, 2));
                    DrawSmallNumber(PixelMap, gNum0 + MenuItem, 1, NewVec2(81, 0)); /* The number of the selected menu item */
                    DrawText(PixelMap, SavedLevel ? "Continue\nNew game\nTop score" : "New game\nTop score", NewVec2(1, 7), 11); /* List menu items, all as text */
                    InvertScreenPart(PixelMap, NewVec2(0, MenuItem * 11 - 5), NewVec2(76, MenuItem * 11 + 5)); /* Inverts the image around the selected menu item */
                    DrawText(PixelMap, "Select", NewVec2(24, 40), 0); /* Select inscription at the bottom */
                    DrawScrollBar(PixelMap, (MenuItem - 1) * (SavedLevel ? 50 : 100)); /* Draw a scroll bar */
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