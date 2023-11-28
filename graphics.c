#include "graphics.h"
#include "font.h"
#include "saves.h"

/* Pixel Maps: Pixel maps of some objects are compiled into the game and can be generated from the editor */
Uint8 /* Hardcode pixel maps */
    /* 3x5 numbers */
    pmNum[10][15] = {{1,1,1,1,0,1,1,0,1,1,0,1,1,1,1},
                     {0,0,1,0,0,1,0,0,1,0,0,1,0,0,1},
                     {1,1,1,0,0,1,1,1,1,1,0,0,1,1,1},
                     {1,1,1,0,0,1,1,1,1,0,0,1,1,1,1},
                     {1,0,1,1,0,1,1,1,1,0,0,1,0,0,1},
                     {1,1,1,1,0,0,1,1,1,0,0,1,1,1,1},
                     {1,1,1,1,0,0,1,1,1,1,0,1,1,1,1},
                     {1,1,1,0,0,1,0,0,1,0,0,1,0,0,1},
                     {1,1,1,1,0,1,1,1,1,1,0,1,1,1,1},
                     {1,1,1,1,0,1,1,1,1,0,0,1,1,1,1}},
    pmSpace[804], pmIntro[531], pmImpact[912], /* These pixel maps have been compressed. Only compressing pixel maps larger than 256 reduces application size. */
    cmSpace[101] = {15,255,63,248,127,131,252,127,227,255,199,255,159,249,255,143,252,120,0,224,231,15,60,3,192,30,0,56,28,225,207,0,120,3,192,7,3,28,57,
                    224,15,0,255,240,255,231,255,60,1,255,143,255,31,248,255,231,0,63,224,3,231,192,28,121,224,15,128,0,124,248,7,143,60,1,240,0,15,31,0,
                    241,231,128,62,1,255,227,224,28,56,255,231,255,127,240,248,7,143,7,249,255,12},
    cmIntro[65] = {0,0,0,0,0,0,45,193,128,0,0,14,0,2,244,27,0,0,2,120,0,191,168,240,0,0,62,156,11,219,41,128,0,9,228,242,220,163,192,0,0,224,125,0,1,176,
                   0,0,32,19,192,0,96,0,0,0,1,192,0,0,0,0,0,0,2},
    cmImpact[114] = {31,31,135,207,252,63,193,254,127,241,225,252,252,255,231,254,127,231,255,30,31,255,204,30,225,231,128,7,131,225,255,249,193,206,28,
                     240,0,240,62,63,255,156,24,225,207,0,15,3,195,206,241,255,159,252,240,0,240,60,60,207,31,241,255,206,0,15,3,195,192,227,192,28,121,
                     224,1,224,60,120,14,60,3,199,158,0,30,7,135,129,227,192,60,121,224,1,224,120,120,30,60,3,199,31,252,30,15,143,1,231,128,120,240,127,
                     131,192},
    pmScrollMark[21] = {1,1,0,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,1,1},
    pmDotEmpty[12] = {0,1,1,0,1,0,0,1,0,1,1}, /* Blank dot on the record screen */
    pmDotFull[12] = {0,1,1,0,1,1,1,1,0,1,1}, /* Full dot on the record screen */
    pmLife[25] = {1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,0,0,0,1}, /* Life */
    pmMissileIcon[25] = {0,0,0,0,0,1,0,1,1,0,1,1,1,1,1,1,0,1,1}, /* Rocket icon */
    pmBeamIcon[25] = {0,0,0,0,0,1,1,0,0,0,1,1,1,1,1,1,1}, /* Radius icon */
    pmWallIcon[25] = {0,1,1,1,0,1,1,0,1,1,1,1,0,1,1,1,1,0,1,1,0,1,1,1}, /* Wall icon */
    pmShot[3] = {1,1,1},
    pmExplosion[2][25] = {{0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1}, {0,0,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,1}};

