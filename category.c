/*
  category.c - Logika za kategorije ribolovne opreme
*/

#include "header.h"

// Pretvaranje numericke vrijednosti enuma u citljivi tekst
const char* category_get_name(int categoryId) {
    switch (categoryId) {
        case CAT_STAPOVI: return "Stapovi";
        case CAT_ROLE:    return "Role";
        case CAT_NAJLON:  return "Najlon";
        case CAT_UDICE:   return "Udice";
        case CAT_HRANA:   return "Hrana";
        default:          return "Nepoznata";
    }
}

// Provjera je li unesena kategorija unutar dozvoljenih granica
int is_valid_category(int categoryId) {
    return (categoryId >= 0 && categoryId < CAT_COUNT);
}
