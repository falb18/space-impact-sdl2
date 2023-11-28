#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <stdlib.h>
#include <SDL2/SDL.h>

typedef struct App {
    SDL_Window *window;
    SDL_Renderer *renderer;
} App;

/* WeaponKind: Projectile type */
typedef enum {
    Standard = 0, /* Smooth shot */
    Missile = 1, /* Missile */
    Beam = 2, /* Laser beam */
    Wall = 3 /* Wall */
} WeaponKind;

/* Graphics: Object IDs */
typedef enum {
    /* Numbers, size 3 * 5 */
    gNum0 = 0, gNum1, gNum2, gNum3, gNum4, gNum5, gNum6, gNum7, gNum8, gNum9,
    /* Menu items */
    gSpace, gIntro, gImpact, gScrollMark, gDotEmpty, gDotFull,
    /* Player related models and icons */
    gLife, /* Life Mark */
    gMissileIcon, /* Rocket icon */
    gBeamIcon,/* Radius icon */
    gWallIcon,
    gShot, /* Bullet */
    gExplosionA1, gExplosionA2 /* Explosion animation, 2 sprites */
} Graphics;

/* Vec2: Two-dimensional vector, stores longitude and latitude */
typedef struct Vec2 {
    Sint16 x, y; /* It is signable because it can be a projectile going out to the left of the screen or an enemy floating in from above. */
} Vec2;

/* struct Object
 * 
 * int sz_x, sz_y: image size
 * uint8_t samples: (Size.x * Size.y) array for object pixels
 */
typedef struct Object {
    Vec2 Size; /* The size of the object */
    Uint8 *Samples; /* Size.x * Size.y size array for the pixels of the object */
} Object;

/* PlayerObject: The player object */
typedef struct PlayerObject {
    Vec2 Pos; /* The position of its upper left corner on the screen */
    Uint8 Lives; /* Lives, one life = one hit */
    Uint16 Score; /* Score: although the display can show results up to 99999, reaching the maximum of Uint16 is impossible */
    Uint8 Bonus; /* Bonus weapon attacks remaining */
    WeaponKind Weapon; /* Current bonus weapon */
    Uint8 Protection; /* How many frames of damage protection are left */
} PlayerObject;

#endif /* STRUCTURES_H */