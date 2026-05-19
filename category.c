/*
  category.c
  Upravljanje kategorijama proizvoda
*/

#include "header.h"


// ==========================================
// Pretvaranje category ID u tekst
// ==========================================

const char* category_get_name(int categoryId) {

    // switch bira tekst prema ID-u kategorije
    switch (categoryId) {

    case CAT_STAPOVI:
        return "Stapovi";

    case CAT_ROLE:
        return "Role";

    case CAT_NAJLON:
        return "Najlon";

    case CAT_UDICE:
        return "Udice";

    case CAT_HRANA:
        return "Hrana";

        // Ako ID nije poznat
    default:
        return "Nepoznata";
    }
}


// ==========================================
// Provjera validnosti kategorije
// ==========================================

int is_valid_category(int categoryId) {

    // Kategorija je validna ako je u rasponu
    return (categoryId >= 0 &&
        categoryId < CAT_COUNT);
}


// ==========================================
// Ispis svih kategorija
// ==========================================

void print_all_categories(void) {

    printf("\nDostupne kategorije:\n");

    // prolazak kroz sve kategorije
    for (int i = 0; i < CAT_COUNT; ++i) {

        printf(
            "%d. %s\n",
            i,
            category_get_name(i)
        );
    }
}