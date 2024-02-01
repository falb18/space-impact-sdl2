#ifndef CONFIG_H
#define CONFIG_H

/* Graphics */
#define SCREEN_WIDTH 84
#define SCREEN_HEIGHT 48
#define GAME_TITLE "Space Impact"
#define BACKLIGHT 178, 189, 8 /* RGB code for background color */
#define FRAMERATE 10 /* Frames / second, original 18, but not a divisor of 60, causing a pulldown */
#define UPSCALE_FACTOR 4 /* How many times increase the 84x48 display to 14 recommended HD, 21 Full HD */

/** Voice **/
#define SAMPLE_RATE 44100 /* AudioSampling frequency, when changing it is recommended to recalibrate the shot sound to another frequency that is distorted by the algorithm */
/*#define LEGACY_AUDIO*/ /* In the original game, only one sound was heard at a time, if it is defined, it will be so */
#define VOLUME 4095 /* The volume of a half component, the total volume is 32767, up to 8 components can be played at a time, so the maximum volume without LEGACY_AUDIO is 4095 */

/** Game elements **/
/*#define GHOSTING*/ /* In some versions of the game, only one button could be pressed at a time, if defined, that will be the case */
#define BONUS_COLLIDER /* In some versions of the game, the bonus object has collected projectiles for points, if defined, so it will be */
/*#define ZEROTH_LIFE*/ /* In some versions of the game, the number of lives did not range from 1 to 3, but from 0 to 2, if defined, so it will be */
// #define LEGACY_TOP_SCORE /* Use the original record screen (turn off to see the top 10 score) */
/*#define PAUSE*/ /* If this is defined, pressing the Esc key during a game will open a pause menu */

#endif /* CONFIG_H */
