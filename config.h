#ifndef CONFIG_H
#define CONFIG_H

/* Graphics */
#define SCREEN_WIDTH 84
#define SCREEN_HEIGHT 48
#define GAME_TITLE "Space Impact"
#define BACKLIGHT 178, 189, 8 /* RGB code for background color */
#define FRAMERATE 10 /* Frames / second, original 18, but not a divisor of 60, causing a pulldown */
#define UPSCALE_FACTOR 4 /* How many times increase the 84x48 display to 14 recommended HD, 21 Full HD */

#endif /* CONFIG_H */
