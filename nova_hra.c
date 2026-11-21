
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nova_hra.h"
#include "web_api.h"

// Barvy terminalu
#define RESET  "\033[0m"
#define GREEN  "\033[1;32m"
#define CYAN   "\033[1;36m"
#define YELLOW "\033[1;33m"
#define PURPLE "\033[1;35m"
#define RED    "\033[1;31m"
#define WHITE  "\033[1;37m"
#define GRAY   "\033[90m"

#define SOUBOR "tamagotchi_save.txt"

// =====================================
// POMOCNE FUNKCE
// =====================================

static void smaz(void) {
    printf("\033[2J\033[H");
}

static void cekej(void) {
    puts("\n  Stiskni Enter...");
    getchar();
}

// Nacteni cisla od hrace
static int cislo(void) {
    char vstup[64];
    char navic;
    int n;

    printf(GREEN "  > " RESET);
    fflush(stdout);

    if (!fgets(vstup, sizeof(vstup), stdin))
        return -1;

    if (sscanf(vstup, " %d %c", &n, &navic) != 1)
        return -2;

    return n;
}

// Omezeni hodnoty na rozsah 0-100
static int omez(int hodnota) {

    if (hodnota < 0)
        return 0;

    if (hodnota > 100)
        return 100;

    return hodnota;
}

// Omezeni vsech statistik
static void omez_statistiky(Mazlicek *m) {

    m->zdravi = omez(m->zdravi);
    m->hlad = omez(m->hlad);
    m->unava = omez(m->unava);
    m->stesti = omez(m->stesti);

    if (m->penize < 0)
        m->penize = 0;
}

// =====================================
// DRUHY MAZLICKU
// =====================================

static const char *druh_text(DruhMazlicka druh) {

    switch (druh) {

        case KRALIK:
            return "Kralik";

        case KOCKA:
            return "Kocka";

        case RYBA:
            return "Ryba";

        default:
            return "Neznamy";
    }
}

// ASCII obrazky
static void obrazek(DruhMazlicka druh) {

    switch (druh) {

        case KRALIK:
        printf(PURPLE);
        printf("             ,\\\n");
        printf("             \\\\\\\,_\n");
        printf("              \\` ,\\\n");
        printf("         __,.-\" =__)\n");
        printf("       .\"        )\n");
        printf("    ,_/   ,    \\/\\_\n");
        printf("    \\_|    )_-\\ \\_-`\n");
        printf("       `-----` `--`\n");
        break;

    case KOCKA:
        printf(YELLOW);
        printf("                   _ |\\_\n");
        printf("                   \\` ..\\\n");
        printf("              __,.-\" =__Y=\n");
        printf("            .\"        )\n");
        printf("      _    /   ,    \\/\\_\n");
        printf("     ((____|    )_-\\ \\_-`\n");
        printf("     `-----'`-----` `--`\n");
        break;

    case RYBA:
        printf(CYAN);
        printf("                /`·.¸\n");
        printf("               /¸...¸`:·\n");
        printf("           ¸.·´  ¸   `·.¸.·´)\n");
        printf("          : © ):´;      ¸  {\n");
        printf("           `·.¸ `·  ¸.·´\\`·¸)\n");
        printf("               `\\\\´´\\¸.·´\n");
        break;
    }
}

// =====================================
// STATISTIKY
// =====================================

// Barevny ukazatel statistiky
static void bar(
    const char *nazev,
    int hodnota,
    const char *barva
) {

    int plne = hodnota / 5;

    printf("  %-9s [", nazev);

    for (int i = 0; i < 20; i++) {

        if (i < plne) {
            printf("%s#", barva);
        }
        else {
            printf(GRAY ".");
        }
    }

    printf(RESET "] %3d/100\n", hodnota);
}