Object StaticObject[] = { /* Static objects, Graphics enumeration stores names for elements */
    {{3, 5}, pmNum[0]}, /* gNum0 */
    {{3, 5}, pmNum[1]}, /* gNum1 */
    {{3, 5}, pmNum[2]}, /* gNum2 */
    {{3, 5}, pmNum[3]}, /* gNum3 */
    {{3, 5}, pmNum[4]}, /* gNum4 */
    {{3, 5}, pmNum[5]}, /* gNum5 */
    {{3, 5}, pmNum[6]}, /* gNum6 */
    {{3, 5}, pmNum[7]}, /* gNum7 */
    {{3, 5}, pmNum[8]}, /* gNum8 */
    {{3, 5}, pmNum[9]}, /* gNum9 */
    {{67, 12}, pmSpace}, /* gSpace */
    {{59, 9}, pmIntro}, /* gIntro */
    {{76, 12}, pmImpact}, /* gImpact */
    {{3, 7}, pmScrollMark}, /* gScrollMark */
    {{4, 3}, pmDotEmpty}, /* gDotEmpty */
    {{4, 3}, pmDotFull}, /* gDotFull */
    {{5, 5}, pmLife}, /* gLife */
    {{5, 5}, pmMissileIcon}, /* gMissileIcon */
    {{5, 5}, pmBeamIcon}, /* gBeamIcon */
    {{5, 5}, pmWallIcon}, /* gWallIcon */
    {{3, 1}, pmShot}, /* gShot */
    {{5, 5}, pmExplosion[0]}, /* gExplosionA1 */
    {{5, 5}, pmExplosion[1]}, /* gExplosionA2 */
};

Object* DynamicObject[256] = {NULL};

void DrawObject(Uint8* PixelMap, Object obj, Vec2 pos) {
    
    /* Start and end coordinates */
    int x = pos.x;
    int ex = pos.x + obj.Size.x;
    int y = pos.y;
    int ey = pos.y + obj.Size.y;

    /* Variables used for scanning */
    int px, py;

    for (px = x; px < ex; px++) {
        for (py = y; py < ey; py++) {
            
            /* Only draw the pixels which are active */
            if (obj.Samples[(py - y) * obj.Size.x + px - x]) {
                if (px < 84 && py < 48 && px >= 0 && py >= 0) {
                    PixelMap[py * 84 + px] = 1;
                }
            }
        }
    }
}

/* Draws an object outlined by its inputs: pixel map, object, position */
void DrawOutlinedObject(Uint8* PixelMap, Object obj, Vec2 pos) {
    int x = pos.x, ex = pos.x + obj.Size.x, y = pos.y, ey = pos.y + obj.Size.y; /* Start and end coordinates */
    int px, py; /* Variables used for scanning */
    for (px = x; px < ex; px++) { /* Step One: Draw around with a background color to make what is already there by accident disappear */
        for (py = y; py < ey; py++) { /* Traverses the screen on the rectangle where the object will be drawn */
            if (obj.Samples[(py - y) * obj.Size.x + px - x]) { /* Only existing pixels should be drawn */
                if (px < 84 && py < 48 && px >= 0 && py >= 0) { /* Do not write beyond the screen */
                    Uint16 PixelPos = py * 84 + px; /* The location of the pixel to be drawn on the pixel map */
                    if (py) { /* Top line */
                        if (px) PixelMap[PixelPos - 85] = 0; /* Redraw the pixel on the top left, if it exists */
                        PixelMap[PixelPos - 84] = 0; /* Redraw the pixel above, if any */
                        if (px != 83) PixelMap[PixelPos - 83] = 0; /* Redraw the pixel on the top right, if one exists */
                    }
                    if (px) PixelMap[PixelPos - 1] = 0; /* Redraw the pixel to the left, if any */
                    if (px != 83) PixelMap[PixelPos + 1] = 0; /* Redraw the pixel to the right, if any */
                    if (py != 47) { /* Bottom line */
                        if (px) PixelMap[PixelPos + 85] = 0; /* Redraw the bottom left pixel, if any */
                        PixelMap[PixelPos + 84] = 0; /* Redraw the pixel below, if it exists */
                        if (px != 83) PixelMap[PixelPos + 83] = 0; /* Redraw the bottom right pixel, if any */
                    }
                }
            }
        }
    }
    DrawObject(PixelMap, obj, pos); /* Step Two: Real Drawing */
}

