/*
  utils.c - Pomocne funkcije za cijeli sustav
*/

#include "header.h"

// Globalna konfiguracija sustava
Config globalConfig = { .verbose = 1 };

// Siguran unos cijelog broja
// Koristi fgets + sscanf kako bi izbjegao probleme s 'ostacima' u bufferu tipkovnice
int read_int(const char* prompt) {
    char buf[128];
    int  val = 0;
    printf("%s", prompt);
    if (!fgets(buf, sizeof(buf), stdin)) return 0;
    if (sscanf(buf, "%d", &val) != 1) return 0; // Provjera je li korisnik stvarno unio broj
    return val;
}

// Siguran unos stringa s uklanjanjem oznake za novi red (\n)
void read_string(const char* prompt, char* buf, size_t bufsize) {
    if (!buf || bufsize == 0) return;
    printf("%s", prompt);
    if (!fgets(buf, (int)bufsize, stdin)) { buf[0] = '\0'; return; }
    
    // fgets ostavlja '\n' na kraju, ovdje ga brisemo radi ljepseg ispisa i spremanja
    size_t len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n') buf[len - 1] = '\0';
}

// Provjera postojanja datoteke na disku
int file_exists(const char* path) {
    FILE* f = fopen(path, "r");
    if (f) { fclose(f); return 1; }
    return 0;
}
