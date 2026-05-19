/*
  product.c
  CRUD operacije i pomocne funkcije za proizvode

  // 7 - Organizacija izvornog koda: product.c je odvojen modul za sve operacije nad proizvodima
  // 1 - CRUID: ovaj fajl implementira Create (add), Read (find), Update, InsertDelete za proizvode
*/

#include "header.h"


// ==========================================
// Povecavanje memorije niza proizvoda
// ==========================================

// 6 - static: grow_product_array vidljiva samo unutar product.c (interna helper funkcija)
// 13 - Funkcija: jedan posao - povecava dinamicki niz
// 14 - Zastita: provjera !arr
static int grow_product_array(ProductArray* arr) {

    // 14 - Zastita parametara: provjera pokazivaca
    if (!arr)
        return -1;

    // 2 - size_t: primitivni tip za kapacitet
    size_t newcap =
        arr->capacity ? arr->capacity * 2 : 16;

    // 17 - realloc: dinamicko prosirenje memorije
    // 16 - Dinamicko zauzimanje memorije za slozeni tip Product
    Product* tmp =
        (Product*)realloc(
            arr->items,
            newcap * sizeof(Product)
        );

    // 18 - Provjera pokazivaca: realloc moze vratiti NULL ako nema memorije
    if (!tmp)
        return -1;

    // 18 - Spremi novu adresu (stara je prosirena ili premjestena)
    arr->items = tmp;
    arr->capacity = newcap;

    return 0;
}


// ==========================================
// Inicijalizacija niza proizvoda
// ==========================================

// 13 - Funkcija: inicijalizira ProductArray na sigurno pocetno stanje
// 14 - Zastita: provjera !arr
void product_init(ProductArray* arr) {

    if (!arr)
        return;

    // 12 - Pokazivac: inicijalno NULL - memorija jos nije alocirana
    arr->items = NULL;
    arr->size = 0;
    arr->capacity = 0;
}


// ==========================================
// Oslobadjanje memorije
// ==========================================

// 13 - Funkcija: oslobadja sve dinamicki zauzete resurse
// 17 - free(): koristena za oslobadjanje memorije
// 18 - Sigurno brisanje: free pa anuliranje pokazivaca
void product_free(ProductArray* arr) {

    if (!arr)
        return;

    // 17 - free: oslobadja dinamicku memoriju
    free(arr->items);

    // 18 - Anuliranje pokazivaca: sprecava double-free i dangling pointer
    arr->items = NULL;
    arr->size = 0;
    arr->capacity = 0;
}


// ==========================================
// Provjera postoji li ID proizvoda
// ==========================================

// 13 - Pomocna funkcija: linearna pretraga
// 14 - Zastita: const parametar, provjera !arr
int product_id_exists(const ProductArray* arr,
    int id) {

    if (!arr)
        return 0;

    for (size_t i = 0; i < arr->size; ++i) {

        if (arr->items[i].id == id)
            return 1;
    }

    return 0;
}


// ==========================================
// Provjera postoji li naziv proizvoda
// ==========================================

// 14 - Zastita parametara: provjera !arr i !name
int product_name_exists(const ProductArray* arr,
    const char* name) {

    if (!arr || !name)
        return 0;

    for (size_t i = 0; i < arr->size; ++i) {

        if (strcmp(arr->items[i].name, name) == 0)
            return 1;
    }

    return 0;
}


// ==========================================
// Dodavanje proizvoda
// ==========================================

// 1 - CRUID - Create: dodavanje novog proizvoda
// 13 - Funkcija: jedan posao - validacija + dodavanje u niz
// 14 - Zastita parametara: provjera !arr i !p
// 12 - Pokazivac: p je const Product* - citamo bez mijenjanja originala
int product_add(ProductArray* arr,
    const Product* p) {

    if (!arr || !p)
        return -1;

    // 14 - Provjera duplikata ID-a
    if (product_id_exists(arr, p->id))
        return -2;

    // 14 - Provjera duplikata naziva
    if (product_name_exists(arr, p->name))
        return -3;

    if (arr->size >= arr->capacity) {

        // 17 - realloc: unutar grow_product_array
        if (grow_product_array(arr) != 0)
            return -1;
    }

    arr->items[arr->size++] = *p;

    return 0;
}


