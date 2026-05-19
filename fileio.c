/*
  fileio.c
  Rad s binarnim datotekama

  // 7 - Organizacija izvornog koda: fileio.c je odvojen modul za sve I/O operacije
  // 19 - Datoteke: koristena binarna datoteka (rb/wb mod), provjera FILE* pokazivaca, zatvaranje

  Funkcije:
  - spremanje podataka
  - ucitavanje podataka
  - kopiranje datoteka
*/

#include "header.h"


// ==========================================
// Spremanje svih podataka u binarnu datoteku
// ==========================================

// 13 - Funkcija: jedan posao - serijalizacija svih podataka u binarnu datoteku
// 14 - Zastita parametara: provjera svih pointera na pocetku
// 19 - Datoteke: binarna datoteka (wb mod), provjera FILE* pointera, fclose na svim putanjama
int fileio_save_all(
    const char* filename,
    const ProductArray* products,
    const SupplierArray* suppliers
) {

    // 14 - Zastita: provjera svih ulaznih pokazivaca
    if (!filename || !products || !suppliers)
        return -1;

    // 19 - Datoteke: otvaranje binarne datoteke za pisanje
    // 12 - Pokazivac: FILE* pointer na datoteku
    FILE* f = fopen(filename, "wb");

    // 19 - Provjera FILE* pokazivaca
    if (!f) {

        // 22 - perror: ispisuje sistemsku gresku (koristi errno interno)
        perror("fopen za pisanje");
        return errno;
    }

    // ==========================================
    // Spremanje broja proizvoda
    // ==========================================

    // 19 - fwrite: pisanje binarnih podataka u datoteku
    if (
        fwrite(&products->size, sizeof(size_t), 1, f) != 1) {
        // 22 - perror: upravljanje pogresama pri pisanju
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
        // 19 - Zatvaranje datoteke i u slucaju greske (ne curenje resursa)
        fclose(f);
        return -1;
    }

    // ==========================================
    // Spremanje svih proizvoda
    // ==========================================

    if (products->size > 0) {
        // 19 - fwrite: serijalizacija cijelog niza struktura u binarnom formatu
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

    // 19 - Zatvaranje datoteke nakon uspjesnog pisanja
    fclose(f);
    return 0;
}

// ==========================================
// Ucitavanje podataka iz binarne datoteke
// ==========================================

// 13 - Funkcija: jedan posao - deserijalizacija podataka iz binarne datoteke
// 14 - Zastita parametara: provjera svih pointera
// 19 - Datoteke: binarna datoteka (rb mod), provjera FILE*, fclose na svim putanjama
int fileio_load_all(
    const char* filename,
    ProductArray* products,
    SupplierArray* suppliers
) {

    // 14 - Zastita svih ulaznih parametara
    if (!filename || !products || !suppliers)
        return -1;

    // 19 - Otvaranje datoteke za citanje u binarnom modu
    FILE* f = fopen(filename, "rb");

    // 19 - Provjera FILE* pokazivaca
    if (!f) {
        // 22 - perror: ispisuje uzrok greske (errno)
        perror("fopen za citanje");
        return errno;
    }

    // 2 - size_t: primitivni tip za broj elemenata
    size_t pcount = 0;
    size_t scount = 0;

    // ==========================================
    // Citanje broja proizvoda
    // ==========================================

    if (fread(&pcount, sizeof(size_t), 1, f) != 1) {

        // 22 - feof: provjera je li kraj datoteke razlog neuspjeha
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

        // 16 - Dinamicko zauzimanje memorije: malloc za niz proizvoda
        // 17 - malloc: alociranje memorije za ucitane proizvode
        products->items =
            (Product*)malloc(pcount * sizeof(Product));

        // 18 - Provjera malloc pokazivaca
        if (!products->items) {
            fclose(f);
            return -1;
        }

        // 19 - fread: citanje binarnih podataka iz datoteke
        if (
            fread(products->items, sizeof(Product), pcount, f) != pcount) {

            perror("fread products");
            // 17 - free + 18 - anuliranje: oslobadjanje memorije u slucaju greske
            free(products->items);
            fclose(f);
            return -1;
        }

        products->size = pcount;
        products->capacity = pcount;
    }

    // ==========================================
    // Ucitavanje dobavljaca
    // ==========================================

    if (scount > 0) {

        // 16 - Dinamicko zauzimanje memorije za dobavljace
        // 17 - malloc: alociranje memorije
        suppliers->items =
            (Supplier*)malloc(
                scount * sizeof(Supplier)
            );

        // 18 - Provjera pokazivaca nakon malloc
        if (!suppliers->items) {

            fclose(f);
            return -1;
        }

        // 19 - fread: citanje dobavljaca iz datoteke
        if (
            fread(suppliers->items, sizeof(Supplier), scount, f) != scount) {
            perror("fread suppliers");
            // 17 - free: oslobadjanje u slucaju greske
            free(suppliers->items);
            fclose(f);
            return -1;
        }

        suppliers->size = scount;
        suppliers->capacity = scount;
    }

    // 19 - Zatvaranje datoteke
    fclose(f);

    // ==========================================
    // Uklanjanje mogucih duplikata
    // ==========================================

    // 23 - Sortiranje i deduplikacija koriste qsort
    product_remove_duplicates(products);
    supplier_remove_duplicates(suppliers);
    return 0;
}

// ==========================================
// Kopiranje datoteke
// ==========================================

// 13 - Funkcija: jedan posao - binarno kopiranje datoteke
// 14 - Zastita: provjera src i dst pointera
// 21 - Implementacija funkcije za kopiranje datoteka (file_copy)
// 19 - Datoteke: otvaranje, provjera FILE*, rad s binarnim modom, zatvaranje
// 22 - ferror: provjera greske pri citanju nakon petlje
int file_copy(
    const char* src,
    const char* dst
) {

    // 14 - Zastita parametara
    if (!src || !dst)
        return -1;

    // 19 - Otvaranje izvorne datoteke u binarnom modu
    FILE* fs = fopen(src, "rb");

    // 19 - Provjera FILE* pokazivaca
    if (!fs) {
        // 22 - perror + errno: upravljanje pogresama
        perror("fopen src");
        return errno;
    }

    // 19 - Otvaranje odredisne datoteke za pisanje
    FILE* fd = fopen(dst, "wb");

    if (!fd) {
        perror("fopen dst");
        fclose(fs);
        return errno;
    }

    // 15 - Staticko polje fiksne velicine za buffer (ne VLA)
    char buf[4096];

    // 2 - size_t: primitivni tip za broj procitanih byteova
    size_t n;

    // ==========================================
    // Citanje i pisanje u petlji
    // ==========================================

    // 19 - fread/fwrite u petlji: kopiranje blokova podataka
    while (

        (n = fread(buf, 1, sizeof(buf), fs)) > 0) {

        if (fwrite(buf, 1, n, fd) != n) {
            perror("fwrite");
            fclose(fs);
            fclose(fd);
            return -1;
        }
    }

    // 22 - ferror: provjera greske pri citanju (nije samo feof)
    if (ferror(fs)) {
        perror("fread");
        fclose(fs);
        fclose(fd);
        return -1;
    }

    // 19 - Zatvaranje obje datoteke
    fclose(fs);
    fclose(fd);

    return 0;
}

// ==========================================
// Racunanje velicine datoteke
// ==========================================

// 20 - fseek, ftell, rewind: koristene za odredjivanje velicine datoteke
// 13 - Funkcija: jedan posao - vraca velicinu datoteke u bajtovima
// 14 - Zastita: provjera filename pokazivaca
long fileio_get_size(const char* filename) {

    // 14 - Zastita parametra
    if (!filename) return -1;

    // 19 - Otvaranje datoteke s provjerom
    FILE* f = fopen(filename, "rb");
    if (!f) return 0; // Datoteka jos ne postoji

    // 20 - fseek: pomicanje pozicije na kraj datoteke
    fseek(f, 0, SEEK_END);

    // 20 - ftell: citanje trenutne pozicije = velicina datoteke
    long size = ftell(f);

    // 20 - rewind: vracanje na pocetak (ekvivalent fseek(f, 0, SEEK_SET))
    rewind(f);

    // 19 - Zatvaranje datoteke
    fclose(f);
    return size;
}