// Zobrazeni mazlicka a statistik
static void stav(Mazlicek *m) {

    smaz();

    printf(CYAN);
    printf("  +------------------------------------------+\n");
    printf("  |" WHITE "              TVUJ MAZLICEK                " CYAN "|\n");
    printf("  +------------------------------------------+\n");
    printf(RESET);

    printf(
        "\n  %s (%s)   |   Den %d, tah %d\n",
        m->jmeno,
        druh_text(m->druh),
        m->den,
        m->tah
    );

    obrazek(m->druh);

    if (m->nemoc_tahu > 0) {
        printf(
            RED "                 [ NEMOCNY: %d tahu ]\n" RESET,
            m->nemoc_tahu
        );
    }
    else {
        printf(GREEN "                    [ SPOKOJENY ]\n" RESET);
    }

    printf("\n");

    bar("Zdravi", m->zdravi, GREEN);
    bar("Hlad", m->hlad, YELLOW);
    bar("Unava", m->unava, CYAN);
    bar("Stesti", m->stesti, PURPLE);

    printf(YELLOW "\n  Penize: %d Kc\n" RESET, m->penize);

    printf(CYAN "\n  +------------------------------------------+\n" RESET);

    printf("  [1] Prace      [2] Jidlo\n");
    printf("  [3] Spanek     [4] Zabava\n");
    printf("  [5] Ulozit     [6] Nacist\n");
    printf("  [0] Hlavni menu\n");

    printf(CYAN "  +------------------------------------------+\n" RESET);
}

// =====================================
// UKLADANI HRY
// =====================================

static int uloz(const Mazlicek *m) {

    FILE *f = fopen(SOUBOR, "w");

    if (!f)
        return 0;

    int ok = fprintf(
        f,
        "TAMAGOTCHI1\n%s\n%d %d %d %d %d %d %d %d %d %d\n",
        m->jmeno,
        (int)m->druh,
        m->zdravi,
        m->hlad,
        m->unava,
        m->stesti,
        m->penize,
        m->den,
        m->tah,
        m->nemoc_tahu,
        (int)m->faze
    ) > 0;

    if (fclose(f) != 0)
        ok = 0;

    return ok;
}

// =====================================
// NACITANI HRY
// =====================================

static int nacti(Mazlicek *m) {

    FILE *f = fopen(SOUBOR, "r");

    if (!f)
        return 0;

    Mazlicek docasny = {0};

    char hlavicka[32];
    char jmeno[50];
    char dalsi;

    int druh;
    int faze;

    int ok =
        fgets(hlavicka, sizeof(hlavicka), f) &&
        strcmp(hlavicka, "TAMAGOTCHI1\n") == 0 &&
        fgets(jmeno, sizeof(jmeno), f) &&
        strchr(jmeno, '\n') != NULL;

    if (ok) {

        jmeno[strcspn(jmeno, "\n")] = '\0';

        ok =
            jmeno[0] != '\0' &&

            fscanf(
                f,
                "%d %d %d %d %d %d %d %d %d %d %c",
                &druh,
                &docasny.zdravi,
                &docasny.hlad,
                &docasny.unava,
                &docasny.stesti,
                &docasny.penize,
                &docasny.den,
                &docasny.tah,
                &docasny.nemoc_tahu,
                &faze,
                &dalsi
            ) == 10 &&

            druh >= KRALIK &&
            druh <= RYBA &&

            faze >= BEZNY_STAV &&
            faze <= KONEC_HRY &&

            docasny.zdravi >= 0 &&
            docasny.zdravi <= 100 &&

            docasny.hlad >= 0 &&
            docasny.hlad <= 100 &&

            docasny.unava >= 0 &&
            docasny.unava <= 100 &&

            docasny.stesti >= 0 &&
            docasny.stesti <= 100 &&

            docasny.penize >= 0 &&
            docasny.den >= 1 &&
            docasny.tah >= 1 &&
            docasny.nemoc_tahu >= 0;
    }

    fclose(f);

    if (!ok)
        return 0;

    strcpy(docasny.jmeno, jmeno);

    docasny.druh = (DruhMazlicka)druh;
    docasny.faze = (Faze)faze;
    docasny.vytvoren = 1;

    *m = docasny;

    return 1;
}

// =====================================
// VYTVORENI MAZLICKA
// =====================================

