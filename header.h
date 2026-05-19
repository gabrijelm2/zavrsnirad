/*
  header.h
  Zajednicki header fajl projekta

  Sadrzi:
  - include biblioteke
  // 7 - Organizacija izvornog koda: svi tipovi, deklaracije i extern varijable su centralizirani u jednom headeru
  - enum tipove
  - strukture
  // 3 - Slozeni tipovi podataka: Product, Supplier, ProductArray, SupplierArray, Config
  - deklaracije funkcija
  - globalne varijable
*/

#ifndef HEADER_H
#define HEADER_H

// Header guard:
// sprjecava visestruko ukljucivanje headera


// ==========================================
// MSVC specificno upozorenje
// ==========================================

// Ako se koristi Microsoft Visual C compiler
#ifdef _MSC_VER

// Gasi warning za "nesigurne" funkcije
#define _CRT_SECURE_NO_WARNINGS

#endif


// ==========================================
// Standardne biblioteke
// ==========================================

// Standardni input/output
#include <stdio.h>

// Dinamicka memorija
// 16 - Priprema za dinamicko zauzimanje memorije (malloc, calloc, realloc, free)
#include <stdlib.h>

// String funkcije
#include <string.h>

// Error kodovi
// 22 - errno: ukljucen za upravljanje pogresama
#include <errno.h>


// ==========================================
// ENUMI
// ==========================================


// 4 - typedef enum: ProductCategory definiran s typedef za jednostavniju upotrebu
// 11 - Kod izbornika koristiti enum tipove: kategorije su enum, koriste se u menuju
typedef enum ProductCategory {

    // Stapovi imaju vrijednost 0
    CAT_STAPOVI = 0,

    // Sljedeci automatski postaje 1
    CAT_ROLE,

    // 2
    CAT_NAJLON,

    // 3
    CAT_UDICE,

    // 4
    CAT_HRANA,

    // Ukupan broj kategorija
    // NIJE prava kategorija
    CAT_COUNT

} ProductCategory;


// 4 - typedef enum: OrderStatus definiran s typedef
typedef enum OrderStatus {

    // Narudzba kreirana
    ORDER_PENDING = 0,

    // Poslana
    ORDER_SHIPPED,

    // Dostavljena
    ORDER_DELIVERED,

    // Otkazana
    ORDER_CANCELLED

} OrderStatus;


// 4 - typedef enum: MainOption za opcije glavnog izbornika
// 11 - Enum tipovi koriste se za opcije izbornika umjesto golih integera
typedef enum MainOption {

    // Izlaz
    OPT_EXIT = 0,

    // Proizvodi
    OPT_PRODUCTS = 1,

    // Dobavljaci
    OPT_SUPPLIERS = 2,

    // Izvjestaji
    OPT_REPORTS = 3,

    // Generiranje testnih podataka
    OPT_GENERATE_TEST = 4,

    // Backup
    OPT_BACKUP = 8,

    // Spremanje podataka
    OPT_SAVE = 9

} MainOption;


// ==========================================
// STRUKTURE
// ==========================================


// 3 - Slozeni tip podataka: struktura Product za pohranu podataka o proizvodu
// 4 - typedef struct: Product definiran s typedef za jednostavniju upotrebu
// 13 - Primjena struktura: Product grupira sve relevantne atribute proizvoda
typedef struct Product {

    // 2 - Primitivni tip int za ID
    int id;

    // 15 - Staticko polje: char array fiksne velicine umjesto VLA
    char name[64];

    // 2 - Primitivni tip int za ID kategorije
    int categoryId;

    // 2 - Primitivni tip double za cijenu (realni broj)
    double price;

    // 2 - Primitivni tip int za kolicinu
    int quantity;

    // 2 - Primitivni tip int za ID dobavljaca
    int supplierId;

} Product;


// 3 - Slozeni tip podataka: struktura Supplier
// 4 - typedef struct: Supplier definiran s typedef
typedef struct Supplier {

    // 2 - int za ID
    int id;

    // 15 - Staticko polje fiksne velicine
    char name[64];

    // 15 - Staticko polje fiksne velicine
    char contact[64];

} Supplier;


// 3 - Slozeni tip podataka: dinamicki niz proizvoda (wrapper oko dinamicke memorije)
// 16 - Priprema za dinamicko zauzimanje: items ce biti alociran mallocom/reallocom
typedef struct ProductArray {

    // 12 - Pokazivac na niz proizvoda
    Product* items;

    // 2 - size_t za broj elemenata (odabran primitivni tip za velicinu)
    size_t size;

    // 2 - size_t za kapacitet memorije
    size_t capacity;

} ProductArray;


// 3 - Slozeni tip podataka: dinamicki niz dobavljaca
// 16 - Priprema za dinamicko zauzimanje: items ce biti alociran dinamicki
typedef struct SupplierArray {

    // 12 - Pokazivac na niz dobavljaca
    Supplier* items;

    // Broj elemenata
    size_t size;

    // Kapacitet memorije
    size_t capacity;

} SupplierArray;


