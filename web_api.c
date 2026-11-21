
#include <stdio.h>
#include <curl/curl.h>
#include "web_api.h"

void odeslat_stav(const Mazlicek *mazlicek) {

    if (mazlicek == NULL) {
        printf("[API] Chyba: mazlicek neexistuje.\n");
        return;
    }

    if (!mazlicek->vytvoren) {
        printf("[API] Mazlicek jeste nebyl vytvoren.\n");
        return;
    }

    CURL *curl = curl_easy_init();

    if (curl == NULL) {
        printf("[API] Nepodarilo se inicializovat CURL.\n");
        return;
    }

    const char *druh;

    switch (mazlicek->druh) {
        case KRALIK: druh = "kralik"; break;
        case KOCKA:  druh = "kocka"; break;
        case RYBA:   druh = "ryba"; break;
        default:     druh = "neznamy"; break;
    }

    char *jmeno = curl_easy_escape(curl, mazlicek->jmeno, 0);

    if (jmeno == NULL) {
        curl_easy_cleanup(curl);
        return;
    }

    char url[1024];

    int delka = snprintf(
        url, sizeof(url),
        "http://localhost:3999/api/update"
        "?jmeno=%s&druh=%s"
        "&zdravi=%d&hlad=%d&unava=%d"
        "&stesti=%d&penize=%d&den=%d&tah=%d",
        jmeno,
        druh,
        mazlicek->zdravi,
        mazlicek->hlad,
        mazlicek->unava,
        mazlicek->stesti,
        mazlicek->penize,
        mazlicek->den,
        mazlicek->tah
    );

    curl_free(jmeno);

    if (delka < 0 || (size_t)delka >= sizeof(url)) {
        printf("[API] URL je prilis dlouha.\n");
        curl_easy_cleanup(curl);
        return;
    }

    printf("\n[API] Odesilam stav mazlicka...\n");

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);

    CURLcode result = curl_easy_perform(curl);

    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

    if (result != CURLE_OK) {
        printf("\n[API] Chyba: %s\n",
               curl_easy_strerror(result));
    }
    else if (http_code == 200) {
        printf("\n[API] Stav uspesne odeslan!\n");
    }
    else {
        printf("\n[API] Server vratil HTTP %ld\n", http_code);
    }

    curl_easy_cleanup(curl);
}
