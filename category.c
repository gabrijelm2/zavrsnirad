/*
  category.c
  Upravljanje kategorijama proizvoda
 
  // 4 - typedef enum: ProductCategory enum (definiran u headeru) koristi se ovdje
*/

#include "header.h"


// ==========================================
// Pretvaranje category ID u tekst
// ==========================================

// 13 - Funkcija: jedan posao - mapiranje enum vrijednosti na string naziv
// 14 - Zastita: switch s default granom za nepoznate vrijednosti
// 11 - Enum: case vrijednosti su CAT_STAPOVI, CAT_ROLE itd. (enum, ne goli integeri)
// 4 - typedef enum: koristi se ProductCategory enum definiran s typedef u headeru
const char* category_get_name(int categoryId) {

    // switch bira tekst prema ID-u kategorije
    switch (categoryId) {

        // 11 - Enum: koriste se simbolicka enum imena umjesto magicnih brojeva
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

        // 14 - Zastita: default grana za nepoznate/neispravne ID-eve
    default:
        return "Nepoznata";
    }
}


// ==========================================
// Provjera validnosti kategorije
// ==========================================

// 13 - Funkcija: jedan posao - validacija raspona kategorije
// 9 - Jednostavna funkcija: kandidat za inline (jedna linija logike)
// 11 - Enum: CAT_COUNT koristen kao gornja granica (definiran u enumu, ne hardkodiran)
int is_valid_category(int categoryId) {

    // 11 - Enum: CAT_COUNT je posljednji clan enuma, predstavlja ukupan broj kategorija
    // 2 - int: categoryId je primitivni cjelobrojni tip
    return (categoryId >= 0 &&
        categoryId < CAT_COUNT);
}


// ==========================================
// Ispis svih kategorija
// ==========================================

// 13 - Funkcija: jedan posao - ispis svih dostupnih kategorija
// 11 - Enum: CAT_COUNT koristen za gornju granicu petlje
void print_all_categories(void) {

    printf("\nDostupne kategorije:\n");

    // 11 - Enum: petlja iterira do CAT_COUNT (ukupan broj kategorija iz enuma)
    for (int i = 0; i < CAT_COUNT; ++i) {

        printf(
            "%d. %s\n",
            i,
            // 13 - Poziv pomocne funkcije za naziv kategorije
            category_get_name(i)
        );
    }
}