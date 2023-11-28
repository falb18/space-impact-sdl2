#include "enemies.h"
#include "graphics.h"
#include "scenery.h"

/* Landscape data structure */
typedef struct SceneryData {
    Graphics FirstObject; /* Graphic ID of the first object */
    Uint8 Objects; /* The number of objects that belong to the level starting from the previously specified identifier */
    Uint8 Upper; /* It appears above */
} SceneryData;

/* Landscape data for levels */
SceneryData ScData[6] = {
    { 0, 0, 0 }, /* Level 1 has no scenery */
    { 256 + 0, 2, 0 }, /* Level 2, 2-element, 700px wide landscape from dynamic location 0 */
    { 256 + 2, 6, 0 }, /* Level 3, 6-element landscape from dynamic location 2, 750 pixels wide */
    { 256 + 8, 6, 0 }, /* Level 4, dynamic location 8, 6-element landscape 1000 pixels wide */
    { 256 + 14, 4, 1 }, /* Level 5, from dynamic location 14, 4 elements, 1250 pixels wide upper landscape */
    { 256 + 14, 4, 1 }, /* Level 6, from level 5 elements, 1600 pixels wide */
};

/* Add a location element to the list received in the parameter, with graphic identifier, in position */
void AddScenery(SceneryList *List, Graphics Model, Vec2 Pos) {
    Scenery *CreateAt = *List; /* The creation memory location where the new last element of the list is written */
    if (*List) { /* If there is already a list, you need to find the end and create the new element there */
        while (CreateAt->Next) /* The new address scrolls through the list to the last item */
            CreateAt = CreateAt->Next;
        CreateAt->Next = (Scenery*)malloc(sizeof(Scenery)); /* The last element points to the newly allocated location instead of nothing */
        CreateAt = CreateAt->Next; /* Enter the address used for writing in the variable */
    } else /* If there is no list yet, the address must be reserved for the pointer */
        CreateAt = *List = (Scenery*)malloc(sizeof(Scenery));
    /* Pass data received as input to the new element */
    CreateAt->Model = Model;
    CreateAt->Pos = Pos;
    CreateAt->Next = NULL; /* No next item, end of list */
}

/* Delete the landscape received in the parameter */
void EmptyScenery(SceneryList *List) {
    Scenery* Last = *List; /* Store address of last element to delete */
    while (*List) { /* While there are still batteries */
        *List = (*List)->Next; /* The pointer should move to the next element */
        free(Last); /* Az előzőleg vizsgált elem felszabadítása */
        Last = *List; /* This is now the last item checked */
    }
    *List = NULL; /* Don't point anywhere because nothing is reserved */
}

/* The function that draws the landscape and manages its list, its input is a chained list of the landscape,
 * the pixel map where it will draw, and a true-false value indicating whether the landscape can continue
 * to move or not */
void HandleScenery(SceneryList *List, Uint8 *PixelMap, Uint8 Move, PlayerObject *Player, Sint8 Level) {
    Scenery *First = *List; /* Storing the first element, because the pointer is changed by traversing the list, so it will have to be reset */
    Sint16 LastX = 0; /* The last pixel drawn */
    Object Model; /* Container of the currently drawn model */
    while (*List) { /* As long as there are items in the list */
        if (Move) /* If an enemy is in front of the player, the landscape stops moving */
            (*List)->Pos.x--; /* Otherwise, the landscape keeps swimming to the left */
        Model = GetObject((*List)->Model); /* It is necessary to retrieve the object in advance, due to its size */
        if (Level != 1 && Intersect((*List)->Pos, Model.Size, Player->Pos, NewVec2(10, 7))) /* The player should take damage on impact, even through protection, but not on the cloudy level */
            Player->Lives--;
        if ((*List)->Pos.x < -Model.Size.x) { /* If the entire object floated to the left */
            *List = (*List)->Next; /* Skip ahead to the next item before it eats up your memory space */
            free(First); /* You can easily free up the starting place, because the game fills up the landscape from left to right, so if one of them falls out, it must be the first */
            First = *List; /* The new first item should be the next item in the list */
        } else { /* If the list item is on the screen */
            LastX = (*List)->Pos.x + Model.Size.x; /* Storage of last position, if a landscape element should be added to the end */
            DrawObject(PixelMap, Model, (*List)->Pos); /* Draw a current landscape element */
            *List = (*List)->Next; /* Continue with the next item */
        }
    }
    if (Level != 0) {
        while (LastX < 84) { /* Until the screen is completely drawn */
            Object Model;
            Scenery* NewScenery = (Scenery*)malloc(sizeof(Scenery)); /* Add a new one */
            NewScenery->Model = ScData[Level].FirstObject + rand() % ScData[Level].Objects; /* Choose a random graphic ID from the level's objects */
            Model = GetObject(NewScenery->Model); /* The object will be needed because of its dimensions */
            NewScenery->Pos = NewVec2(LastX, ScData[Level].Upper ? 0 : 48 - Model.Size.y); /* Placement after the current elements, up or down depending on the track */
            NewScenery->Next = NULL; /* No next item, end of list */
            LastX = NewScenery->Pos.x + Model.Size.x; /* Update last position to new item */
            if (First == NULL) /* If there is nothing in the list */
                First = NewScenery; /* Add this using the last line of the function */
            else {
                for (*List = First; (*List)->Next; *List = (*List)->Next); /* Find the end of the list */
                (*List)->Next = NewScenery; /* And add to it */
            }
        }
    }
    *List = First; /* Setting the new first item */
}
