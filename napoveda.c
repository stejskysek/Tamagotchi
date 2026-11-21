
#include <stdio.h>
#include "napoveda.h"

void napoveda(void) {
    printf("\033[2J\033[H");

    printf("\033[1;33m");
    printf("  +------------------------------------------+\n");
    printf("  |                 NAPOVEDA                 |\n");
    printf("  +------------------------------------------+\n");
    printf("\033[0m");

    printf("\n  Jak hrat Tamagotchi?\n\n");

    printf("  [1] Pracuj a vydelavej penize.\n");
    printf("  [2] Jez, abys nemel hlad.\n");
    printf("  [3] Spi, abys nebyl unaveny.\n");
    printf("  [4] Bav se a zvysuj stesti.\n");

    printf("\n  Udrzuj sveho mazlicka nazivu!\n");

    printf("\n  Stiskni Enter pro navrat do menu...");
    getchar();
}
