/*
  supplier.c
  CRUD operacije i pomocne funkcije za dobavljace

  // 1 - CRUID: ovaj fajl implementira Create (add), Read (find), Update, InsertDelete za dobavljace
*/

#include "header.h"


// ==========================================
// Funkcija za povecavanje kapaciteta niza
// ==========================================

// 6 - static lokalna funkcija: grow_supplier_array vidljiva samo unutar supplier.c
// 13 - Funkcija radi tocno jedan posao: povecava kapacitet dinamickog niza
// 14 - Zastita parametara: provjera !arr na pocetku
static int grow_supplier_array(SupplierArray* arr) {

    // 14 - Zastita: provjera pokazivaca
    if (!arr)
        return -1;

    // 2 - size_t: primitivni tip za kapacitet
    size_t newcap = arr->capacity ? arr->capacity * 2 : 8;

    // 17 - realloc: povecava dinamicki alociranu memoriju
    // 16 - Dinamicko zauzimanje memorije za slozeni tip Supplier
    Supplier* tmp =
        (Supplier*)realloc(arr->items, newcap * sizeof(Supplier));

    // 18 - Provjera pokazivaca: ako realloc ne uspije, stara memorija ostaje netaknuta
    if (!tmp)
        return -1;

    // 18 - Anuliranje: nova adresa se sprema; stara je ili prosirena ili preseli na novu lokaciju
    arr->items = tmp;
    arr->capacity = newcap;

    return 0;
}


// ==========================================
// Inicijalizacija niza dobavljaca
// ==========================================

// 13 - Funkcija: inicijalizira SupplierArray na sigurno pocetno stanje
// 14 - Zastita: provjera !arr
void supplier_init(SupplierArray* arr) {

    if (!arr)
        return;

    // 12 - Pokazivac postavljen na NULL (nije jos alociran)
    arr->items = NULL;
    arr->size = 0;
    arr->capacity = 0;
}


// ==========================================
// Oslobadjanje memorije
// ==========================================

// 13 - Funkcija: oslobadja dinamicki alociranu memoriju
// 18 - Sigurno brisanje: free -> NULL -> reset velicine i kapaciteta
// 17 - free(): koristena za oslobadjanje memorije
void supplier_free(SupplierArray* arr) {

    if (!arr)
        return;

    // 17 - free: oslobadjanje dinamicke memorije
    free(arr->items);

    // 18 - Anuliranje pokazivaca nakon free (sprecava double-free i dangling pointer)
    arr->items = NULL;
    arr->size = 0;
    arr->capacity = 0;
}


// ==========================================
// Provjera postoji li ID
// ==========================================

// 13 - Pomocna funkcija: linearna pretraga po ID-u
// 14 - Zastita: provjera !arr
int supplier_id_exists(const SupplierArray* arr, int id) {

    if (!arr)
        return 0;

    for (size_t i = 0; i < arr->size; ++i) {

        if (arr->items[i].id == id)
            return 1;
    }

    return 0;
}


// ==========================================
// Provjera postoji li naziv
// ==========================================

// 14 - Zastita parametara: provjera !arr i !name
int supplier_name_exists(const SupplierArray* arr, const char* name) {

    if (!arr || !name)
        return 0;

    for (size_t i = 0; i < arr->size; ++i) {

        if (strcmp(arr->items[i].name, name) == 0)
            return 1;
    }

    return 0;
}


// ==========================================
// Dodavanje dobavljaca
// ==========================================

// 1 - CRUID - Create: dodavanje novog dobavljaca
// 13 - Funkcija: product_add radi jedan posao - dodaje element u dinamicki niz
// 14 - Zastita parametara: provjera !arr i !s
// 12 - Pokazivac: s je const Supplier* - citamo podatke bez mijenjanja originala
int supplier_add(SupplierArray* arr, const Supplier* s) {

    if (!arr || !s)
        return -1;

    // 14 - Provjera duplikata
    if (supplier_id_exists(arr, s->id))
        return -2;

    if (supplier_name_exists(arr, s->name))
        return -3;

    // Ako je niz pun - povecaj memoriju
    if (arr->size >= arr->capacity) {

        // 17 - realloc unutar grow_supplier_array
        if (grow_supplier_array(arr) != 0)
            return -1;
    }

    // Dodavanje elementa
    arr->items[arr->size++] = *s;

    return 0;
}


// ==========================================
// Pretraga po ID-u
// ==========================================

// 1 - CRUID - Read: pretraga dobavljaca
// 12 - Vraca pokazivac na element u nizu (ne kopiju)
// 13 - Funkcija: jedan posao - pronadji dobavljaca po ID-u
// 14 - Zastita: provjera !arr
Supplier* supplier_find_by_id(SupplierArray* arr, int id) {

    if (!arr)
        return NULL;

    // Linearna pretraga
    for (size_t i = 0; i < arr->size; ++i) {

        if (arr->items[i].id == id)

            // 12 - Vraca pokazivac na element u nizu
            return &arr->items[i];
    }

    return NULL;
}