// ==========================================
// Pretraga proizvoda po ID-u
// ==========================================

// 1 - CRUID - Read: pretraga proizvoda
// 12 - Vraca pokazivac na element u nizu (direktan pristup, ne kopija)
// 13 - Funkcija: jedan posao - pronadji proizvod
// 14 - Zastita: provjera !arr
Product* product_find_by_id(ProductArray* arr,
    int id) {

    if (!arr)
        return NULL;

    for (size_t i = 0; i < arr->size; ++i) {

        if (arr->items[i].id == id)

            // 12 - Vraca adresu elementa u nizu
            return &arr->items[i];
    }

    return NULL;
}


// ==========================================
// Azuriranje proizvoda
// ==========================================

// 1 - CRUID - Update: azuriranje podataka
// 13 - Funkcija: jedan posao - azurira tocno jedan zapis
// 14 - Zastita: provjera !arr i !p
int product_update(ProductArray* arr,
    int id,
    const Product* p) {

    if (!arr || !p)
        return -1;

    // 12 - Pokazivac: found ukazuje direktno na element u nizu
    Product* found =
        product_find_by_id(arr, id);

    if (!found)
        return -1;

    // Provjera konfliktnog ID-a kod promjene
    if (p->id != id &&
        product_id_exists(arr, p->id))
        return -2;

    // Kopiranje strukture kroz dereferencirani pokazivac
    *found = *p;

    return 0;
}


// ==========================================
// Brisanje proizvoda
// ==========================================

// 1 - CRUID - InsertDelete: brisanje proizvoda iz niza
// 13 - Funkcija: brise element i kompaktira niz
// 14 - Zastita: provjera !arr
int product_delete(ProductArray* arr,
    int id) {

    if (!arr)
        return -1;

    size_t idx = arr->size;

    for (size_t i = 0; i < arr->size; ++i) {

        if (arr->items[i].id == id) {
            idx = i;
            break;
        }
    }

    if (idx == arr->size)
        return -1;

    // Zadnji element ide na mjesto obrisanog (O(1) brisanje)
    if (idx != arr->size - 1)
        arr->items[idx] =
        arr->items[arr->size - 1];

    arr->size--;

    return 0;
}


// ==========================================
// Ispis jednog proizvoda
// ==========================================

// 13 - Funkcija: jedan posao - formatiran ispis jednog produkta
// 14 - Zastita: provjera !p
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

        // 3 - Poziv pomocne funkcije za pretvaranje enum ID-a u naziv kategorije
        category_get_name(p->categoryId),

        p->price,
        p->quantity,
        p->supplierId
    );
}


// ==========================================
// Ispis svih proizvoda
// ==========================================

// 13 - Funkcija: iterira kroz niz i ispisuje sve elemente
// 14 - Zastita: provjera !arr
void product_print_all(const ProductArray* arr) {

    if (!arr)
        return;

    if (arr->size == 0) {
        printf("Nema proizvoda.\n");
        return;
    }

    for (size_t i = 0; i < arr->size; ++i) {
        product_print(&arr->items[i]);
    }
}


// ==========================================
// Generiranje testnih proizvoda
// ==========================================

