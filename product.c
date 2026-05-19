/*
  product.c
  CRUD operacije i pomocne funkcije za proizvode
*/

#include "header.h"


// ==========================================
// Povecavanje memorije niza proizvoda
// ==========================================

static int grow_product_array(ProductArray* arr) {

    // Provjera pointera
    if (!arr)
        return -1;

    // Ako postoji capacity:
    // capacity * 2
    // inace pocetni kapacitet = 16
    size_t newcap =
        arr->capacity ? arr->capacity * 2 : 16;

    // realloc povecava dinamicku memoriju
    Product* tmp =
        (Product*)realloc(
            arr->items,
            newcap * sizeof(Product)
        );

    // Ako realloc nije uspio
    if (!tmp)
        return -1;

    // Spremi novu adresu memorije
    arr->items = tmp;

    // Novi kapacitet
    arr->capacity = newcap;

    return 0;
}


// ==========================================
// Inicijalizacija niza proizvoda
// ==========================================

void product_init(ProductArray* arr) {

    if (!arr)
        return;

    // Nema memorije
    arr->items = NULL;

    // Trenutni broj elemenata
    arr->size = 0;

    // Kapacitet memorije
    arr->capacity = 0;
}


// ==========================================
// Oslobadjanje memorije
// ==========================================

void product_free(ProductArray* arr) {

    if (!arr)
        return;

    // Oslobodi dinamicku memoriju
    free(arr->items);

    // Reset pointera
    arr->items = NULL;

    // Reset velicine
    arr->size = 0;

    // Reset kapaciteta
    arr->capacity = 0;
}


// ==========================================
// Provjera postoji li ID proizvoda
// ==========================================

int product_id_exists(const ProductArray* arr,
    int id) {

    if (!arr)
        return 0;

    // Linearna pretraga
    for (size_t i = 0; i < arr->size; ++i) {

        if (arr->items[i].id == id)
            return 1;
    }

    return 0;
}


// ==========================================
// Provjera postoji li naziv proizvoda
// ==========================================

int product_name_exists(const ProductArray* arr,
    const char* name) {

    if (!arr || !name)
        return 0;

    // Prolazak kroz niz
    for (size_t i = 0; i < arr->size; ++i) {

        // strcmp vraca 0 ako su stringovi isti
        if (strcmp(arr->items[i].name, name) == 0)
            return 1;
    }

    return 0;
}


// ==========================================
// Dodavanje proizvoda
// ==========================================

int product_add(ProductArray* arr,
    const Product* p) {

    if (!arr || !p)
        return -1;

    // Provjera ID-a
    if (product_id_exists(arr, p->id))
        return -2;

    // Provjera naziva
    if (product_name_exists(arr, p->name))
        return -3;

    // Ako nema mjesta u memoriji
    if (arr->size >= arr->capacity) {

        // Povecaj memoriju
        if (grow_product_array(arr) != 0)
            return -1;
    }

    // Dodavanje elementa na kraj niza
    arr->items[arr->size++] = *p;

    return 0;
}


// ==========================================
// Pretraga proizvoda po ID-u
// ==========================================

Product* product_find_by_id(ProductArray* arr,
    int id) {

    if (!arr)
        return NULL;

    // Linearna pretraga
    for (size_t i = 0; i < arr->size; ++i) {

        if (arr->items[i].id == id)

            // Vrati adresu proizvoda
            return &arr->items[i];
    }

    return NULL;
}


// ==========================================
// Azuriranje proizvoda
// ==========================================

int product_update(ProductArray* arr,
    int id,
    const Product* p) {

    if (!arr || !p)
        return -1;

    // Pronadji proizvod
    Product* found =
        product_find_by_id(arr, id);

    // Ako ne postoji
    if (!found)
        return -1;

    // Ako se ID mijenja
    // provjeri postoji li novi ID
    if (p->id != id &&
        product_id_exists(arr, p->id))
        return -2;

    // Kopiranje cijele strukture
    *found = *p;

    return 0;
}


// ==========================================
// Brisanje proizvoda
// ==========================================

int product_delete(ProductArray* arr,
    int id) {

    if (!arr)
        return -1;

    // Pozicija elementa za brisanje
    size_t idx = arr->size;

    // Trazenje elementa
    for (size_t i = 0; i < arr->size; ++i) {

        if (arr->items[i].id == id) {

            idx = i;
            break;
        }
    }

    // Ako nije pronadjen
    if (idx == arr->size)
        return -1;

    // Ako nije zadnji element
    if (idx != arr->size - 1)

        // Zadnji element ide na mjesto obrisanog
        arr->items[idx] =
        arr->items[arr->size - 1];

    // Smanji broj elemenata
    arr->size--;

    return 0;
}