static int zaloz(Mazlicek *m) {

    int vyber;

    do {

        smaz();

        printf(CYAN);
        printf("  +------------------------------------------+\n");
        printf("  |" WHITE "            VYTVORENI MAZLICKA            " CYAN "|\n");
        printf("  +------------------------------------------+\n");
        printf(RESET);

        printf("\n");
        printf("  [1] Kralik\n");
        printf("  [2] Kocka\n");
        printf("  [3] Ryba\n");
        printf("  [0] Zpet\n");

        vyber = cislo();

        if (vyber == -1 || vyber == 0)
            return 0;

    } while (vyber < 1 || vyber > 3);

    char jmeno[50];

    smaz();

    printf(PURPLE "  JMENO MAZLICKA\n" RESET);
    printf("  Jak se bude mazlicek jmenovat?\n");

    while (1) {

        printf(GREEN "  > " RESET);
        fflush(stdout);

        if (!fgets(jmeno, sizeof(jmeno), stdin))
            return 0;

        char *konec = strchr(jmeno, '\n');

        if (!konec) {

            int c;

            while ((c = getchar()) != '\n' && c != EOF) {
            }

            printf(RED "  Jmeno je prilis dlouhe (max. 49 znaku).\n" RESET);

            continue;
        }

        *konec = '\0';

        if (jmeno[0] != '\0')
            break;

        printf(RED "  Jmeno nesmi byt prazdne.\n" RESET);
    }

    // Inicializace noveho mazlicka
    *m = (Mazlicek){0};

    strcpy(m->jmeno, jmeno);

    m->druh = (DruhMazlicka)vyber;
    m->vytvoren = 1;

    m->zdravi = 100;
    m->hlad = 20;
    m->unava = 10;
    m->stesti = 80;
    m->penize = 100;

    m->den = 1;
    m->tah = 1;

    m->faze = BEZNY_STAV;

    return 1;
}

// =====================================
// NAHODNE UDALOSTI
// =====================================

// Seznam vsech moznych udalosti
static const Udalost udalosti[] = {

    {
        "Nasel jsi 25 Kc!",
        0, 0, 0, 5, 25, 0
    },

    {
        "Necekany vydaj: -20 Kc.",
        0, 0, 0, -5, -20, 0
    },

    {
        "Mazlicek chytil nemoc.",
        -8, 5, 5, -10, 0, 3
    },

    {
        "Krasny den zvedl naladu!",
        0, 0, 0, 15, 0, 0
    },

    {
        "Mazlicek snedl svacinu.",
        0, -15, 0, 0, 0, 0
    },

    {
        "Spatna noc: vice unavy.",
        0, 0, 15, -5, 0, 0
    }
};

// Vygenerovani nahodne udalosti
static void nahodna_udalost(Mazlicek *m) {

    // 30% sance na udalost
    if (rand() % 100 >= 30)
        return;

    int pocet = sizeof(udalosti) / sizeof(udalosti[0]);

    const Udalost *u = &udalosti[rand() % pocet];

    // Uplatneni dopadu udalosti
    m->zdravi += u->zdravi;
    m->hlad += u->hlad;
    m->unava += u->unava;
    m->stesti += u->stesti;
    m->penize += u->penize;

    if (u->nemoc_tahu > 0)
        m->nemoc_tahu = u->nemoc_tahu;

    omez_statistiky(m);

    printf(
        YELLOW "\n  NAHODNA UDALOST: %s\n" RESET,
        u->popis
    );

    cekej();
}

// =====================================
// NASLEDKY SPATNYCH STATISTIK
// =====================================

static void dusledky(Mazlicek *m) {

    // Prilis vysoky hlad
    if (m->hlad >= 80)
        m->zdravi -= 12;

    // Prilis vysoka unava
    if (m->unava >= 80)
        m->zdravi -= 12;

    // Nizke stesti
    if (m->stesti <= 20)
        m->zdravi -= 5;

    // Nemoc
    if (m->nemoc_tahu > 0) {

        m->zdravi -= 8;
        m->nemoc_tahu--;
    }

    omez_statistiky(m);

    // Kontrola smrti a nemoci
    if (m->zdravi == 0) {
        m->faze = KONEC_HRY;
    }
    else if (m->nemoc_tahu > 0) {
        m->faze = NEMOC;
    }
    else {
        m->faze = BEZNY_STAV;
    }
}

// =====================================
// CAS A TAHY
// =====================================