/* Draws a small number, inputs: pixel map, number, number of digits (fills the remainder with leading zeros), position of the last digit */
void DrawSmallNumber(Uint8* PixelMap, Uint16 Num, Uint8 Digits, Vec2 LastDigit) {
    while (Digits--) { /* The number of digits always comes from the input, there is no dynamic drawing in the original either */
        DrawObject(PixelMap, GetObject((Graphics)(Num % 10)), NewVec2(LastDigit.x, LastDigit.y)); /* Draw the last digit, every number in the enumeration is in its appropriate place */
        Num /= 10; /* Omitting the last digit for the next round */
        LastDigit.x -= 4; /* The function draws backwards starting from the last digit */
    }
}

/* Writes a text, inputs: pixel map, text, upper left pixel position, row height */
void DrawText(Uint8* PixelMap, const char* Text, Vec2 Pos, int LineHeight) {
    Sint16 PosX = Pos.x; /* Store left indent for line breaks as it will modify Pos for movement */
    while (*Text) { /* As long as the examined character is not a trailing zero */
        if (*Text == '\n') { /* If there is a line break, move the cursor to the beginning of the next line */
            Pos.x = PosX;
            Pos.y += LineHeight;
        } else {
            DrawObject(PixelMap, NewObject(NewVec2(5, 8), Font[(unsigned char)*Text]), Pos); /* Draw current character (cast removes an irrelevant warning) */
            Pos.x += 6; /* Jump to the next position in the line */
        }
        ++Text; /* Jump to the next character */
    }
}

/* Draws a scrollbar, the first input is the pixel map, the second input is the scrolling position in percentage */
void DrawScrollBar(Uint8* PixelMap, Uint8 Percent) {
    Uint16 i;
    Uint8 Row = Percent / 4 + 6; /* The column from which the scroll indicator is drawn */
    for (i = 6 * 84 + 81; i < 39 * 84 + 81; i += 84) /* Between rows 6 and 38 in column 81 per row */
        PixelMap[i] = 1; /* First, it colors the entire left side of the 3*32 strip */
    InvertScreenPart(PixelMap, NewVec2(81, Row + 1), NewVec2(81, Row + 5)); /* By inverting, it removes the drawn vertical line from the location of the marker */
    DrawObject(PixelMap, GetObject(gScrollMark), NewVec2(81, Percent / 4 + 6)); /* Draws the marker */
}

/* Inverting the entire screen, input is the pixel map */
void InvertScreen(Uint8* PixelMap) {
    int i;
    for (i = 0; i < 4032; ++i) /* Traverse and invert all 84x48 pixels */
        PixelMap[i] = 1 - PixelMap[i];
}

/* It inverts the part of the screen, its first input is the pixel map, followed by the initial and then the end coordinates */
void InvertScreenPart(Uint8* PixelMap, Vec2 From, Vec2 To) {
    Sint8 yFrom = From.y; /* The inner cycle must always be started from the beginning, so its initial value must be stored */
    for (; From.x <= To.x; ++From.x) /* Travel between start and finish on both axes */
        for (From.y = yFrom; From.y <= To.y; From.y++) {
            Uint8 *Pixel = PixelMap + From.y * 84 + From.x; /* The memory address of the given pixel */
            *Pixel = 1 - *Pixel;
        }
}

/* Extract compressed pixel map inside array
 *
 * Uint8* PixelMap: pixel map where compressed pixel map is stored.
 *                  The final pixel map it's store in this same array.
 * Uint16 Pixels: The actual number of pixels to draw on the screen.
 * Uint16 Bytes: The number of bytes in the compressed pixel map.
 */
