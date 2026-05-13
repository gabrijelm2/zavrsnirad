/*
  utils.c
  Pomocne funkcije: unos podataka, provjera datoteke, konfiguracija
*/

#include "header.h"

Config globalConfig = { .verbose = 1 };

void init_utils(void) {
    globalConfig.verbose = 1;
}

void cleanup_utils(void) {
    /* placeholder */
}

int read_int(const char* prompt) {
    char buf[128];
    int  val = 0;
    printf("%s", prompt);
    if (!fgets(buf, sizeof(buf), stdin)) return 0;
    if (sscanf(buf, "%d", &val) != 1) return 0;
    return val;
}

double read_double(const char* prompt) {
    char   buf[128];
    double val = 0.0;
    printf("%s", prompt);
    if (!fgets(buf, sizeof(buf), stdin)) return 0.0;
    if (sscanf(buf, "%lf", &val) != 1) return 0.0;
    return val;
}

void read_string(const char* prompt, char* buf, size_t bufsize) {
    if (!buf || bufsize == 0) return;
    printf("%s", prompt);
    if (!fgets(buf, (int)bufsize, stdin)) { buf[0] = '\0'; return; }
    size_t len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n') buf[len - 1] = '\0';
}

int file_exists(const char* filename) {
    FILE* f = fopen(filename, "rb");
    if (f) { fclose(f); return 1; }
    return 0;
}