static void dalsi_tah(Mazlicek *m) {

    m->tah++;

    // Kazdych 5 tahu novy den
    if ((m->tah - 1) % 5 == 0)
        m->den++;

    // Nahodna udalost na zacatku tahu
    nahodna_udalost(m);
}

// =====================================
// HERNI AKCE
// =====================================

static int akce(Mazlicek *m, int volba) {

    switch (volba) {

        // PRACE
        case 1:

            m->penize += 35;
            m->hlad += 18;
            m->unava += 22;
            m->stesti -= 8;

            printf("\n  Pracoval jsi a vydelal 35 Kc.\n");

            break;

        // JIDLO
        case 2:

            if (m->penize < 30) {

                printf(RED "\n  Nemas 30 Kc na jidlo!\n" RESET);

                return 0;
            }

            m->penize -= 30;
            m->hlad -= 35;
            m->zdravi += 4;

            printf("\n  Mazlicek se najedl.\n");

            break;

        // SPANEK
        case 3:

            m->faze = SPANEK;

            m->unava -= 45;
            m->hlad += 12;
            m->zdravi += 5;

            printf("\n  Mazlicek se vyspal.\n");

            break;

        // ZABAVA
        case 4:

            if (m->penize < 20) {

                printf(RED "\n  Nemas 20 Kc na zabavu!\n" RESET);

                return 0;
            }

            m->penize -= 20;
            m->stesti += 25;
            m->unava += 10;
            m->hlad += 8;

            printf("\n  Mazlicek si uzil zabavu.\n");

            break;

        default:
            return 0;
    }

    // Omezeni hodnot na 0-100
    omez_statistiky(m);

    return 1;
}

// =====================================
// HLAVNI FUNKCE HRY
// =====================================

void nova_hra(Mazlicek *m) {

    // Pokud jeste neexistuje mazlicek
    if (!m->vytvoren) {

        smaz();

        printf(CYAN);
        printf("  +------------------------------------------+\n");
        printf("  |                NOVA HRA                  |\n");
        printf("  +------------------------------------------+\n");
        printf(RESET);

        printf("\n  [1] Vytvorit mazlicka\n");
        printf("  [2] Nacist ulozenou hru\n");
        printf("  [0] Zpet do menu\n");

        int volba = cislo();

        if (volba == 1) {

            if (!zaloz(m))
                return;
        }
        else if (volba == 2) {

            if (!nacti(m)) {

                printf(RED);
                printf("  Ulozena hra neexistuje nebo je poskozena.\n");
                printf(RESET);

                cekej();
                return;
            }
        }
        else {
            return;
        }
    }

    // Hlavni herni smycka
    while (1) {

        odeslat_stav(m);

        stav(m);

        // Konec hry
        if (m->faze == KONEC_HRY || m->zdravi == 0) {

            printf(RED);
            printf("\n  KONEC HRY! Mazlicek prisel o vsechno zdravi.\n");
            printf(RESET);

            printf("  [1] Zalozit noveho mazlicka\n");
            printf("  [0] Do menu\n");

            int volba = cislo();

            if (volba == 1) {

                if (!zaloz(m))
                    return;

                continue;
            }

            return;
        }

        // Vyber akce
        int volba = cislo();

        if (volba == -1 || volba == 0)
            return;

        // Ulozeni hry
        if (volba == 5) {

            if (uloz(m)) {
                printf(GREEN "\n  Hra byla ulozena.\n" RESET);
            }
            else {
                printf(RED "\n  Ulozeni selhalo.\n" RESET);
            }

            cekej();
        }

        // Nacteni hry
        else if (volba == 6) {

            if (nacti(m)) {
                printf(GREEN "\n  Hra byla nactena.\n" RESET);
            }
            else {
                printf(RED "\n  Nacteni selhalo.\n" RESET);
            }

            cekej();
        }

        // Prace, jidlo, spanek a zabava
        else if (volba >= 1 && volba <= 4) {

            if (akce(m, volba)) {

                // Zdravotni nasledky akce
                dusledky(m);

                if (m->faze != KONEC_HRY) {

                    printf(GRAY "  Nasleduje dalsi tah...\n" RESET);

                    cekej();
                    dalsi_tah(m);
                }
                else {
                    cekej();
                }
            }
            else {
                cekej();
            }
        }
    }
}
