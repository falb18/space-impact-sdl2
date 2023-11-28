#include "graphics.h"
#include "shotlist.h"

Uint8 ShotDamages[4] = {1, 3, 10, 25};
Vec2 ShotSizes[4] = {{3, 1}, {5, 3}, {84, 3}, {1, 43}};

void AddShot(ShotList *Shots, Vec2 Pos, Sint8 v, Uint8 FromPlayer, WeaponKind Kind) {
    Shot *CreateAt = *Shots;
    
    /* If list is not empty, append the shot at the end of the list */
    if (*Shots) {
        
        /* Walk throught the list until the last element is found */
        while (CreateAt->Next) {
            CreateAt = CreateAt->Next;
        }
        
        CreateAt->Next = (Shot*)malloc(sizeof(Shot));
        CreateAt = CreateAt->Next;
    } else {
        CreateAt = *Shots = (Shot*)malloc(sizeof(Shot));
    }
    
    CreateAt->Pos = Pos;
    CreateAt->v = v;
    CreateAt->FromPlayer = FromPlayer;
    CreateAt->Kind = Kind;
    
    /* Set type-specific properties depending on the weapon type */
    CreateAt->Damage = ShotDamages[Kind];
    CreateAt->Size = ShotSizes[Kind];
    CreateAt->Next = NULL;
}

void EmptyShotList(ShotList *Shots) {
    Shot* Last = *Shots; /* Store address of last element to delete */
    
    while (*Shots) {
        *Shots = (*Shots)->Next;
        free(Last);
        Last = *Shots;
    }
    
    *Shots = NULL; /* Don't point anywhere because nothing is reserved */
}

/* Removes the element at the address given in the input from the list given in the input */
void RemoveShot(ShotList *Shots, Shot *Address) {
    Shot *Checked = *Shots, *Last = *Shots;
   
    while (Checked) {
        if (Checked == Address) {
            Shot *Current = Checked;

            if (Checked == *Shots)
                *Shots = (*Shots)->Next;
            
            Last->Next = Current->Next;
            free(Current);
            
            return; /* From there, the function has nothing to do, it can be interrupted */
        }
        
        Last = Checked; /* The last element is the one currently under investigation */
        Checked = Checked->Next; /* It examines the next element in the next round */
    }
}

/* Retrieves data for two rectangles in starting position and size, then returns a true-false value to see if they have a common point */
Uint8 Intersect(Vec2 Start1, Vec2 Size1, Vec2 Start2, Vec2 Size2) {
    /* The two rectangles overlap unless 1 is completely to the left / right / up / down of 2 */
    return !(Start1.x > Start2.x + Size2.x - 1 || Start1.y > Start2.y + Size2.y - 1 || Start1.x + Size1.x - 1 < Start2.x || Start1.y + Size1.y - 1 < Start2.y);
}


void ShotListTick(ShotList *Shots, Uint8 *PixelMap, PlayerObject *Player) {
    Shot *Current = *Shots;
    
    while (Current) {
        /* Check if the player has been hit by an enemy */
        Uint8 HitPlayer = !Current->FromPlayer && Intersect(Current->Pos, Current->Size, Player->Pos, NewVec2(10, 7));
        
        /* Did the player's shot hit an enemie's projectile */
        Uint8 HitOther = 0;
        Shot *CheckForHit = *Shots;
        Current->Pos.x += Current->v;
        
        /* If player is hit and doesn't have the shield it losses a life */
        if (HitPlayer && !Player->Protection)
            Player->Lives--;
        
        while (CheckForHit && !HitOther) {
            /* Avoid collision between player and its bullets */
            HitOther = CheckForHit != Current && !(CheckForHit->FromPlayer && Current->FromPlayer) &&
            /* Check collision */
            Intersect(Current->Pos, Current->Size, CheckForHit->Pos, CheckForHit->Size);

            if (HitOther) {
                Current->Damage--;
                CheckForHit->Damage--;

                if (CheckForHit->Damage == 0)
                    RemoveShot(Shots, CheckForHit);
            }

            CheckForHit = CheckForHit->Next;
        }

        /* Delete the bullet if it goes off the screen, hits the player or it can't no longer do any damage */
        if (Current->Pos.x < -2 || Current->Pos.x > 83 || HitPlayer || Current->Damage == 0) {
            Shot* Remove = Current;
            Current = Current->Next;
            RemoveShot(Shots, Remove);
        } else {
            DrawObject(PixelMap, GetObject(Current->Kind == Standard ? gShot : G_MISSILE + Current->Kind - Missile), Current->Pos);
            Current = Current->Next;
        }
    }
}
