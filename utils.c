/*
  utils.c
  Pomocne funkcije za:
  - unos podataka
  - provjeru datoteka
  - konfiguraciju programa
*/

#include "header.h"   // Ucitavanje zajednickog header fajla

// Globalna konfiguracija programa
// verbose = 1 znaci da ce program ispisivati dodatne informacije
Config globalConfig = { .verbose = 1 };


// Funkcija za inicijalizaciju utility sustava
void init_utils(void) {

    // Postavljanje verbose moda
    globalConfig.verbose = 1;
}


// Funkcija za cleanup
// Trenutno ne radi nista ali ostavljena za buduce potrebe
void cleanup_utils(void) {

    /* placeholder */
}


// Funkcija za siguran unos integer vrijednosti
int read_int(const char* prompt) {

    // Buffer za unos teksta s tipkovnice
    char buf[128];

    // Varijabla u koju spremamo broj
    int val = 0;

    // Ispis poruke korisniku
    printf("%s", prompt);

    // fgets cita cijeli red teksta
    // stdin = tipkovnica
    // sizeof(buf) = maksimalna velicina buffera
    if (!fgets(buf, sizeof(buf), stdin))
        return 0;

    // sscanf pretvara string u integer
    // %d oznacava integer
    // &val = adresa gdje spremiti broj
    if (sscanf(buf, "%d", &val) != 1)
        return 0;

    // Vracanje unesenog broja
    return val;
}


// Funkcija za unos decimalnog broja
double read_double(const char* prompt) {

    // Buffer za tekstualni unos
    char buf[128];

    // Varijabla za decimalni broj
    double val = 0.0;

    // Ispis poruke
    printf("%s", prompt);

    // Citanje teksta
    if (!fgets(buf, sizeof(buf), stdin))
        return 0.0;

    // Pretvaranje stringa u double
    // %lf = format za double
    if (sscanf(buf, "%lf", &val) != 1)
        return 0.0;

    // Vracanje decimalnog broja
    return val;
}


// Funkcija za unos stringa
void read_string(const char* prompt, char* buf, size_t bufsize) {

    // Provjera postoji li buffer
    // i ima li mjesta za upis
    if (!buf || bufsize == 0)
        return;

    // Ispis poruke korisniku
    printf("%s", prompt);

    // fgets cita tekst iz tipkovnice
    if (!fgets(buf, (int)bufsize, stdin)) {

        // Ako unos nije uspio
        // postavi prazan string
        buf[0] = '\0';

        return;
    }

    // Duljina unesenog stringa
    size_t len = strlen(buf);

    // fgets sprema ENTER znak '\n'
    // pa ga ovdje uklanjamo
    if (len > 0 && buf[len - 1] == '\n')
        buf[len - 1] = '\0';
}


// Funkcija provjerava postoji li datoteka
int file_exists(const char* filename) {

    // Pokusaj otvaranja datoteke u binary read modu
    FILE* f = fopen(filename, "rb");

    // Ako je fopen uspio
    // datoteka postoji
    if (f) {

        // Zatvaranje datoteke
        fclose(f);

        return 1;
    }

    // Datoteka ne postoji
    return 0;
}
