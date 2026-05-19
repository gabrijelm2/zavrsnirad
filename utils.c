/*
  utils.c
  Pomocne funkcije za:
  - unos podataka
  - provjeru datoteka
  - konfiguraciju programa

  // 7 - Organizacija izvornog koda: utils.c je odvojen modul za pomocne funkcije
*/

#include "header.h"   // Ucitavanje zajednickog header fajla

// 6 - static globalna varijabla NIJE koristena ovdje (globalConfig nije static jer mora biti extern)
// 8 - extern: globalConfig je deklariran kao extern u headeru, a DEFINIRAN ovdje u utils.c
// 3 - Slozeni tip Config za globalnu konfiguraciju programa
Config globalConfig = { .verbose = 1 };


// 13 - Funkcija: init_utils inicijalizira utility sustav
// 9 - Ova funkcija je jednostavna (moze se razmatrati kao kandidat za inline)
void init_utils(void) {

    // Postavljanje verbose moda
    globalConfig.verbose = 1;
}


// 13 - Funkcija: cleanup_utils - placeholder za buduce ciscenje resursa
// 9 - Jednostavna/prazna funkcija; kandidat za inline ili makro u buducnosti
void cleanup_utils(void) {

    /* placeholder */
}


// 13 - Funkcija: read_int za siguran unos integera s tipkovnice
// 14 - Zastita parametara: prompt je const char* - ne moze se slucajno mijenjati
int read_int(const char* prompt) {

    // 15 - Staticko polje fiksne velicine umjesto VLA
    char buf[128];

    // 2 - Primitivni tip int za pohranu unesene vrijednosti
    int val = 0;

    // Ispis poruke korisniku
    printf("%s", prompt);

    // 19 - Datoteke/stdin: fgets cita iz standardnog ulaza, provjera povratne vrijednosti
    if (!fgets(buf, sizeof(buf), stdin))
        return 0;

    // 12 - Pokazivaci nisu direktno potrebni ovdje; sscanf koristi adresu &val
    if (sscanf(buf, "%d", &val) != 1)
        return 0;

    // Vracanje unesenog broja
    return val;
}


// 13 - Funkcija: read_double za unos realnog broja
// 14 - Zastita: prompt je const, provjera povratne vrijednosti fgets-a
double read_double(const char* prompt) {

    // 15 - Staticko polje fiksne velicine
    char buf[128];

    // 2 - double: odabrani primitivni tip za realne vrijednosti
    double val = 0.0;

    printf("%s", prompt);

    // 19 - stdin: fgets s provjerom pointera
    if (!fgets(buf, sizeof(buf), stdin))
        return 0.0;

    // sscanf pretvara string u double
    if (sscanf(buf, "%lf", &val) != 1)
        return 0.0;

    return val;
}


// 13 - Funkcija: read_string za siguran unos stringa
// 14 - Zastita parametara: provjera buf != NULL i bufsize > 0 na pocetku
void read_string(const char* prompt, char* buf, size_t bufsize) {

    // 14 - Zastita: provjera da buffer postoji i ima mjesta
    if (!buf || bufsize == 0)
        return;

    printf("%s", prompt);

    // 19 - stdin: fgets cita sigurno do bufsize znakova
    if (!fgets(buf, (int)bufsize, stdin)) {

        // Ako unos nije uspio postavi prazan string
        buf[0] = '\0';
        return;
    }

    // 2 - size_t: odabrani primitivni tip za duljinu stringa
    size_t len = strlen(buf);

    // Uklanjanje '\n' koji fgets sprema
    if (len > 0 && buf[len - 1] == '\n')
        buf[len - 1] = '\0';
}


// 13 - Funkcija: file_exists provjerava postoji li datoteka
// 19 - Datoteke: fopen s provjerom pointera i zatvaranje fclose
int file_exists(const char* filename) {

    // 19 - Provjera pokazivaca FILE*: ako fopen vrati NULL, datoteka ne postoji
    FILE* f = fopen(filename, "rb");

    if (f) {

        // 19 - Zatvaranje datoteke nakon provjere
        fclose(f);
        return 1;
    }

    return 0;
}
