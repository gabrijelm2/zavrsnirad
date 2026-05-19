/*
  fileio.c
  Rad s binarnim datotekama

  Funkcije:
  - spremanje podataka
  - ucitavanje podataka
  - kopiranje datoteka
*/

#include "header.h"


// ==========================================
// Spremanje svih podataka u binarnu datoteku
// ==========================================

int fileio_save_all(
    const char* filename,
    const ProductArray* products,
    const SupplierArray* suppliers
) {

    // Provjera parametara
    if (!filename || !products || !suppliers)
        return -1;

    // fopen:
    // wb = write binary
    FILE* f = fopen(filename, "wb");

    // Ako datoteka nije otvorena
    if (!f) {

        // perror ispisuje sistemsku gresku
        perror("fopen za pisanje");
        return errno;
    }

    // ==========================================
    // Spremanje broja proizvoda
    // ==========================================

    if (
        fwrite(&products->size, sizeof(size_t), 1, f) != 1) {
        perror("fwrite");
        fclose(f);
        return -1;
    }

    // ==========================================
    // Spremanje broja dobavljaca
    // ==========================================

    if (
        fwrite(&suppliers->size, sizeof(size_t), 1, f) != 1) {
        perror("fwrite");
        fclose(f);
        return -1;
    }

    // ==========================================
    // Spremanje svih proizvoda
    // ==========================================

    if (products->size > 0) {
        if (fwrite(products->items, sizeof(Product), products->size, f) != products->size) {
            perror("fwrite products");
            fclose(f);
            return -1;
        }
    }

    // ==========================================
    // Spremanje svih dobavljaca
    // ==========================================

    if (suppliers->size > 0) {

        if (fwrite(suppliers->items, sizeof(Supplier), suppliers->size, f) != suppliers->size) {
            perror("fwrite suppliers");
            fclose(f);
            return -1;
        }
    }

    // Zatvaranje datoteke
    fclose(f);
    return 0;
}

// ==========================================
// Ucitavanje podataka iz binarne datoteke
// ==========================================

int fileio_load_all(
    const char* filename,
    ProductArray* products,
    SupplierArray* suppliers
) {

    // Provjera parametara
    if (!filename || !products || !suppliers)
        return -1;


    // Otvaranje datoteke
    // rb = read binary
    FILE* f = fopen(filename, "rb");

    // Ako otvaranje nije uspjelo
    if (!f) {
        perror("fopen za citanje");
        return errno;
    }

    // Broj proizvoda i dobavljaca
    size_t pcount = 0;
    size_t scount = 0;

    // ==========================================
    // Citanje broja proizvoda
    // ==========================================

    if (fread(&pcount, sizeof(size_t), 1, f) != 1) {

        // Ako je kraj datoteke
        if (feof(f)) {
            fclose(f);
            return 0;
        }
        perror("fread");
        fclose(f);
        return -1;
    }

    // ==========================================
    // Citanje broja dobavljaca
    // ==========================================

    if (
        fread(&scount, sizeof(size_t), 1, f) != 1) {
        perror("fread");
        fclose(f);
        return -1;
    }


    // ==========================================
    // Reset nizova
    // ==========================================

    product_init(products);
    supplier_init(suppliers);


    // ==========================================
    // Ucitavanje proizvoda
    // ==========================================

    if (pcount > 0) {

        // Alokacija memorije
        products->items =
            (Product*)malloc(pcount * sizeof(Product));

        // Ako malloc nije uspio
        if (!products->items) {
            fclose(f);
            return -1;
        }

        // Ucitavanje proizvoda iz datoteke
        if (
            fread(products->items, sizeof(Product), pcount, f) != pcount) {

            perror("fread products");
            free(products->items);
            fclose(f);
            return -1;
        }

        // Postavljanje velicine niza
        products->size = pcount;

        // Postavljanje kapaciteta
        products->capacity = pcount;
    }

    // ==========================================
    // Ucitavanje dobavljaca
    // ==========================================

    if (scount > 0) {

        // Alokacija memorije
        suppliers->items =
            (Supplier*)malloc(
                scount * sizeof(Supplier)
            );

        // Ako malloc nije uspio
        if (!suppliers->items) {

            fclose(f);
            return -1;
        }

        // Ucitavanje dobavljaca
        if (
            fread(suppliers->items, sizeof(Supplier), scount, f) != scount) {
            perror("fread suppliers");
            free(suppliers->items);
            fclose(f);
            return -1;
        }

        // Postavljanje velicine
        suppliers->size = scount;

        // Postavljanje kapaciteta
        suppliers->capacity = scount;
    }

    // Zatvaranje datoteke
    fclose(f);

    // ==========================================
    // Uklanjanje mogucih duplikata
    // ==========================================
    product_remove_duplicates(products);
    supplier_remove_duplicates(suppliers);
    return 0;
}

// ==========================================
// Kopiranje datoteke
// ==========================================

int file_copy(
    const char* src,
    const char* dst
) {

    // Provjera parametara
    if (!src || !dst)
        return -1;

    // Otvaranje source datoteke
    FILE* fs = fopen(src, "rb");

    // Ako source nije otvoren
    if (!fs) {
        perror("fopen src");
        return errno;
    }


    // Otvaranje destination datoteke
    FILE* fd = fopen(dst, "wb");

    // Ako destination nije otvoren
    if (!fd) {
        perror("fopen dst");
        fclose(fs);
        return errno;
    }

    // Buffer za kopiranje podataka
    char buf[4096];

    // Broj procitanih byteova
    size_t n;


    // ==========================================
    // Citanje i pisanje u petlji
    // ==========================================

    while (

        // fread vraca broj procitanih byteova
        (n = fread(buf, 1, sizeof(buf), fs)) > 0) {

        // fwrite zapisuje byteove
        if (fwrite(buf, 1, n, fd) != n) {
            perror("fwrite");
            fclose(fs);
            fclose(fd);
            return -1;
        }
    }


    // Provjera greske pri citanju
    if (ferror(fs)) {
        perror("fread");
        fclose(fs);
        fclose(fd);

        return -1;
    }
    // Zatvaranje datoteka
    fclose(fs);
    fclose(fd);

    return 0;
}

// Funkcija koja koristi fseek i ftell za racunanje velicine datoteke (Pravilo 20)
long fileio_get_size(const char* filename) {
    if (!filename) return -1;

    FILE* f = fopen(filename, "rb");
    if (!f) return 0; // Datoteka jos ne postoji

    fseek(f, 0, SEEK_END); // Skoci na kraj
    long size = ftell(f);  // Procitaj poziciju (velicina u bajtovima)
    rewind(f);             // Vrati na pocetak (isto kao fseek na 0)

    fclose(f);
    return size;
}