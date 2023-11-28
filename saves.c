#include "saves.h"

/* Reads the saved level if it has been saved */
void ReadSavedLevel(Uint8 *Level) {
    FILE *f = fopen("saved_level.txt", "rt"); /* Open file for reading */
    if (f) { /* If the file can be opened, then try to read it */
        fscanf(f, "%c", Level); /* Reads a single byte in the file */
        fclose(f); /* Close file */
    }
}

/* Reads the saved best scores into the array received in the parameter if they have been saved */
void ReadTopScore(unsigned int *Arr) {
    FILE* f = fopen("top_score.txt", "rt"); /* Fájl megnyitása olvasásra */
    if (f) { /* If the file can be opened, then try to read it */
        unsigned int* End = Arr + 10;
        while (Arr != End) /* It reads the 10 values one by one */
            fscanf(f, "%u", Arr++);
        fclose(f); /* Close file */
    }
}

/* Saves the level number received as input */
void SaveLevel(Uint8 Level) {
    FILE *f = fopen("saved_level.txt", "wt"); /* Open file for writing */
    if (f) { /* If the file can be opened, try writing to it */
        fprintf(f, "%c", Level); /* Write it as bytes */
        fclose(f); /* Close file */
    }
}

/* In the 10-element array received as input, it inserts the element received in the second parameter so that it remains in descending order, and then writes it to a file */
void PlaceTopScore(unsigned int *Arr, Uint16 Entry) {
    FILE* f = fopen("top_score.txt", "wt"); /* Open file for writing */
    unsigned int *Start = Arr, *End = Arr + 10;
    while (Arr != End) { /* The first element of the array is to search for elements that are smaller than the new one */
        if (*Arr < Entry) {
            int j;
            for (j = 9; j >= Arr - Start; j--) /* Moving the rest of the block forward */
                Start[j] = Start[j - 1];
            *Arr = Entry; /* The new value can go to the place created in this way */
            Arr = End; /* Exiting the cycle */
        } else
            ++Arr; /* If there is not enough back to insert the new point, continue */
    }
    /* Rescue */
    if (f) { /* Ha megnyílt a fájl, akkor próbál írni bele */
        for (Arr = Start; Arr != End; ++Arr) /* Writes the value 10 by one */
            fprintf(f, "%u ", *Arr);
        fclose(f); /* Close file */
    }
}

/* Append a filename to a path by identifier */
void FillFileName(char* Path, Uint16 FileID) {
    char Number[6]; /* As text, the identifier, 100000, just won't be there */
    // itoa(FileID, Number, 10); /* Creating text from numbers */
    sprintf(Number, "%d", FileID);
    strcat(Path, Number); /* Append filename to path */
    strcat(Path, ".dat"); /* Append an extension to the path */
}
