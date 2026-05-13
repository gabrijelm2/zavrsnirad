/*
  product.c
  CRUID operacije i pomocne funkcije za proizvode
*/

#include "header.h"

static int grow_product_array(ProductArray* arr) {
    if (!arr) return -1;
    size_t   newcap = arr->capacity ? arr->capacity * 2 : 16;
    Product* tmp    = (Product*)realloc(arr->items, newcap * sizeof(Product));
    if (!tmp) return -1;
    arr->items    = tmp;
    arr->capacity = newcap;
    return 0;
}

void product_init(ProductArray* arr) {
    if (!arr) return;
    arr->items    = NULL;
    arr->size     = 0;
    arr->capacity = 0;
}

void product_free(ProductArray* arr) {
    if (!arr) return;
    free(arr->items);
    arr->items    = NULL;
    arr->size     = 0;
    arr->capacity = 0;
}

int product_id_exists(const ProductArray* arr, int id) {
    if (!arr) return 0;
    for (size_t i = 0; i < arr->size; ++i) {
        if (arr->items[i].id == id) return 1;
    }
    return 0;
}

int product_name_exists(const ProductArray* arr, const char* name) {
    if (!arr || !name) return 0;
    for (size_t i = 0; i < arr->size; ++i) {
        if (strcmp(arr->items[i].name, name) == 0) return 1;
    }
    return 0;
}

int product_add(ProductArray* arr, const Product* p) {
    if (!arr || !p) return -1;
    if (product_id_exists(arr, p->id))     return -2;
    if (product_name_exists(arr, p->name)) return -3;
    if (arr->size >= arr->capacity) {
        if (grow_product_array(arr) != 0) return -1;
    }
    arr->items[arr->size++] = *p;
    return 0;
}

Product* product_find_by_id(ProductArray* arr, int id) {
    if (!arr) return NULL;
    for (size_t i = 0; i < arr->size; ++i) {
        if (arr->items[i].id == id) return &arr->items[i];
    }
    return NULL;
}

int product_update(ProductArray* arr, int id, const Product* p) {
    if (!arr || !p) return -1;
    Product* found = product_find_by_id(arr, id);
    if (!found) return -1;
    if (p->id != id && product_id_exists(arr, p->id)) return -2;
    *found = *p;
    return 0;
}

int product_delete(ProductArray* arr, int id) {
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

void product_print(const Product* p) {
    if (!p) return;
    printf("ID:%d | Naziv:%s | Kategorija:%s | Cijena:%.2f | Kolicina:%d | DobavljacID:%d\n",
        p->id, p->name, category_get_name(p->categoryId),
        p->price, p->quantity, p->supplierId);
}

void product_print_all(const ProductArray* arr) {
    if (!arr) return;
    if (arr->size == 0) { printf("Nema proizvoda.\n"); return; }
    for (size_t i = 0; i < arr->size; ++i) {
        product_print(&arr->items[i]);
    }
}

int product_generate_test_data(ProductArray* arr, size_t n) {
    if (!arr) return -1;
    int startId = 1;
    for (size_t i = 0; i < arr->size; ++i) {
        if (arr->items[i].id >= startId) startId = arr->items[i].id + 1;
    }
    for (size_t i = 0; i < n; ++i) {
        Product p    = { 0 };
        p.id         = startId + (int)i;
        snprintf(p.name, sizeof(p.name), "Proizvod_%d", p.id);
        p.categoryId = (int)(i % CAT_COUNT);
        p.price      = 3.5 + (double)(i % 200) * 0.25;
        p.quantity   = (int)(5 + (i % 100));
        p.supplierId = (int)((i % 50) + 1);
        int rc = product_add(arr, &p);
        if (rc == -2 || rc == -3) continue;
        else if (rc != 0) return -1;
    }
    return 0;
}

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

void product_remove_duplicates(ProductArray* arr) {
    if (!arr || arr->size <= 1) return;
    qsort(arr->items, arr->size, sizeof(Product), product_compare_by_id);
    size_t write = 0;
    for (size_t read = 0; read < arr->size; ++read) {
        if (write == 0 || arr->items[read].id != arr->items[write - 1].id) {
            arr->items[write++] = arr->items[read];
        } else {
            if (globalConfig.verbose) {
                printf("Upozorenje: pronaden duplikat proizvoda s ID=%d; preskacem duplikat.\n",
                    arr->items[read].id);
            }
        }
    }
    arr->size = write;
}
