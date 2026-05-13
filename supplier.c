/*
  supplier.c - Modul za dobavljace
*/

#include "header.h"

// Provjera postoji li vec dobavljac s istim ID-om
// Koristi se kod dodavanja kako bismo osigurali integritet podataka
int supplier_id_exists(const SupplierArray* arr, int id) {
    if (!arr) return 0;
    for (size_t i = 0; i < arr->size; ++i) {
        if (arr->items[i].id == id) return 1;
    }
    return 0;
}

// Brisanje dobavljaca iz dinamickog polja
int supplier_delete(SupplierArray* arr, int id) {
    for (size_t i = 0; i < arr->size; ++i) {
        if (arr->items[i].id == id) {
            /* Algoritam: prepisujemo element koji brisemo zadnjim elementom 
               u polju i smanjujemo velicinu. Brzo i efikasno. */
            arr->items[i] = arr->items[arr->size - 1];
            arr->size--;
            return 0;
        }
    }
    return -1;
}

// Funkcija za ispis jednog dobavljaca s formatiranim izlazom
void supplier_print(const Supplier* s) {
    if (!s) return;
    printf("ID: %-4d | Naziv: %-20s | Kontakt: %s\n", s->id, s->name, s->contact);
}
