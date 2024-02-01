#include <math.h>
#include "audio.h"
#include "config.h"

#define PIx2 6.283185307179586476925286766559 /* Pi * 2 */

/** Creating a sound from two frequencies **/
void FillStream(Sint16* stream, Sint32 freq_distort, Sint32 freq_sine, Sint32 playrem, Sint32 partrem) {
    Sint32 i;
    for (i = 0; i < (playrem < partrem ? playrem : partrem); i++) /* Remaining samples: either go to the end of the effect or to the end of the callback */
        stream[i] += (sin((playrem - i) * i / /* The spell (*i): pulls up the frequency in all productions */
            /* Note that the spell is really just the result of a completely random "what if..." attempt.
               Then it accidentally made the exact same sound as the original game. Who would have thought? */
            (double)SAMPLE_RATE * PIx2 * freq_distort) + /* Szinuszhullám előállítása: sin(idő*2*pi*frekvencia) */
            sin((playrem - i) / (double)SAMPLE_RATE * PIx2 * freq_sine)) * VOLUME; /* Ugyanígy a másik komponenst is, csak varázslat nélkül */
}

/** Sound mixer: mixes the currently set flag to the output **/
void AudioCallback(void* flags, Uint8* stream8, int len) {
    Sint16* stream = (Sint16*)stream8; /* Casting output to the appropriate type, because basically SDL does not use this format for callbacks */
    static Sint32 btnrem = 0, shotrem = 0, bwpnrem = 0, deathrem = 0; /* Remaining samples of all kinds of sounds */
    memset(stream8, 0, len); /* Drain sound */
    len /= sizeof(Sint16); /* You would have specified the length in bytes */
    if (*(Sint32*)flags & SOUND_MENUBTN) /* If there is a button pressed in the menu */
        btnrem = 2 * SAMPLE_RATE / FRAMERATE; /* Outputs for 2 frames */
    if (*(Sint32*)flags & SOUND_SHOT) /* If there is a gunshot */
        shotrem = 3 * SAMPLE_RATE / FRAMERATE; /* Outputs for 3 frames (do not let the sound of two shots flow into each other, no matter how much the image update is) */
    if (*(Sint32*)flags & SOUND_BONUSWPN) /* If there is a bonus weapon sound */
        bwpnrem = 4 * SAMPLE_RATE / FRAMERATE; /* Outputs for 4 frames */
    if (*(Sint32*)flags & SOUND_DEATH) /* If there is a sound of death */
        deathrem = 6 * SAMPLE_RATE / FRAMERATE; /* Outputs for 6 frames */
    *((Sint32*)flags) = 0; /* Reset flags, don't stay for the next callback */
    /* The sounds cancel each other out according to a priority order */
    #ifdef LEGACY_AUDIO /* If the original audio mapping is set */
    if (btnrem > 0) /* The button press sound is the most important */
        FillStream(stream, 0, BUTTON_FREQ, btnrem, len); /* The button press sound only needs the sine mapping */
    else if (deathrem > 0) /* The sound of death is the most important in the game */
        FillStream(stream, DEATH_FREQ_DISTORT, DEATH_FREQ_CONTINOUS, deathrem, len);
    else if (bwpnrem > 0) /* Bonus weapon sounds are given medium priority */
        FillStream(stream, BONUS_FREQ_DISTORT, BONUS_FREQ_CONTINOUS, bwpnrem, len);
    else if (shotrem > 0) /* The gunshot sound is the least important */
        FillStream(stream, SHOT_FREQ_DISTORT, SHOT_FREQ_CONTINOUS, shotrem, len);
    #else
    FillStream(stream, 0, BUTTON_FREQ, btnrem, len); /* Play button press sound (if there is a condition covered by the length for these) */
    FillStream(stream, SHOT_FREQ_DISTORT, SHOT_FREQ_CONTINOUS, shotrem, len); /* Play gunshot sound */
    FillStream(stream, BONUS_FREQ_DISTORT, BONUS_FREQ_CONTINOUS, bwpnrem, len); /* Play bonus weapon sound */
    FillStream(stream, DEATH_FREQ_DISTORT, DEATH_FREQ_CONTINOUS, deathrem, len); /* Play death sound */
    #endif
    /* Decrease the remaining time of each sound, even if it has not been played */
    /* Yes, you can underflow that way, but it takes >12 hours, we'd rather save bytes than prepare for such a low-probability event */
    btnrem -= len;
    shotrem -= len;
    bwpnrem -= len;
    deathrem -= len;
}
