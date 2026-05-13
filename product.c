/*
  product.c - Implementacija logike za artikle
  Ovaj modul sadrzi funkcije za "rast" memorije i manipulaciju podacima.
*/

#include "header.h"

/* PAMETNO UPRAVLJANJE MEMORIJOM 
   Ova funkcija automatski udvostrucuje kapacitet polja kada se popuni.
   Koristi realloc kako bi zadrzala postojece podatke na novoj lokaciji.
*/
static int grow_product_array(ProductArray* arr) {
    if (!arr) return -1;
    size_t   newcap = arr->capacity ? arr->capacity * 2 : 16;
    Product* tmp    = (Product*)realloc(arr->items, newcap * sizeof(Product));
    if (!tmp) return -1; // Provjera uspjesnosti alokacije
    arr->items    = tmp;
    arr->capacity = newcap;
    return 0;
}

// Inicijalizacija polja na sigurne pocetne vrijednosti
void product_init(ProductArray* arr) {
    if (!arr) return;
    arr->items    = NULL;
    arr->size     = 0;
    arr->capacity = 0;
}

// Dodavanje novog artikla u memoriju
int product_add(ProductArray* arr, const Product* p) {
    if (!arr || !p) return -1;
    // Ako je polje puno, povecaj ga
    if (arr->size >= arr->capacity) {
        if (grow_product_array(arr) != 0) return -2;
    }
    // Kopiranje podataka novog proizvoda na kraj polja
    arr->items[arr->size] = *p;
    arr->size++;
    return 0;
}

/* ALGORITMI SORTIRANJA 
   Koristimo ugradenu funkciju qsort() koja implementira QuickSort algoritam.
   Za to nam trebaju komparacijske funkcije koje vracaju -1, 0 ili 1.
*/
int product_compare_by_id(const void* a, const void* b) {
    const Product* pa = (const Product*)a;
    const Product* pb = (const Product*)b;
    return (pa->id - pb->id);
}

int product_compare_by_name(const void* a, const void* b) {
    const Product* pa = (const Product*)a;
    const Product* pb = (const Product*)b;
    return strcmp(pa->name, pb->name);
}

// Oslobadjanje dinamicki zauzete memorije
void product_free(ProductArray* arr) {
    if (!arr) return;
    free(arr->items); // Kljucni korak za sprjecavanje "memory leak-a"
    arr->items = NULL;
    arr->size = arr->capacity = 0;
}
