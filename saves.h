#ifndef SAVES_H
#define SAVES_H

#include <SDL2/SDL.h>
#include <stdio.h>

/* The save functions can be found in saves.c */
void ReadSavedLevel(Uint8*);
void ReadTopScore(unsigned int*);
void SaveLevel(Uint8);
void PlaceTopScore(unsigned int*, Uint16);
void FillFileName(char*, Uint16);

#endif /* SAVES_H */