// ==========================================
// Azuriranje dobavljaca
// ==========================================

// 1 - CRUID - Update: azuriranje podataka postojeceg dobavljaca
// 13 - Funkcija: azurira tocno jedan zapis
// 14 - Zastita: provjera !arr i !s
int supplier_update(SupplierArray* arr,
    int id,
    const Supplier* s) {

    if (!arr || !s)
        return -1;

    // 12 - Pokazivac: found pokazuje direktno na element u nizu
    Supplier* found =
        supplier_find_by_id(arr, id);

    if (!found)
        return -1;

    // Provjera konfliktnog ID-a
    if (s->id != id &&
        supplier_id_exists(arr, s->id))
        return -2;

    // Kopiranje cijele strukture na lokaciju pokazivaca
    *found = *s;

    return 0;
}


// ==========================================
// Brisanje dobavljaca
// ==========================================

// 1 - CRUID - InsertDelete: brisanje dobavljaca iz niza
// 13 - Funkcija: jedan posao - brise element i kompaktira niz
// 14 - Zastita: provjera !arr
int supplier_delete(SupplierArray* arr, int id) {

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

    // Zadnji element prebaci na mjesto obrisanog (brzo brisanje O(1))
    if (idx != arr->size - 1)
        arr->items[idx] =
        arr->items[arr->size - 1];

    arr->size--;

    return 0;
}


// ==========================================
// Ispis jednog dobavljaca
// ==========================================

// 13 - Funkcija: jedan posao - ispis jednog zapisa
// 14 - Zastita: provjera !s
void supplier_print(const Supplier* s) {

    if (!s)
        return;

    printf(
        "ID:%d | Naziv:%s | Kontakt:%s\n",
        s->id,
        s->name,
        s->contact
    );
}


// ==========================================
// Ispis svih dobavljaca
// ==========================================

// 13 - Funkcija: iterira i ispisuje sve elemente
// 14 - Zastita: provjera !arr
void supplier_print_all(const SupplierArray* arr) {

    if (!arr)
        return;

    if (arr->size == 0) {
        printf("Nema dobavljaca.\n");
        return;
    }

    for (size_t i = 0; i < arr->size; ++i) {
        supplier_print(&arr->items[i]);
    }
}


// ==========================================
// Generiranje testnih podataka
// ==========================================

// 13 - Funkcija: generira testne podatke za dobavljace
// 14 - Zastita: provjera !arr
int supplier_generate_test_data(SupplierArray* arr,
    size_t n) {

    if (!arr)
        return -1;

    int startId = 1;

    for (size_t i = 0; i < arr->size; ++i) {
        if (arr->items[i].id >= startId)
            startId = arr->items[i].id + 1;
    }

    for (size_t i = 0; i < n; ++i) {

        // 3 - Slozeni tip: lokalna Supplier struktura inicijalizirana na 0
        Supplier s = { 0 };

        s.id = startId + (int)i;

        snprintf(
            s.name,
            sizeof(s.name),
            "Dobavljac_%d",
            s.id
        );

        snprintf(
            s.contact,
            sizeof(s.contact),
            "kontakt%d@primjer.com",
            s.id
        );

        int rc = supplier_add(arr, &s);

        if (rc == -2 || rc == -3)
            continue;
        else if (rc != 0)
            return -1;
    }

    return 0;
}


// ==========================================
// Comparator za qsort po ID-u
// ==========================================

// 23 - Sortiranje: comparator funkcija za qsort
// 26 - Pokazivac na funkciju: ova funkcija se prosljedjuje qsortu kao argument (pointer na funkciju)
// 12 - Pokazivaci: void* parametri pretvoreni u konkretne tipove
int supplier_compare_by_id(const void* a,
    const void* b) {

    // 12 - Pretvaranje void* u Supplier*
    const Supplier* sa = (const Supplier*)a;
    const Supplier* sb = (const Supplier*)b;

    return (sa->id - sb->id);
}


// ==========================================
// Uklanjanje duplikata
// ==========================================

// 13 - Funkcija: uklanja duplikate sortiranjem pa linearnim prolaskom
// 23 - Sortiranje: koristi qsort s comparatorom
// 26 - Pokazivac na funkciju: supplier_compare_by_id se prosljedjuje qsortu
void supplier_remove_duplicates(SupplierArray* arr) {

    if (!arr || arr->size <= 1)
        return;

    // 23 - qsort: standardna bibliotecna funkcija za sortiranje
    // 26 - Pokazivac na funkciju: supplier_compare_by_id proslijedjen kao argument
    qsort(
        arr->items,
        arr->size,
        sizeof(Supplier),
        supplier_compare_by_id
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

            // 8 - extern globalConfig: pristup globalnoj varijabli definiranoj u utils.c
            if (globalConfig.verbose) {

                printf(
                    "Upozorenje: pronaden duplikat "
                    "dobavljaca s ID=%d; "
                    "preskacem duplikat.\n",

                    arr->items[read].id
                );
            }
        }
    }

    arr->size = write;
}