#ifndef HEADER_H
#define HEADER_H

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

typedef enum {
    CAT_STAPOVI = 0,
    CAT_ROLE,
    CAT_NAJLON,
    CAT_COUNT
} ProductCategory;

typedef enum {
    OPT_IZLAZ = 0,
    OPT_DODAJ_P,
    OPT_ISPISI_P,
    OPT_AZURIRAJ_P,
    OPT_OBRISI_P,
    OPT_DODAJ_D,
    OPT_TRAZI_P,
    OPT_SORT_P,
    OPT_BACKUP,
    OPT_CSV_EXPORT
} MenuOption;

typedef struct {
    int id;
    char ime[50];
    int kategorijaId;
    double cijena;
    int kolicina;
    int dobavljacId;
} Product;

typedef struct {
    int id;
    char ime[50];
    char kontakt[50];
} Supplier;

typedef struct {
    Product* items;
    size_t size;
    size_t capacity;
} ProductArray;

typedef struct {
    Supplier* items;
    size_t size;
    size_t capacity;
} SupplierArray;

typedef struct {
    int verbose;
} Config;

extern Config globalnaKonfiguracija;

#define JE_VALJANA_KAT(id) ((id) >= 0 && (id) < CAT_COUNT)

void p_init(ProductArray* a);
void p_free(ProductArray* a);
int  p_add(ProductArray* a, const Product* p);

void s_init(SupplierArray* a);
void s_free(SupplierArray* a);
int  s_add(SupplierArray* a, const Supplier* s);

int  spasi_sve(const char* fn, const ProductArray* pa, const SupplierArray* sa);
int  ucitaj_sve(const char* fn, ProductArray* pa, SupplierArray* sa);
int  kopiraj_datoteku(const char* src, const char* dst);
int  datoteka_postoji(const char* fn);
long dohvati_velicinu_datoteke(const char* fn);

int  p_search_rekurzivni(const Product* artikli, int niski, int visoki, int trazeniId);
int  usporedi_proizvode(const void* a, const void* b);

int  p_id_postoji(const ProductArray* a, int id);
int  p_ime_postoji(const ProductArray* a, const char* ime);
int  s_id_postoji(const SupplierArray* a, int id);
int  s_ime_postoji(const SupplierArray* a, const char* ime);

int  obnovi_datoteku_na_disku(const char* fn, const ProductArray* pa, const SupplierArray* sa);

int  export_csv(const char* fn, const ProductArray* pa);

#endif
