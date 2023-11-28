#include <string.h>
#include "config.h"
#include "enemies.h"
#include "graphics.h"
#include "saves.h"
#include "shotlist.h"


void AddEnemy(EnemyListStart *Enemies, Vec2 Pos, Uint8 EnemyID, Sint8 MoveDir) {
    EnemyList *CreateAt = *Enemies;
    
    /* If the list is not empty add a new enemy at the end of the list */
    if (*Enemies) {
        
        /* Go through the list until the end is reached */
        while (CreateAt->Next) {
            CreateAt = CreateAt->Next;
        }

        /* Create a new enemy at the end of the list */
        CreateAt->Next = (EnemyList*)malloc(sizeof(EnemyList));
        CreateAt = CreateAt->Next;

    } else {
        CreateAt = *Enemies = (EnemyList*)malloc(sizeof(EnemyList));
    }

    CreateAt->Pos = Pos;
    
    /* Get enemy information and its pixelmap */
    CreateAt->Type = GetEnemy(EnemyID);

    CreateAt->AnimState = 0;
    CreateAt->Lives = CreateAt->Type.Lives;
    CreateAt->MoveDir = MoveDir;
    CreateAt->Cooldown = CreateAt->Type.ShotTime;
    CreateAt->Next = NULL;
}

/* Removes the item at the input address from the input list */
void RemoveEnemy(EnemyListStart *List, EnemyList *Address) {
    EnemyList *Checked = *List, *Last = *List; /* The first variable is the title currently being examined, the second is the previous one */
    while (Checked) { /* As long as there is something to watch */
        if (Checked == Address) { /* If you have the match */
            EnemyList *Current = Checked; /* The current memory address */
            if (Checked == *List) /* If the first item in the list is to be deleted, the pointer to the list must be moved to the second item */
                *List = (*List)->Next;
            Last->Next = Current->Next; /* The next indicator of the previously examined element must be shown after the currently examined element */
            free(Current); /* Release the deleted item */
            return; /* From here, the function no longer has anything to do, it can be interrupted */
        }
        Last = Checked; /* The last item is currently under investigation */
        Checked = Checked->Next; /* Examine the next item in the next round */
    }
}

void EmptyEnemyList(EnemyListStart *Enemies) {
    EnemyList* Last = *Enemies; /* Store address of last element to delete */
    
    while (*Enemies) { /* While there are still batteries */
        *Enemies = (*Enemies)->Next; /* The pointer should move to the next element */
        free(Last); /* Release of the previously examined element */
        Last = *Enemies; /* This is now the last item checked */
    }
    
    *Enemies = NULL; /* Don't point anywhere because nothing is reserved */
}

