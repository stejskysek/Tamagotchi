
#include <stdio.h>
#include <string.h>
#include "zmena_jmena.h"

// Barvy terminalu
#define RESET   "\033[0m"
#define GREEN   "\033[1;32m"
#define CYAN    "\033[1;36m"
#define PURPLE  "\033[1;35m"
#define RED     "\033[1;31m"
#define WHITE   "\033[1;37m"
#define YELLOW  "\033[1;33m"

void zmena_jmena(Mazlicek *mazlicek) {

    // Vymazani terminalu
    printf("\033[2J\033[H");

    printf(PURPLE);
    printf("  +------------------------------------------+\n");
    printf("  |               ZMENA JMENA               |\n");
    printf("  +------------------------------------------+\n");
    printf(RESET);

    // Kontrola, jestli mazlicek existuje
    if (mazlicek->vytvoren == 0) {

        printf(RED "\n  Zatim nemas zadneho mazlicka!\n" RESET);
        printf(YELLOW "  Nejprve si ho vytvor v Nove hre.\n" RESET);

        printf("\n  Stiskni Enter pro navrat do menu...");
        getchar();
        return;
    }

    // Zobrazeni aktualniho jmena
    printf("\n  Aktualni jmeno: ");
    printf(GREEN "%s\n" RESET, mazlicek->jmeno);

    char nove_jmeno[50];

    printf("\n  Zadej nove jmeno mazlicka:\n");
    printf(GREEN "  > " RESET);
    fflush(stdout);

    // Nacteni noveho jmena
    if (!fgets(nove_jmeno, sizeof(nove_jmeno), stdin)) {
        return;
    }

    // Odstraneni Enteru
    nove_jmeno[strcspn(nove_jmeno, "\n")] = '\0';

    // Kontrola prazdneho jmena
    if (nove_jmeno[0] == '\0') {
        printf(RED "\n  Jmeno nesmi byt prazdne!\n" RESET);
    }
    else {
        // Ulozeni noveho jmena
        strcpy(mazlicek->jmeno, nove_jmeno);

        printf(GREEN "\n  Jmeno bylo uspesne zmeneno!\n" RESET);

        printf(WHITE "  Nove jmeno: ");
        printf(CYAN "%s\n" RESET, mazlicek->jmeno);
    }

    printf("\n  Stiskni Enter pro navrat do menu...");
    getchar();
}
