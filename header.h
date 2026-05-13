/*
  header.h
  Zajednicki header fajl projekta

  Sadrzi:
  - include biblioteke
  - enum tipove
  - strukture
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
#include <stdlib.h>

// String funkcije
#include <string.h>

// Error kodovi
#include <errno.h>


// ==========================================
// ENUMI
// ==========================================


// Enum kategorija proizvoda
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


// Enum statusa narudzbe
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


// Enum glavnog izbornika
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


// Struktura proizvoda
typedef struct Product {

    // ID proizvoda
    int id;

    // Naziv proizvoda
    char name[64];

    // ID kategorije
    int categoryId;

    // Cijena proizvoda
    double price;

    // Kolicina na skladistu
    int quantity;

    // ID dobavljaca
    int supplierId;

} Product;


// Struktura dobavljaca
typedef struct Supplier {

    // ID dobavljaca
    int id;

    // Naziv dobavljaca
    char name[64];

    // Kontakt dobavljaca
    char contact[64];

} Supplier;


// Dinamicki niz proizvoda
typedef struct ProductArray {

    // Pointer na niz proizvoda
    Product* items;

    // Trenutni broj elemenata
    size_t size;

    // Kapacitet memorije
    size_t capacity;

} ProductArray;


// Dinamicki niz dobavljaca
typedef struct SupplierArray {

    // Pointer na niz dobavljaca
    Supplier* items;

    // Broj elemenata
    size_t size;

    // Kapacitet memorije
    size_t capacity;

} SupplierArray;


// Globalna konfiguracija programa
typedef struct Config {

    // Verbose mod:
    // 1 = dodatni ispisi
    // 0 = tihi rad
    int verbose;

} Config;


// ==========================================
// GLOBALNE VARIJABLE
// ==========================================


// extern:
// varijabla postoji u drugom .c fajlu
extern Config globalConfig;


// ==========================================
// DEKLARACIJE FUNKCIJA - utils
// ==========================================


// Inicijalizacija utility sustava
void init_utils(void);

// Cleanup utility sustava
void cleanup_utils(void);

// Unos integera
int read_int(const char* prompt);

// Unos decimalnog broja
double read_double(const char* prompt);

// Unos stringa
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


// Pretvara category ID u tekst
const char* category_get_name(int categoryId);

// Provjera je li kategorija valjana
int is_valid_category(int categoryId);

// Ispis svih kategorija
void print_all_categories(void);


// ==========================================
// DEKLARACIJE - product
// ==========================================


// Inicijalizacija niza proizvoda
void product_init(ProductArray* arr);

// Oslobadjanje memorije
void product_free(ProductArray* arr);

// Dodavanje proizvoda
int product_add(
    ProductArray* arr,
    const Product* p
);

// Pretraga po ID-u
Product* product_find_by_id(
    ProductArray* arr,
    int id
);

// Azuriranje proizvoda
int product_update(
    ProductArray* arr,
    int id,
    const Product* p
);

// Brisanje proizvoda
int product_delete(
    ProductArray* arr,
    int id
);

// Ispis jednog proizvoda
void product_print(const Product* p);

// Ispis svih proizvoda
void product_print_all(
    const ProductArray* arr
);

// Generiranje testnih podataka
int product_generate_test_data(
    ProductArray* arr,
    size_t n
);

// Comparator po ID-u
int product_compare_by_id(
    const void* a,
    const void* b
);

// Comparator po nazivu
int product_compare_by_name(
    const void* a,
    const void* b
);

// Provjera postoji li ID
int product_id_exists(
    const ProductArray* arr,
    int id
);

// Provjera postoji li naziv
int product_name_exists(
    const ProductArray* arr,
    const char* name
);

// Uklanjanje duplikata
void product_remove_duplicates(
    ProductArray* arr
);


// ==========================================
// DEKLARACIJE - supplier
// ==========================================


// Inicijalizacija dobavljaca
void supplier_init(SupplierArray* arr);

// Oslobadjanje memorije
void supplier_free(SupplierArray* arr);

// Dodavanje dobavljaca
int supplier_add(
    SupplierArray* arr,
    const Supplier* s
);

// Pretraga po ID-u
Supplier* supplier_find_by_id(
    SupplierArray* arr,
    int id
);

// Azuriranje dobavljaca
int supplier_update(
    SupplierArray* arr,
    int id,
    const Supplier* s
);

// Brisanje dobavljaca
int supplier_delete(
    SupplierArray* arr,
    int id
);

// Ispis jednog dobavljaca
void supplier_print(const Supplier* s);

// Ispis svih dobavljaca
void supplier_print_all(
    const SupplierArray* arr
);

// Generiranje testnih podataka
int supplier_generate_test_data(
    SupplierArray* arr,
    size_t n
);

// Comparator po ID-u
int supplier_compare_by_id(
    const void* a,
    const void* b
);

// Provjera ID-a
int supplier_id_exists(
    const SupplierArray* arr,
    int id
);

// Provjera naziva
int supplier_name_exists(
    const SupplierArray* arr,
    const char* name
);

// Uklanjanje duplikata
void supplier_remove_duplicates(
    SupplierArray* arr
);


// ==========================================
// DEKLARACIJE - fileio
// ==========================================


// Spremanje svih podataka
int fileio_save_all(
    const char* filename,
    const ProductArray* products,
    const SupplierArray* suppliers
);

// Ucitavanje svih podataka
int fileio_load_all(
    const char* filename,
    ProductArray* products,
    SupplierArray* suppliers
);

// Kopiranje datoteke
int file_copy(
    const char* src,
    const char* dst
);


// ==========================================
// DEKLARACIJE - menu
// ==========================================


// Glavni izbornik programa
void main_menu(
    ProductArray* products,
    SupplierArray* suppliers,
    const char* dataFile
);


// Kraj header guarda
#endif