void EnemyListTick(EnemyListStart *Enemies, PlayerObject *Player, Uint8 *PixelMap, ShotList *Shots, Uint8 AnimPulse, Uint8 *MoveScene) {
    Shot* CheckedShot;
    EnemyList* Current = *Enemies;
    
    while (Current) { /* As long as there are enemies on the linked list */
        /* Is the enemy still alive? */
        Uint8 Alive = Current->Lives > 0;

        /* Note: some enemies move back and forth */
        Uint8 InScreen = Current->Pos.x <= 60;
        
        if (!Current->Next && InScreen)
            *MoveScene = 0;
        
        if ((Current->Type.MoveUp || Current->Type.MoveDown) && (InScreen || Current->Type.MoveAnyway)) {
            
            if (Current->MoveDir == 1 && Current->Pos.y == Current->Type.MovesBetween.y)
                Current->MoveDir = -Current->Type.MoveUp;
            else if (Current->MoveDir == -1 && Current->Pos.y == Current->Type.MovesBetween.x)
                Current->MoveDir = Current->Type.MoveDown;
            
            Current->Pos.y += Current->MoveDir;
        }
        
        /* Move enemy until it reaches the other side of the screen. */
        if (!Current->Type.Floats || !InScreen) {
            Current->Pos.x--;
        }
        
        /* Switch between enemy's animation frames */
        if (AnimPulse) {
            Current->AnimState = (Current->AnimState + 1) % Current->Type.AnimCount;
        }

        /* Only check for bullet shots if enemy is alive */
        if (Alive) {
            /* Did the player hit the enemy? */
            Uint8 GotHit = 0;
            CheckedShot = *Shots;
            
            while (CheckedShot && !GotHit) { /* Go through the shooting list if there were no hits yet */
                
                if (CheckedShot->FromPlayer) { /* Enemies should not shoot each other, only the player's shots should be considered */
                    
                    if (Intersect(CheckedShot->Pos, CheckedShot->Size, Current->Pos, Current->Type.Size)) { /* Whether the square drawn around the enemy and the bullet fits */
                        
                        GotHit = 1; /* Hit detected */
                        #ifdef BONUS_COLLIDER
                        if (Current->Type.Lives == 127) {
                            Player->Score += CheckedShot->Damage * 5; /* The total remaining damage to the projectile should be points */
                            RemoveShot(Shots, CheckedShot); /* Remove the bullet anyway */
                        } else {
                        #else
                        if (Current->Type.Lives != 127) { /* If it's not a bonus item, take it as a hit */
                        #endif
                            Current->Lives -= CheckedShot->Damage; /* Reduce enemy life by projectile damage */
                            Player->Score += 5; /* Points reward for the hit */
                            
                            if (Current->Lives < 0) { /* If the projectile has not used up all its damage */
                                CheckedShot->Damage = -Current->Lives; /* The opposite of enemy life will be the remaining damage */
                                Current->Lives = 0; /* The non-positive lives of the enemy are states of explosion animation */
                            } else {
                                RemoveShot(Shots, CheckedShot); /* Remove the bullet when it has run out of damage */
                            }
                            
                            if (Current->Lives == 0) { /* In case of death */
                                Player->Score += 5; /* The player gets 5 more points */
                                Alive = 0; /* The enemy is no longer alive, so act in later branches */
                                Current->Type.MoveUp = Current->Type.MoveDown = 0; /* Disable movements to prevent the blast animation from moving */
                            }
                        }
                    }
                }
                if (!GotHit) /* Only continue the scan if no hit is detected */
                    CheckedShot = CheckedShot->Next; /* Continue with the next item in the chained list */
            }
        }
        
        if (Alive && Intersect(Player->Pos, NewVec2(10, 7), Current->Pos, Current->Type.Size)) { /* If he is still alive, but the player has gone */
            Alive = 0; /* He will definitely die in a collision */
            if (Current->Type.Lives == 127) { /* Give a bonus in the event of a collision with a bonus carrier */
                WeaponKind NewKind = (WeaponKind)(rand() % 3 + 1); /* New Random Bonus Weapon */
                if (NewKind != Player->Weapon) { /* If the player did not have one, reset the number of bonuses and give him one */
                    Player->Bonus = 0;
                    Player->Weapon = NewKind;
                }
                Player->Bonus += 4 - (int)NewKind; /* It gives 3 from a rocket, 2 from a beam, 1 from a wall */
                Current->Lives = -2; /* Skip explosion animations */
            } else if (Player->Lives && !Player->Protection) /* If the player is still alive and has no defense, the main is called first to deal with the shots, in which he may die.g */
                Player->Lives--; /* The player loses 1 life from any damage */
        }

        if (Current->Pos.x < -Current->Type.Size.x || !Alive) { /* If enemy is outside the screen or dead, run the blast animation and after that delete it */
            Current->Lives--; /* Increase animation phase (absolute value of life) */
            
            if (Current->Lives == -3) { /* The final phase: deletion */
                EnemyList* Remove = Current; /* The item to be deleted */
                Current = Current->Next; /* On the next round, continue with the next item in the chained list before the list item is deleted */
                RemoveEnemy(Enemies, Remove); /* Delete an enemy */
                continue; /* Do not run this iteration again, do not reach Current = Current-> Next again; shorts */
            } else { /* Draw explosion animation */
                Vec2 AnimCenter = NewVec2(Current->Pos.x + Current->Type.Size.x / 2, Current->Pos.y + Current->Type.Size.y / 2); /* The center of the animation object */
                /* Left positions are included in the drawing positions */
                /* This, and the animation phase, can be easily calculated if we know that life can
                 * be -1 and -2 depending on the phase, and the IDs of the two objects come one
                 * after the other.
                 */
                DrawObject(PixelMap, GetObject(gExplosionA1 - Current->Lives - 1), NewVec2(AnimCenter.x - 3 - Current->Lives, AnimCenter.y - 2));
            }
        
        } else {
            if (Current->Pos.x < 84) { /* If the enemy is still alive and its position is within the screen */
                DrawObject(PixelMap, GetObject(Current->Type.Model + Current->AnimState), Current->Pos);
                if (Current->Type.ShotTime) { /* If the enemy can shoot */
                    Current->Cooldown--; /* Decrement reloading time */
                    if (Current->Cooldown == 0) { /* If gun reloading time has ended */
                        /* Enemy can shoot now */
                        AddShot(Shots, NewVec2(Current->Pos.x - 1, Current->Pos.y + (Current->Type.Size.y / 2)), -2, 0, Standard);
                        /* Re-start gun reloading time */
                        Current->Cooldown = Current->Type.ShotTime;
                    }
                }
            }
        }
        
        Current = Current->Next; /* Continue with the next item in the chained list */
    }
    /* Bonus weapon management */
    CheckedShot = *Shots; /* The memory address required to traverse the concatenated list */
    
    while (CheckedShot) { /* As long as there is the next item */
        Shot* LastShot = CheckedShot;
        
        if (CheckedShot->Kind == Missile) { /* The case of missiles */
            Uint8 TargetHeight = CheckedShot->Pos.y; /* Target altitude, if there are no nearby enemies, stay here */
            EnemyList* Current = *Enemies; /* You need to look through the list of enemies to determine the one closest to you */
            while (Current) { /* The following rows are based on the order in which the items are listed in ascending order by X position */
                if (Current->Pos.x > 84) /* Stop following enemies that are not visible on the screen */
                    Current = NULL; /* You can safely stop here because they are added one after the other */
                else if (Current->Pos.x > CheckedShot->Pos.x) { /* Once you have the first hit on the enemy to the right of the missile */
                    TargetHeight = Current->Pos.y; /* The goal should be the height of the hit */
                    Current = NULL; /* You can stop the search, you had the nearest one */
                } else /* If there is no reason to stop, continue with the list */
                    Current = Current->Next;
            }
            if (CheckedShot->Pos.y != TargetHeight) { /* If you are not at the target height, head towards it */
                if (CheckedShot->Pos.y < TargetHeight) /* If it is above it, descend */
                    CheckedShot->Pos.y++;
                else  /* Otherwise, get up */
                    CheckedShot->Pos.y--;
            }
        }

        /* If there's a beam type ammo on the field, it must be deleted because it has a lifespan of 1 frame */
        CheckedShot = CheckedShot->Next;
        if (LastShot->Kind == Beam)
            RemoveShot(Shots, LastShot);
    }
}

/* Open a level file */
FILE* GetLevel(Uint8 Level) {
    /* Folder = 12
     * 3-digit ID = 3
     * Null character = 1
     * Start the string with the path to the level files
     */
    char Path[12 + 3 + 1 ] = "data/levels/";
    
    /* Add file name to the path */
    FillFileName(Path, Level);
    
    return fopen(Path, "rb");
}

/* Place enemies in a fixed position */
void LevelSpawner(EnemyListStart *Enemies, Uint8 Level) {
    
    Uint8 EnemyCount;
    FILE* LevelData = GetLevel(Level);
    
    if (!LevelData)
        return;
    
    EmptyEnemyList(Enemies);

    /* Read the number of enemies in the loaded level */
    fread(&EnemyCount, sizeof(Uint8), 1, LevelData);
    
    while (EnemyCount--) {
        Uint8 EnemyData[5];
        fread(&EnemyData, sizeof(Uint8), 5, LevelData);
        AddEnemy(Enemies, NewVec2(EnemyData[0] * 256 + EnemyData[1], EnemyData[2]), EnemyData[3], (Sint8)EnemyData[4] - 1);
    }
    
    fclose(LevelData);
}

/* List of all enemies on the game */ 
Enemy* Enemies[256] = {NULL};

/* Get enemy's properties
 * The information of the enemy is loaded in RAM in case it hasn't.
 * The function return a copy of the properties of the enemy.
 */
Enemy GetEnemy(Uint8 ID) {

    /* Load enemy data if is not yet in the array */
    if (!Enemies[ID]) {
        
        FILE* ObjectData;
        Uint8 i, ModelID;

        /* The two coordinates which the enemy's move up and down */
        Uint8 MovesBetween[2];

        char Path[13 + 3 + 1 ] = "data/enemies/";
        FillFileName(Path, ID);
        ObjectData = fopen(Path, "rb");
        
        /* If the file wasn't found, return an empty object */
        if (!ObjectData) {
            Enemy Empty;
            memset(&Empty, 0, sizeof(Enemy));
            return Empty;
        }
        
        Enemies[ID] = (Enemy*)malloc(sizeof(Enemy));

        fread(&ModelID, sizeof(Uint8), 1, ObjectData);
        
        Enemies[ID]->Model = ModelID + 256;
        Enemies[ID]->Size.x = Enemies[ID]->Size.y = 0;
        
        fread(&Enemies[ID]->AnimCount, sizeof(Uint8), 7, ObjectData);
        fread(MovesBetween, sizeof(Uint8), 2, ObjectData);
        
        Enemies[ID]->MovesBetween.x = MovesBetween[0];
        Enemies[ID]->MovesBetween.y = MovesBetween[1];
        
        fclose(ObjectData);
        
        /* Get pixelmap for each frame in enemy's animation */
        for (i = ModelID; i < ModelID + Enemies[ID]->AnimCount; ++i) {
            Object AnimPhase = GetObject(Enemies[ID]->Model);
            if (Enemies[ID]->Size.x < AnimPhase.Size.x)
                Enemies[ID]->Size.x = AnimPhase.Size.x;
            if (Enemies[ID]->Size.y < AnimPhase.Size.y)
                Enemies[ID]->Size.y = AnimPhase.Size.y;
        }
    }

    return *Enemies[ID];
}

/* Free all dynamically occupied enemies */
void FreeDynamicEnemies() {
    int i;
    for (i = 0; i < 256; i++) /* Walking down the block */
        if (Enemies[i]) /* If you find a loaded dynamic enemy */
            free(Enemies[i]); /* Free the memory */
}
