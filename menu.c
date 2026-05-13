/*
  menu.c - Implementacija korisnickog sucelja
  Ovaj modul upravlja prikazom opcija i usmjerava korisnikove zahtjeve 
  prema funkcijama za obradu podataka (proizvodi, dobavljaci, izvjestaji).
*/

#include "header.h"

// Staticke deklaracije pomocnih funkcija (vidljive samo unutar menu.c)
static void products_menu(ProductArray* products, SupplierArray* suppliers);
static void suppliers_menu(SupplierArray* suppliers);
static void reports_menu(ProductArray* products, SupplierArray* suppliers);
static int  cmp_qty_desc(const void* a, const void* b);

/* GLAVNI IZBORNIK
   Ova funkcija vrti beskonacnu petlju dok korisnik ne odabere izlaz (0).
   Ovdje demonstriras kontrolu toka programa.
*/
void main_menu(ProductArray* products, SupplierArray* suppliers, const char* dataFile) {
    int running = 1;
    while (running) {
        printf("\n=== Skladiste ribolovne opreme ===\n");
        printf("1. Upravljanje proizvodima\n");
        printf("2. Upravljanje dobavljacima\n");
        printf("3. Izvjestaji\n");
        printf("4. Generiraj testne podatke\n");
        printf("8. Backup datoteke\n");
        printf("9. Spremi podatke\n");
        printf("0. Izlaz\n");

        int opt = read_int("Odaberi opciju: ");
        
        /* Koristimo Enum MainOption definiran u headeru radi citljivosti */
        switch (opt) {
            case OPT_PRODUCTS:
                products_menu(products, suppliers);
                break;
            case OPT_SUPPLIERS:
                suppliers_menu(suppliers);
                break;
            case OPT_REPORTS:
                reports_menu(products, suppliers);
                break;
            case OPT_GENERATE_TEST:
                // Poziv funkcija za automatsko punjenje baze (korisno za testiranje)
                product_generate_test_data(products, 20);
                supplier_generate_test_data(suppliers, 5);
                printf("Testni podaci generirani.\n");
                break;
            case OPT_BACKUP:
                // Implementacija kopiranja datoteke (zahtjev br. 21 iz pravila)
                if (file_copy(dataFile, "backup_data.bin") == 0)
                    printf("Backup uspjesno kreiran.\n");
                break;
            case OPT_SAVE:
                // Poziv fileio modula za trajno spremanje
                if (fileio_save_all(dataFile, products, suppliers) == 0)
                    printf("Podaci spremljeni u %s\n", dataFile);
                break;
            case OPT_EXIT:
                running = 0;
                break;
            default:
                printf("Nevazeca opcija, pokusaj ponovo.\n");
        }
    }
}

/* IZBORNIK IZVJESTAJA
   Ovdje mozes pokazati kako filtriras podatke (npr. niska zaliha).
*/
static void reports_menu(ProductArray* products, SupplierArray* suppliers) {
    printf("\n--- Izvjestaji ---\n");
    printf("1. Svi proizvodi (sortirano po ID-u)\n");
    printf("2. Proizvodi s niskom zalihom (manje od 5 kom)\n");
    printf("3. Broj proizvoda po dobavljacu\n");
    
    int opt = read_int("Odabir: ");
    
    if (opt == 1) {
        /* Prezentacijski bitno: qsort koristi pokazivac na funkciju 
           usporedbe (zadnji parametar). To je napredni C koncept. */
        qsort(products->items, products->size, sizeof(Product), product_compare_by_id);
        product_print_all(products);
    } 
    else if (opt == 2) {
        int threshold = 5;
        printf("Proizvodi s kolicinom manjom od %d:\n", threshold);
        for (size_t i = 0; i < products->size; ++i) {
            if (products->items[i].quantity < threshold) {
                product_print(&products->items[i]);
            }
        }
    }
}

// Komparator za sortiranje po kolicini silazno
static int cmp_qty_desc(const void* a, const void* b) {
    const Product* pa = (const Product*)a;
    const Product* pb = (const Product*)b;
    return (pb->quantity - pa->quantity);
}
