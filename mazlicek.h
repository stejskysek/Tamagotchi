
#ifndef MAZLICEK_H
#define MAZLICEK_H

// Druhy mazlicku
typedef enum {
    KRALIK = 1,
    KOCKA = 2,
    RYBA = 3
} DruhMazlicka;

// Faze hry
typedef enum {
    BEZNY_STAV,
    NEMOC,
    SPANEK,
    KONEC_HRY
} Faze;

// Hlavni struktura mazlicka
typedef struct {
    char jmeno[50];
    DruhMazlicka druh;
    int vytvoren;

    // Statistiky
    int zdravi;
    int hlad;
    int unava;
    int stesti;
    int penize;

    // Herni cas
    int den;
    int tah;

    // Nemoc
    int nemoc_tahu;

    Faze faze;
} Mazlicek;

// Nahodne udalosti
typedef struct {
    const char *popis;

    int zdravi;
    int hlad;
    int unava;
    int stesti;
    int penize;

    int nemoc_tahu;
} Udalost;

#endif
