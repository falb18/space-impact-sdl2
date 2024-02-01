#ifndef AUDIO_H
#define AUDIO_H

#include <SDL2/SDL.h>

/* Flags of usable sounds */
#define SOUND_SHOT 0x00000001 /* Gunshot */
#define SOUND_DEATH 0x00000002 /* Dead sound */
#define SOUND_BONUSWPN 0x00000004 /* Sound of bonus weapons */
#define SOUND_MENUBTN 0x10000000 /* Men's voice */

/* Frequencies of sounds */
#define BUTTON_FREQ 1000 /* The frequency of the button press sound */
#define SHOT_FREQ_DISTORT 1500 /* The distorted component of the shot */
#define SHOT_FREQ_CONTINOUS 6000 /* Continuous component of shooting */
#define DEATH_FREQ_DISTORT 4200 /* Distorted component of fish */
#define DEATH_FREQ_CONTINOUS 5200 /* Continuous component of fish */
#define BONUS_FREQ_DISTORT 4900 /* The distorted component of the bonus */
#define BONUS_FREQ_CONTINOUS 5250 /* Continuous component of the bonus */

/* You can find audio functions in audio.c */
void AudioCallback(void*, Uint8*, int);

#endif /* AUDIO_H */