// 3 - Slozeni tip podataka: globalna konfiguracija programa
// 4 - typedef struct: Config definiran s typedef
typedef struct Config {

    // Verbose mod:
    // 1 = dodatni ispisi
    // 0 = tihi rad
    int verbose;

} Config;


// ==========================================
// GLOBALNE VARIJABLE
// ==========================================


// 8 - extern: globalConfig deklariran ovdje, definiran u utils.c
// extern oznacava da varijabla postoji u drugom .c fajlu
extern Config globalConfig;


// ==========================================
// DEKLARACIJE FUNKCIJA - utils
// ==========================================


// Inicijalizacija utility sustava
void init_utils(void);

// Cleanup utility sustava
void cleanup_utils(void);

// 13 - Funkcije: read_int vrsi unos integera
// 14 - Zastita parametara: prompt je const char* (ne moze se mijenjati)
int read_int(const char* prompt);

// Unos decimalnog broja
double read_double(const char* prompt);

// 14 - Zastita parametara: prompt je const, buf dobiva bufsize za sigurno upisivanje
void read_string(
    const char* prompt,
    char* buf,
    size_t bufsize
);

// Provjera postoji li datoteka
int file_exists(const char* filename);


// ==========================================
// DEKLARACIJE - category
// ==========================================


// 13 - Funkcija: category_get_name pretvara ID u string
// 14 - Zastita: vraca const char* - rezultat se ne smije mijenjati
const char* category_get_name(int categoryId);

// Provjera je li kategorija valjana
int is_valid_category(int categoryId);

// Ispis svih kategorija
void print_all_categories(void);


// ==========================================
// DEKLARACIJE - product
// ==========================================


// 13 - Funkcije za rad s produktima (CRUD + pomocne)
// 14 - Zastita: arr je pointer koji se provjerava unutar svake funkcije

void product_init(ProductArray* arr);
void product_free(ProductArray* arr);

// 1 - CRUID - Create: product_add
int product_add(
    ProductArray* arr,
    const Product* p
);

// 1 - CRUID - Read: product_find_by_id
// 12 - Vraca pokazivac na pronadjeni proizvod
Product* product_find_by_id(
    ProductArray* arr,
    int id
);

// 1 - CRUID - Update: product_update
int product_update(
    ProductArray* arr,
    int id,
    const Product* p
);

// 1 - CRUID - InsertDelete: product_delete
int product_delete(
    ProductArray* arr,
    int id
);

void product_print(const Product* p);
void product_print_all(
    const ProductArray* arr
);

int product_generate_test_data(
    ProductArray* arr,
    size_t n
);

// 26 - Pokazivac na funkciju: ove funkcije se koriste kao argumenti za qsort/bsearch
int product_compare_by_id(
    const void* a,
    const void* b
);

int product_compare_by_name(
    const void* a,
    const void* b
);

int product_id_exists(
    const ProductArray* arr,
    int id
);

int product_name_exists(
    const ProductArray* arr,
    const char* name
);

void product_remove_duplicates(
    ProductArray* arr
);


// ==========================================
// DEKLARACIJE - supplier
// ==========================================


// 1 - CRUID operacije za dobavljace (Create, Read, Update, InsertDelete)
void supplier_init(SupplierArray* arr);
void supplier_free(SupplierArray* arr);

int supplier_add(
    SupplierArray* arr,
    const Supplier* s
);

// 12 - Vraca pokazivac na pronadjenog dobavljaca
Supplier* supplier_find_by_id(
    SupplierArray* arr,
    int id
);

int supplier_update(
    SupplierArray* arr,
    int id,
    const Supplier* s
);

int supplier_delete(
    SupplierArray* arr,
    int id
);

void supplier_print(const Supplier* s);
void supplier_print_all(
    const SupplierArray* arr
);

int supplier_generate_test_data(
    SupplierArray* arr,
    size_t n
);

// 26 - Pokazivac na funkciju: koristi se kao argument za qsort
int supplier_compare_by_id(
    const void* a,
    const void* b
);

int supplier_id_exists(
    const SupplierArray* arr,
    int id
);

int supplier_name_exists(
    const SupplierArray* arr,
    const char* name
);

void supplier_remove_duplicates(
    SupplierArray* arr
);


// ==========================================
// DEKLARACIJE - fileio
// ==========================================


// 19 - Datoteke: funkcije za rad s binarnom datotekom
int fileio_save_all(
    const char* filename,
    const ProductArray* products,
    const SupplierArray* suppliers
);

int fileio_load_all(
    const char* filename,
    ProductArray* products,
    SupplierArray* suppliers
);

// 21 - file_copy: implementacija kopiranja datoteke
int file_copy(
    const char* src,
    const char* dst
);

// 20 - fseek/ftell/rewind: deklaracija funkcije za dobivanje velicine datoteke
long fileio_get_size(const char* filename);


// ==========================================
// DEKLARACIJE - menu
// ==========================================


// 10 - Izbornik: deklaracija glavnog izbornika
void main_menu(
    ProductArray* products,
    SupplierArray* suppliers,
    const char* dataFile
);


// Kraj header guarda
#endif
