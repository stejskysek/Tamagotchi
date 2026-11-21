
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "web_api.h"
#include "mazlicek.h"
#include "nova_hra.h"
#include "napoveda.h"
#include "zmena_jmena.h"

// Barvy terminalu
#define RESET "\033[0m"
#define GREEN "\033[1;32m"
#define CYAN "\033[1;36m"
#define YELLOW "\033[1;33m"
#define PURPLE "\033[1;35m"
#define RED "\033[1;31m"
#define WHITE "\033[1;37m"
#define GRAY "\033[90m"

// Nacteni cisla od uzivatele
static int nacti_cislo(void)
{
    char radek[64];
    char navic;
    int n;

    if (!fgets(radek, sizeof(radek), stdin))
        return -1;

    if (sscanf(radek, " %d %c", &n, &navic) != 1)
        return -2;

    return n;
}

// Hlavni menu
static void zobraz_menu(void)
{

    // Vymazani terminalu
    printf("\033[2J\033[H");
    // Menu
    printf(GREEN "\n       [1] " WHITE "Nova hra / Pokracovat\n");
    printf(YELLOW "       [2] " WHITE "Napoveda\n");
    printf(PURPLE "       [3] " WHITE "Zmena jmena\n");
    printf(RED "       [4] " WHITE "Ukonceni programu\n");
    printf(RESET);

    printf(CYAN "\n  +------------------------------------------+\n" RESET);
    printf(GRAY "\n       Vyber moznost (1-4)\n" RESET);
    printf(GREEN "       > " RESET);

    fflush(stdout);
}

int main(void)
{

    Mazlicek mazlicek = {0};

    // Inicializace nahodnych cisel
    srand((unsigned)time(NULL));

    while (1)
    {

        zobraz_menu();

        int volba = nacti_cislo();

        if (volba == -1)
            break;

        switch (volba)
        {

        case 1:
            printf("\n[API] Spoustim hru...\n");

            nova_hra(&mazlicek);

            printf("\n[API] Navrat do hlavniho menu.\n");

            odeslat_stav(&mazlicek);
            break;

        case 2:
            napoveda();
            break;

        case 3:
            zmena_jmena(&mazlicek);
            break;

        case 4:
            printf("\033[2J\033[H");
            printf(GREEN "Diky za hrani! Ahoj!\n" RESET);
            return 0;

        default:
            break;
        }
    }

    return 0;
}