void UncompressPixelMap(Uint8* PixelMap, Uint16 Pixels, Uint16 Bytes) {
    Uint8 Bits = Pixels % 8;

    if (Bits == 0)
        Bits = 8;
    
    while (Bytes--) {
        
        while (Bits--) {
            PixelMap[Bytes * 8 + Bits] = PixelMap[Bytes] % 2;
            if (Bytes != 0 || Bits != 0)
                PixelMap[Bytes] >>= 1;
        }

        Bits = 8;
    }
}

/* Extract a compressed object
 *
 * Uint8* compressed: compressed array
 * int CompressedSize: number of elements in compressed array
 * Uint8* Container: pixel map to store the uncompressed array
 * int ContainerSize: size of pixel map
 */
void UncompressObject(Uint8* Compressed, int CompressedSize, Uint8* Container, int ContainterSize) {
    memcpy(Container, Compressed, CompressedSize);
    UncompressPixelMap(Container, ContainterSize, CompressedSize);
}

/* Extract compressed objects */
void UncompressObjects() {
    UncompressObject(cmSpace, sizeof(cmSpace), pmSpace, sizeof(pmSpace));
    UncompressObject(cmIntro, sizeof(cmIntro), pmIntro, sizeof(pmIntro));
    UncompressObject(cmImpact, sizeof(cmImpact), pmImpact, sizeof(pmImpact));
}

/* Get object from ID */
Object GetObject(Uint16 ObjectID) {

    /* Object ID < 256 are hardcoded */
    if (ObjectID < 256) {
        return StaticObject[ObjectID];
    } else {
         
        /* Search in dynamic objects array */
        
        /* Get the object ID, modulo prevents possible overindexing */
        ObjectID %= 256;
        
        /* If the object is not already loaded, search for the file and load it */
        if (!DynamicObject[ObjectID]) {
            
            FILE* ObjectData;
            Uint8 Size[2];
            Uint8* NewPixelMap;
            Uint16 Pixels, Bytes;
            
            /* The complete path = folder + max three-digit ID + null character
             * Example: data/objects/1.dat
             */
            /* Init the string with the folder location */
            char Path[13 + 3  + 1] = "data/objects/";
            FillFileName(Path, ObjectID);
            ObjectData = fopen(Path, "rb");
            
            /* If failed to open file return an empty object */
            if (!ObjectData)
                return NewObject(NewVec2(0, 0), NULL);
            
            /* Read file's bytes and store them in the object */
            fread(Size, sizeof(Uint8), 2, ObjectData);
            Pixels = (Uint16)Size[0] * (Uint16)Size[1];
            Bytes = Pixels / 8 + (Pixels % 8 != 0);
            NewPixelMap = (Uint8*)malloc(Pixels);
            fread(NewPixelMap, sizeof(Uint8), Bytes, ObjectData);
            fclose(ObjectData);
            UncompressPixelMap(NewPixelMap, Pixels, Bytes);
            
            DynamicObject[ObjectID] = (Object*)malloc(sizeof(Object));
            DynamicObject[ObjectID]->Size.x = Size[0];
            DynamicObject[ObjectID]->Size.y = Size[1];
            DynamicObject[ObjectID]->Samples = NewPixelMap;
        }
        return *DynamicObject[ObjectID];
    }
    return NewObject(NewVec2(0, 0), NULL); /* If it wasn't dynamic, then nothing */
}

/* Release all dynamically allocated objects */
void FreeDynamicGraphics() {
    int i;
    for (i = 0; i < 256; ++i) { /* Walking down the block */
        if (DynamicObject[i]) { /* If it finds a loaded dynamic object */
            free(DynamicObject[i]->Samples); /* Free your pixel map first */
            free(DynamicObject[i]); /* Then the object itself */
        }
    }
}

Vec2 NewVec2(Sint16 x, Sint16 y) {
    Vec2 ret;
    ret.x = x;
    ret.y = y;
    return ret;
}

/* Returns a new object to avoid the pedantic "ISO C90 forbids compound literals" warning */
Object NewObject(Vec2 Size, Uint8* Samples) {
    Object ret;
    ret.Size = Size;
    ret.Samples = Samples;
    return ret;
}