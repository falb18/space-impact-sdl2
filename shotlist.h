#ifndef SHOTLIST_H
#define SHOTLIST_H

#include "structures.h"

/* A description of the shotlist functions can be found in shotlist.c */
void AddShot(ShotList*, Vec2, Sint8, Uint8, WeaponKind);
void EmptyShotList(ShotList*);
void RemoveShot(ShotList*, Shot*);
Uint8 Intersect(Vec2, Vec2, Vec2, Vec2);
void ShotListTick(ShotList*, Uint8*, PlayerObject*);

#endif /* SHOTLIST_H */