// 13 - Funkcija: generira testne podatke za proizvode
// 14 - Zastita: provjera !arr
int product_generate_test_data(ProductArray* arr,
    size_t n) {

    if (!arr)
        return -1;

    int startId = 1;

    for (size_t i = 0; i < arr->size; ++i) {
        if (arr->items[i].id >= startId)
            startId = arr->items[i].id + 1;
    }

    for (size_t i = 0; i < n; ++i) {

        // 3 - Slozeni tip: lokalna Product struktura inicijalizirana na 0
        Product p = { 0 };

        p.id = startId + (int)i;

        snprintf(
            p.name,
            sizeof(p.name),
            "Proizvod_%d",
            p.id
        );

        // 11 - Enum: CAT_COUNT koristen za modulo da se kategorije vrte u krug
        p.categoryId =
            (int)(i % CAT_COUNT);

        // 2 - double: realna vrijednost cijene
        p.price =
            3.5 + (double)(i % 200) * 0.25;

        // 2 - int: cjelobrojna kolicina
        p.quantity =
            (int)(5 + (i % 100));

        p.supplierId =
            (int)((i % 50) + 1);

        int rc = product_add(arr, &p);

        if (rc == -2 || rc == -3)
            continue;
        else if (rc != 0)
            return -1;
    }

    return 0;
}


// ==========================================
// Comparator za sortiranje po ID-u
// ==========================================

// 23 - Sortiranje: comparator za qsort
// 26 - Pokazivac na funkciju: ova funkcija se prosljedjuje qsortu kao argument
// 12 - Pokazivaci: void* argumenti pretvoreni u konkretne tipove
int product_compare_by_id(const void* a,
    const void* b) {

    const Product* pa = (const Product*)a;
    const Product* pb = (const Product*)b;

    return (pa->id - pb->id);
}


// ==========================================
// Comparator za sortiranje po nazivu
// ==========================================

// 23 - Sortiranje: comparator za qsort (abecedno po nazivu)
// 26 - Pokazivac na funkciju: prosljedjuje se qsortu
int product_compare_by_name(const void* a,
    const void* b) {

    const Product* pa = (const Product*)a;
    const Product* pb = (const Product*)b;

    // strcmp za leksikografsko sortiranje stringova
    return strcmp(pa->name, pb->name);
}


// ==========================================
// Uklanjanje duplikata proizvoda
// ==========================================

// 13 - Funkcija: uklanja duplikate - sortiranje pa linearni prolazak
// 23 - Sortiranje: koristi qsort
// 26 - Pokazivac na funkciju: product_compare_by_id prosljedjen qsortu
void product_remove_duplicates(ProductArray* arr) {

    if (!arr || arr->size <= 1)
        return;

    // 23 - qsort: standardna bibliotecna funkcija
    // 26 - Pokazivac na funkciju: product_compare_by_id kao callback
    qsort(
        arr->items,
        arr->size,
        sizeof(Product),
        product_compare_by_id
    );

    size_t write = 0;

    for (size_t read = 0;
        read < arr->size;
        ++read) {

        if (write == 0 ||
            arr->items[read].id !=
            arr->items[write - 1].id) {

            arr->items[write++] =
                arr->items[read];
        }
        else {

            // 8 - extern: pristup globalnoj varijabli definiranoj u utils.c
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

    arr->size = write;
}

// ==========================================
// Rekurzivni binarni search
// ==========================================

// 24 - Pretraživanje: implementacija binarnog pretrazivanja
// 25 - Rekurzija: funkcija poziva samu sebe (rekurzivni algoritam pretrazivanja)
// 13 - Funkcija: jedan posao - binarno pretrazivanje sortiranog niza po ID-u
int product_binary_search_recursive(const Product* items, int low, int high, int targetId) {

    // Bazni slucaj: raspon je prazan - element nije pronaden
    if (low > high) return -1;

    // 2 - int: primitivni tip za sredisnji indeks
    int mid = low + (high - low) / 2;

    // Element pronaden
    if (items[mid].id == targetId) return mid;

    if (items[mid].id > targetId) {

        // 25 - Rekurzivni poziv: trazi u lijevoj polovici
        return product_binary_search_recursive(items, low, mid - 1, targetId);
    }

    // 25 - Rekurzivni poziv: trazi u desnoj polovici
    return product_binary_search_recursive(items, mid + 1, high, targetId);
}
