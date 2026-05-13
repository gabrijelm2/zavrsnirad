/*
  category.c
  Upravljanje kategorijama proizvoda
*/

#include "header.h"

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

int is_valid_category(int categoryId) {
    return (categoryId >= 0 && categoryId < CAT_COUNT);
}

void print_all_categories(void) {
    printf("\nDostupne kategorije:\n");
    for (int i = 0; i < CAT_COUNT; ++i) {
        printf("%d. %s\n", i, category_get_name(i));
    }
}
