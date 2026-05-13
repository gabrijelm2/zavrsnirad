/*
  header.h - Zaglavlje sustava
  Ovdje definiramo "nacrt" naseg programa: strukture podataka, enumeracije i 
  prototipe svih funkcija koje se koriste kroz razlicite module.
*/

#ifndef HEADER_H
#define HEADER_H

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* ENUMERACIJE (Nabrojani tipovi) 
   Omogucuju bolju citljivost koda zamjenom magicnih brojeva opisnim nazivima.
*/
typedef enum ProductCategory {
    CAT_STAPOVI = 0,
    CAT_ROLE,
    CAT_NAJLON,
    CAT_UDICE,
    CAT_HRANA,
    CAT_COUNT
} ProductCategory;

typedef enum MainOption {
    OPT_EXIT          = 0,
    OPT_PRODUCTS      = 1,
    OPT_SUPPLIERS     = 2,
    OPT_REPORTS       = 3,
    OPT_GENERATE_TEST = 4,
    OPT_BACKUP        = 8,
    OPT_SAVE          = 9
} MainOption;

/* STRUKTURE PODATAKA 
   Product i Supplier su osnovni objekti s kojima radimo.
   ProductArray i SupplierArray su omotaci koji nam omogucuju dinamicko 
   povecanje polja (kapacitet i trenutna velicina).
*/
typedef struct Product {
    int    id;
    char   name[64];
    int    categoryId;
    double price;
    int    quantity;
    int    supplierId;
} Product;

typedef struct ProductArray {
    Product* items;    // Pokazivac na dinamicki alocirano polje
    size_t   size;     // Trenutni broj elemenata u polju
    size_t   capacity; // Maksimalni kapacitet prije nove alokacije
} ProductArray;

typedef struct Supplier {
    int  id;
    char name[64];
    char contact[64];
} Supplier;

typedef struct SupplierArray {
    Supplier* items;
    size_t    size;
    size_t    capacity;
} SupplierArray;

/* Konfiguracija sustava */
typedef struct Config {
    int verbose; // Odreduje koliko ce sustav biti "pricljiv" u konzoli
} Config;

extern Config globalConfig;

/* DEKLARACIJE FUNKCIJA (Prototipi)
   Odvajanjem deklaracija omogucujemo da razlicite .c datoteke znaju jedne za druge.
*/

// Pomocne funkcije (utils.c)
void init_utils(void);
int  read_int(const char* prompt);
double read_double(const char* prompt);
void read_string(const char* prompt, char* buf, size_t bufsize);
int  file_exists(const char* path);

// Kategorije (category.c)
const char* category_get_name(int categoryId);
int  is_valid_category(int categoryId);
void print_all_categories(void);

// Upravljanje proizvodima (product.c) - CRUID operacije
void     product_init(ProductArray* arr);
void     product_free(ProductArray* arr);
int      product_add(ProductArray* arr, const Product* p);
Product* product_find_by_id(ProductArray* arr, int id);
int      product_update(ProductArray* arr, int id, const Product* p);
int      product_delete(ProductArray* arr, int id);
void     product_print(const Product* p);
void     product_print_all(const ProductArray* arr);
int      product_generate_test_data(ProductArray* arr, size_t n);
int      product_compare_by_id(const void* a, const void* b);
int      product_compare_by_name(const void* a, const void* b);
void     product_remove_duplicates(ProductArray* arr);

// Upravljanje dobavljacima (supplier.c)
void      supplier_init(SupplierArray* arr);
void      supplier_free(SupplierArray* arr);
int       supplier_add(SupplierArray* arr, const Supplier* s);
Supplier* supplier_find_by_id(SupplierArray* arr, int id);
int       supplier_update(SupplierArray* arr, int id, const Supplier* s);
int       supplier_delete(SupplierArray* arr, int id);
void      supplier_print(const Supplier* s);
void      supplier_print_all(const SupplierArray* arr);
int       supplier_generate_test_data(SupplierArray* arr, size_t n);
int       supplier_compare_by_id(const void* a, const void* b);
int       supplier_id_exists(const SupplierArray* arr, int id);
int       supplier_name_exists(const SupplierArray* arr, const char* name);
void      supplier_remove_duplicates(SupplierArray* arr);

// Rad s datotekama (fileio.c)
int fileio_save_all(const char* filename, const ProductArray* products, const SupplierArray* suppliers);
int fileio_load_all(const char* filename, ProductArray* products, SupplierArray* suppliers);
int file_copy(const char* src, const char* dst);

// Izbornik (menu.c)
void main_menu(ProductArray* products, SupplierArray* suppliers, const char* dataFile);

#endif