// ==========================================
// Ispis jednog proizvoda
// ==========================================

void product_print(const Product* p) {

    if (!p)
        return;

    printf(
        "ID:%d | Naziv:%s | "
        "Kategorija:%s | "
        "Cijena:%.2f | "
        "Kolicina:%d | "
        "DobavljacID:%d\n",

        p->id,
        p->name,

        // Pretvaranje category ID-a u tekst
        category_get_name(p->categoryId),

        p->price,
        p->quantity,
        p->supplierId
    );
}


// ==========================================
// Ispis svih proizvoda
// ==========================================

void product_print_all(const ProductArray* arr) {

    if (!arr)
        return;

    // Ako nema proizvoda
    if (arr->size == 0) {

        printf("Nema proizvoda.\n");
        return;
    }

    // Ispis svih proizvoda
    for (size_t i = 0; i < arr->size; ++i) {

        product_print(&arr->items[i]);
    }
}


// ==========================================
// Generiranje testnih proizvoda
// ==========================================

int product_generate_test_data(ProductArray* arr,
    size_t n) {

    if (!arr)
        return -1;

    // Pocetni ID
    int startId = 1;

    // Pronadji najveci postojeci ID
    for (size_t i = 0; i < arr->size; ++i) {

        if (arr->items[i].id >= startId)
            startId = arr->items[i].id + 1;
    }

    // Generiranje podataka
    for (size_t i = 0; i < n; ++i) {

        Product p = { 0 };

        // Generiranje ID-a
        p.id = startId + (int)i;

        // Generiranje naziva
        snprintf(
            p.name,
            sizeof(p.name),
            "Proizvod_%d",
            p.id
        );

        // Kategorije se vrte u krug:
        // 0 1 2 3 4 0 1 2 ...
        p.categoryId =
            (int)(i % CAT_COUNT);

        // Generiranje cijene
        p.price =
            3.5 + (double)(i % 200) * 0.25;

        // Generiranje kolicine
        p.quantity =
            (int)(5 + (i % 100));

        // Generiranje supplier ID-a
        p.supplierId =
            (int)((i % 50) + 1);

        // Dodavanje proizvoda
        int rc = product_add(arr, &p);

        // Preskoci duplikate
        if (rc == -2 || rc == -3)
            continue;

        // Ostale greske
        else if (rc != 0)
            return -1;
    }

    return 0;
}


// ==========================================
// Comparator za sortiranje po ID-u
// ==========================================

int product_compare_by_id(const void* a,
    const void* b) {

    // Pretvaranje void* u Product*
    const Product* pa = (const Product*)a;
    const Product* pb = (const Product*)b;

    // Razlika ID-eva
    return (pa->id - pb->id);
}


// ==========================================
// Comparator za sortiranje po nazivu
// ==========================================

int product_compare_by_name(const void* a,
    const void* b) {

    // Pretvaranje pointera
    const Product* pa = (const Product*)a;
    const Product* pb = (const Product*)b;

    // strcmp sortira stringove abecedno
    return strcmp(pa->name, pb->name);
}


// ==========================================
// Uklanjanje duplikata proizvoda
// ==========================================

void product_remove_duplicates(ProductArray* arr) {

    if (!arr || arr->size <= 1)
        return;

    // Sortiranje po ID-u
    qsort(
        arr->items,
        arr->size,
        sizeof(Product),
        product_compare_by_id
    );

    // write = pozicija zapisivanja
    size_t write = 0;

    // read = citanje elemenata
    for (size_t read = 0;
        read < arr->size;
        ++read) {

        // Ako nije duplikat
        if (write == 0 ||
            arr->items[read].id !=
            arr->items[write - 1].id) {

            // Kopiranje elementa
            arr->items[write++] =
                arr->items[read];
        }
        else {

            // Verbose upozorenje
            if (globalConfig.verbose) {

                printf(
                    "Upozorenje: pronaden "
                    "duplikat proizvoda "
                    "s ID=%d; "
                    "preskacem duplikat.\n",

                    arr->items[read].id
                );
            }
        }
    }

    // Novi broj elemenata
    arr->size = write;
}

// Rekurzivni binarni search (Pravilo 24 i 25)
int product_binary_search_recursive(const Product* items, int low, int high, int targetId) {
    if (low > high) return -1; // Bazni slucaj: nije pronaden

    int mid = low + (high - low) / 2;

    if (items[mid].id == targetId) return mid; // Pronaden!

    if (items[mid].id > targetId) {
        return product_binary_search_recursive(items, low, mid - 1, targetId); // Trazi lijevo
    }
    return product_binary_search_recursive(items, mid + 1, high, targetId); // Trazi desno
}
