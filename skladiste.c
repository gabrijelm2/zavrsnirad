#include "header.h"

Config globalnaKonfiguracija = { .verbose = 1 };

int p_id_postoji(const ProductArray* a, int id) {
    if (!a) return 0;
    for (size_t i = 0; i < a->size; i++)
        if (a->items[i].id == id) return 1;
    return 0;
}

int p_ime_postoji(const ProductArray* a, const char* ime) {
    if (!a || !ime) return 0;
    for (size_t i = 0; i < a->size; i++)
        if (strcmp(a->items[i].ime, ime) == 0) return 1;
    return 0;
}

int s_id_postoji(const SupplierArray* a, int id) {
    if (!a) return 0;
    for (size_t i = 0; i < a->size; i++)
        if (a->items[i].id == id) return 1;
    return 0;
}

int s_ime_postoji(const SupplierArray* a, const char* ime) {
    if (!a || !ime) return 0;
    for (size_t i = 0; i < a->size; i++)
        if (strcmp(a->items[i].ime, ime) == 0) return 1;
    return 0;
}

static int prosiri_p(ProductArray* a) {
    size_t nc = a->capacity ? a->capacity * 2 : 4;
    Product* tmp = realloc(a->items, nc * sizeof(Product));
    if (!tmp) return -1;
    a->items = tmp;
    a->capacity = nc;
    return 0;
}

static int prosiri_s(SupplierArray* a) {
    size_t nc = a->capacity ? a->capacity * 2 : 4;
    Supplier* tmp = realloc(a->items, nc * sizeof(Supplier));
    if (!tmp) return -1;
    a->items = tmp;
    a->capacity = nc;
    return 0;
}

void p_init(ProductArray* a) { a->items = NULL; a->size = a->capacity = 0; }
void s_init(SupplierArray* a) { a->items = NULL; a->size = a->capacity = 0; }

void p_free(ProductArray* a) {
    free(a->items);
    a->items = NULL;
    a->size = a->capacity = 0;
}

void s_free(SupplierArray* a) {
    free(a->items);
    a->items = NULL;
    a->size = a->capacity = 0;
}

int p_add(ProductArray* a, const Product* p) {
    if (!a || !p || !JE_VALJANA_KAT(p->kategorijaId)) return -1;
    if (p_id_postoji(a, p->id) || p_ime_postoji(a, p->ime)) return -1;
    if (a->size >= a->capacity && prosiri_p(a) != 0) return -1;
    a->items[a->size++] = *p;
    return 0;
}

int s_add(SupplierArray* a, const Supplier* s) {
    if (!a || !s) return -1;
    if (s_id_postoji(a, s->id) || s_ime_postoji(a, s->ime)) return -1;
    if (a->size >= a->capacity && prosiri_s(a) != 0) return -1;
    a->items[a->size++] = *s;
    return 0;
}

int p_search_rekurzivni(const Product* artikli, int niski, int visoki, int trazeniId) {
    if (niski > visoki) return -1;
    int sredina = niski + (visoki - niski) / 2;
    if (artikli[sredina].id == trazeniId) return sredina;
    if (artikli[sredina].id > trazeniId)
        return p_search_rekurzivni(artikli, niski, sredina - 1, trazeniId);
    return p_search_rekurzivni(artikli, sredina + 1, visoki, trazeniId);
}

int usporedi_proizvode(const void* a, const void* b) {
    return ((const Product*)a)->id - ((const Product*)b)->id;
}

int datoteka_postoji(const char* fn) {
    FILE* f = fopen(fn, "rb");
    if (!f) return 0;
    fclose(f);
    return 1;
}

long dohvati_velicinu_datoteke(const char* fn) {
    FILE* f = fopen(fn, "rb");
    if (!f) return -1;
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fclose(f);
    return sz;
}

int spasi_sve(const char* fn, const ProductArray* pa, const SupplierArray* sa) {
    FILE* f = fopen(fn, "wb");
    if (!f) return -1;

    fwrite(&pa->size, sizeof(size_t), 1, f);
    fwrite(&sa->size, sizeof(size_t), 1, f);

    if (pa->size) fwrite(pa->items, sizeof(Product), pa->size, f);
    if (sa->size) fwrite(sa->items, sizeof(Supplier), sa->size, f);

    fclose(f);
    return 0;
}

int obnovi_datoteku_na_disku(const char* fn, const ProductArray* pa, const SupplierArray* sa) {
    const char* temp = "temp_skladiste.bin";
    if (spasi_sve(temp, pa, sa) != 0) return -1;
    remove(fn);
    if (rename(temp, fn) != 0) return -1;
    return 0;
}

int ucitaj_sve(const char* fn, ProductArray* pa, SupplierArray* sa) {
    FILE* f = fopen(fn, "rb");
    if (!f) return -1;

    size_t p_sz = 0, s_sz = 0;
    fread(&p_sz, sizeof(size_t), 1, f);
    fread(&s_sz, sizeof(size_t), 1, f);

    p_init(pa);
    s_init(sa);

    if (p_sz) {
        pa->items = malloc(p_sz * sizeof(Product));
        fread(pa->items, sizeof(Product), p_sz, f);
        pa->size = pa->capacity = p_sz;
    }

    if (s_sz) {
        sa->items = malloc(s_sz * sizeof(Supplier));
        fread(sa->items, sizeof(Supplier), s_sz, f);
        sa->size = sa->capacity = s_sz;
    }

    fclose(f);
    return 0;
}

int kopiraj_datoteku(const char* src, const char* dst) {
    FILE* s = fopen(src, "rb");
    FILE* d = fopen(dst, "wb");
    if (!s || !d) return -1;

    char buf[512];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), s)) > 0)
        fwrite(buf, 1, n, d);

    fclose(s);
    fclose(d);
    return 0;
}

int export_csv(const char* fn, const ProductArray* pa) {
    FILE* f = fopen(fn, "w");
    if (!f) return -1;

    fprintf(f, "ID,Ime,Kategorija,Cijena,Kolicina,DobavljacID\n");

    for (size_t i = 0; i < pa->size; i++) {
        fprintf(f, "%d,%s,%d,%.2f,%d,%d\n",
            pa->items[i].id,
            pa->items[i].ime,
            pa->items[i].kategorijaId,
            pa->items[i].cijena,
            pa->items[i].kolicina,
            pa->items[i].dobavljacId
        );
    }

    fclose(f);
    return 0;
}
