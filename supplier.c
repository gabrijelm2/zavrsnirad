/*
  supplier.c
  CRUID operacije i pomocne funkcije za dobavljace
*/

#include "header.h"

static int grow_supplier_array(SupplierArray* arr) {
    if (!arr) return -1;
    size_t    newcap = arr->capacity ? arr->capacity * 2 : 8;
    Supplier* tmp    = (Supplier*)realloc(arr->items, newcap * sizeof(Supplier));
    if (!tmp) return -1;
    arr->items    = tmp;
    arr->capacity = newcap;
    return 0;
}

void supplier_init(SupplierArray* arr) {
    if (!arr) return;
    arr->items    = NULL;
    arr->size     = 0;
    arr->capacity = 0;
}

void supplier_free(SupplierArray* arr) {
    if (!arr) return;
    free(arr->items);
    arr->items    = NULL;
    arr->size     = 0;
    arr->capacity = 0;
}

int supplier_id_exists(const SupplierArray* arr, int id) {
    if (!arr) return 0;
    for (size_t i = 0; i < arr->size; ++i) {
        if (arr->items[i].id == id) return 1;
    }
    return 0;
}

int supplier_name_exists(const SupplierArray* arr, const char* name) {
    if (!arr || !name) return 0;
    for (size_t i = 0; i < arr->size; ++i) {
        if (strcmp(arr->items[i].name, name) == 0) return 1;
    }
    return 0;
}

int supplier_add(SupplierArray* arr, const Supplier* s) {
    if (!arr || !s) return -1;
    if (supplier_id_exists(arr, s->id))     return -2;
    if (supplier_name_exists(arr, s->name)) return -3;
    if (arr->size >= arr->capacity) {
        if (grow_supplier_array(arr) != 0) return -1;
    }
    arr->items[arr->size++] = *s;
    return 0;
}

Supplier* supplier_find_by_id(SupplierArray* arr, int id) {
    if (!arr) return NULL;
    for (size_t i = 0; i < arr->size; ++i) {
        if (arr->items[i].id == id) return &arr->items[i];
    }
    return NULL;
}

int supplier_update(SupplierArray* arr, int id, const Supplier* s) {
    if (!arr || !s) return -1;
    Supplier* found = supplier_find_by_id(arr, id);
    if (!found) return -1;
    if (s->id != id && supplier_id_exists(arr, s->id)) return -2;
    *found = *s;
    return 0;
}

int supplier_delete(SupplierArray* arr, int id) {
    if (!arr) return -1;
    size_t idx = arr->size;
    for (size_t i = 0; i < arr->size; ++i) {
        if (arr->items[i].id == id) { idx = i; break; }
    }
    if (idx == arr->size) return -1;
    if (idx != arr->size - 1) arr->items[idx] = arr->items[arr->size - 1];
    arr->size--;
    return 0;
}

void supplier_print(const Supplier* s) {
    if (!s) return;
    printf("ID:%d | Naziv:%s | Kontakt:%s\n", s->id, s->name, s->contact);
}

void supplier_print_all(const SupplierArray* arr) {
    if (!arr) return;
    if (arr->size == 0) { printf("Nema dobavljaca.\n"); return; }
    for (size_t i = 0; i < arr->size; ++i) {
        supplier_print(&arr->items[i]);
    }
}

int supplier_generate_test_data(SupplierArray* arr, size_t n) {
    if (!arr) return -1;
    int startId = 1;
    for (size_t i = 0; i < arr->size; ++i) {
        if (arr->items[i].id >= startId) startId = arr->items[i].id + 1;
    }
    for (size_t i = 0; i < n; ++i) {
        Supplier s = { 0 };
        s.id = startId + (int)i;
        snprintf(s.name,    sizeof(s.name),    "Dobavljac_%d",          s.id);
        snprintf(s.contact, sizeof(s.contact), "kontakt%d@primjer.com", s.id);
        int rc = supplier_add(arr, &s);
        if (rc == -2 || rc == -3) continue;
        else if (rc != 0) return -1;
    }
    return 0;
}

int supplier_compare_by_id(const void* a, const void* b) {
    const Supplier* sa = (const Supplier*)a;
    const Supplier* sb = (const Supplier*)b;
    return (sa->id - sb->id);
}

void supplier_remove_duplicates(SupplierArray* arr) {
    if (!arr || arr->size <= 1) return;
    qsort(arr->items, arr->size, sizeof(Supplier), supplier_compare_by_id);
    size_t write = 0;
    for (size_t read = 0; read < arr->size; ++read) {
        if (write == 0 || arr->items[read].id != arr->items[write - 1].id) {
            arr->items[write++] = arr->items[read];
        } else {
            if (globalConfig.verbose) {
                printf("Upozorenje: pronaden duplikat dobavljaca s ID=%d; preskacem duplikat.\n",
                    arr->items[read].id);
            }
        }
    }
    arr->size = write;
}
