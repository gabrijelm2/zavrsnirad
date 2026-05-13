/*
  supplier.c
  CRUD operacije i pomocne funkcije za dobavljace
*/

#include "header.h"


// ==========================================
// Funkcija za povecavanje kapaciteta niza
// ==========================================

static int grow_supplier_array(SupplierArray* arr) {

    // Provjera postoji li niz
    if (!arr)
        return -1;

    // Ako postoji capacity:
    // novi kapacitet = capacity * 2
    // inace pocetni kapacitet = 8
    size_t newcap = arr->capacity ? arr->capacity * 2 : 8;

    // realloc:
    // povecava ili premjesta memoriju
    Supplier* tmp =
        (Supplier*)realloc(arr->items, newcap * sizeof(Supplier));

    // Ako realloc nije uspio
    if (!tmp)
        return -1;

    // Spremanje nove memorijske adrese
    arr->items = tmp;

    // Spremanje novog kapaciteta
    arr->capacity = newcap;

    return 0;
}


// ==========================================
// Inicijalizacija niza dobavljaca
// ==========================================

void supplier_init(SupplierArray* arr) {

    if (!arr)
        return;

    // Nema elemenata
    arr->items = NULL;

    // Trenutni broj elemenata = 0
    arr->size = 0;

    // Kapacitet memorije = 0
    arr->capacity = 0;
}


// ==========================================
// Oslobadjanje memorije
// ==========================================

void supplier_free(SupplierArray* arr) {

    if (!arr)
        return;

    // Oslobadjanje dinamicke memorije
    free(arr->items);

    // Reset pointera
    arr->items = NULL;

    // Reset velicine
    arr->size = 0;

    // Reset kapaciteta
    arr->capacity = 0;
}


// ==========================================
// Provjera postoji li ID
// ==========================================

int supplier_id_exists(const SupplierArray* arr, int id) {

    if (!arr)
        return 0;

    // Prolazak kroz cijeli niz
    for (size_t i = 0; i < arr->size; ++i) {

        // Ako postoji isti ID
        if (arr->items[i].id == id)
            return 1;
    }

    // ID nije pronadjen
    return 0;
}


// ==========================================
// Provjera postoji li naziv
// ==========================================

int supplier_name_exists(const SupplierArray* arr, const char* name) {

    if (!arr || !name)
        return 0;

    // Prolazak kroz niz
    for (size_t i = 0; i < arr->size; ++i) {

        // strcmp:
        // vraca 0 ako su stringovi jednaki
        if (strcmp(arr->items[i].name, name) == 0)
            return 1;
    }

    return 0;
}


// ==========================================
// Dodavanje dobavljaca
// ==========================================

int supplier_add(SupplierArray* arr, const Supplier* s) {

    // Provjera parametara
    if (!arr || !s)
        return -1;

    // Provjera postoji li ID
    if (supplier_id_exists(arr, s->id))
        return -2;

    // Provjera postoji li naziv
    if (supplier_name_exists(arr, s->name))
        return -3;

    // Ako je niz pun
    if (arr->size >= arr->capacity) {

        // Povecaj memoriju
        if (grow_supplier_array(arr) != 0)
            return -1;
    }

    // Dodavanje elementa na kraj niza
    arr->items[arr->size++] = *s;

    return 0;
}


// ==========================================
// Pretraga po ID-u
// ==========================================

Supplier* supplier_find_by_id(SupplierArray* arr, int id) {

    if (!arr)
        return NULL;

    // Linearna pretraga
    for (size_t i = 0; i < arr->size; ++i) {

        // Ako je ID pronadjen
        if (arr->items[i].id == id)

            // Vrati adresu elementa
            return &arr->items[i];
    }

    return NULL;
}


// ==========================================
// Azuriranje dobavljaca
// ==========================================

int supplier_update(SupplierArray* arr,
                    int id,
                    const Supplier* s) {

    if (!arr || !s)
        return -1;

    // Pronadji postojeci zapis
    Supplier* found =
        supplier_find_by_id(arr, id);

    // Ako ne postoji
    if (!found)
        return -1;

    // Ako se ID mijenja
    // provjeri postoji li novi ID
    if (s->id != id &&
        supplier_id_exists(arr, s->id))
        return -2;

    // Kopiranje cijele strukture
    *found = *s;

    return 0;
}


// ==========================================
// Brisanje dobavljaca
// ==========================================

int supplier_delete(SupplierArray* arr, int id) {

    if (!arr)
        return -1;

    // idx = pozicija elementa za brisanje
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

        // Zadnji element prebaci na mjesto obrisanog
        arr->items[idx] =
            arr->items[arr->size - 1];

    // Smanji broj elemenata
    arr->size--;

    return 0;
}


// ==========================================
// Ispis jednog dobavljaca
// ==========================================

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

void supplier_print_all(const SupplierArray* arr) {

    if (!arr)
        return;

    // Ako nema elemenata
    if (arr->size == 0) {

        printf("Nema dobavljaca.\n");
        return;
    }

    // Ispis svih elemenata
    for (size_t i = 0; i < arr->size; ++i) {

        supplier_print(&arr->items[i]);
    }
}


// ==========================================
// Generiranje testnih podataka
// ==========================================

int supplier_generate_test_data(SupplierArray* arr,
                                size_t n) {

    if (!arr)
        return -1;

    // Pocetni ID
    int startId = 1;

    // Pronadji najveci ID
    for (size_t i = 0; i < arr->size; ++i) {

        if (arr->items[i].id >= startId)
            startId = arr->items[i].id + 1;
    }

    // Generiranje podataka
    for (size_t i = 0; i < n; ++i) {

        Supplier s = { 0 };

        // Postavljanje ID-a
        s.id = startId + (int)i;

        // Generiranje naziva
        snprintf(
            s.name,
            sizeof(s.name),
            "Dobavljac_%d",
            s.id
        );

        // Generiranje kontakta
        snprintf(
            s.contact,
            sizeof(s.contact),
            "kontakt%d@primjer.com",
            s.id
        );

        // Dodavanje u niz
        int rc = supplier_add(arr, &s);

        // Ako postoji duplikat
        if (rc == -2 || rc == -3)
            continue;

        // Ostale greske
        else if (rc != 0)
            return -1;
    }

    return 0;
}


// ==========================================
// Comparator za qsort po ID-u
// ==========================================

int supplier_compare_by_id(const void* a,
                           const void* b) {

    // Pretvaranje void* u Supplier*
    const Supplier* sa = (const Supplier*)a;
    const Supplier* sb = (const Supplier*)b;

    // Razlika ID-eva
    return (sa->id - sb->id);
}


// ==========================================
// Uklanjanje duplikata
// ==========================================

void supplier_remove_duplicates(SupplierArray* arr) {

    if (!arr || arr->size <= 1)
        return;

    // Sortiranje po ID-u
    qsort(
        arr->items,
        arr->size,
        sizeof(Supplier),
        supplier_compare_by_id
    );

    // write = pozicija za zapisivanje
    size_t write = 0;

    // read = citanje elemenata
    for (size_t read = 0;
         read < arr->size;
         ++read) {

        // Ako je prvi element
        // ili nije duplikat
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
                    "Upozorenje: pronaden duplikat "
                    "dobavljaca s ID=%d; "
                    "preskacem duplikat.\n",

                    arr->items[read].id
                );
            }
        }
    }

    // Novi broj elemenata
    arr->size = write;
}
