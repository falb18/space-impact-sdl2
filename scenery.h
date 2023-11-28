#ifndef SCENERY_H
#define SCENERY_H

#include "structures.h"

/** The scene functions can be found in scenery.c **/
void AddScenery(SceneryList*, Graphics, Vec2);
void EmptyScenery(SceneryList*);
void HandleScenery(SceneryList*, Uint8*, Uint8, PlayerObject*, Sint8);

#endif /* SCENERY_H */
