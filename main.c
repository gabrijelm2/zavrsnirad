/*
  main.c
  Ulazna tocka programa - upravlja inicijalizacijom i rutinom programa

  Program je sustav za upravljanje skladistem koji omogucava:
  - Upravljanje kategorijama proizvoda
  - Upravljanje proizvodima
  - Upravljanje dobavljacima
  - Spreminanje i ucitavanje podataka iz binarne datoteke

  Kompajliranje:
    GCC:  gcc -std=c11 -Wall -Wextra -O2 -o skladiste main.c utils.c category.c product.c supplier.c fileio.c menu.c
    MSVC: cl /W3 /Ox main.c utils.c category.c product.c supplier.c fileio.c menu.c
*/

#include "header.h"

/**
 * main() - Glavna funkcija programa
 * 
 * Redoslijed izvršavanja:
 * 1. Inicijalizira pomoćne alate (utils)
 * 2. Definiše putanju do datoteke za pohranu podataka
 * 3. Pravi nizove za pohranu proizvoda i dobavljača
 * 4. Učitava postojeće podatke iz datoteke ako postoje
 * 5. Prikazuje glavni izbornik programu
 * 6. Sprema sve podatke prije zatváranja programa
 * 7. Oslobađa memoriju i završava
 *
 * Povratna vrijednost: 0 (uspješan završetak)
 */
int main(void) {
    /* Inicijalizacija utils modula - sprema se globalna konfiguracija */
    init_utils();

    /* Konstanta s putanjom do binarne datoteke gdje se čuvaju podaci */
    const char* DATA_FILE = "data.bin";

    /*
     * Deklaracija struktura za pohranu podataka:
     * - ProductArray: dinamički niz svih proizvoda
     * - SupplierArray: dinamički niz svih dobavljača
     */
    ProductArray  products;
    SupplierArray suppliers;
    
    /* Inicijalizacija nizova (postavljanje na prazne nizove) */
    product_init(&products);
    supplier_init(&suppliers);

    /*
     * UCITAVANJE POSTOJECIH PODATAKA
     * 
     * Provjeravamo postoji li datoteka s prethodnim podacima.
     * Ako postoji, pitamo korisnika želi li ju učitati.
     */
    if (file_exists(DATA_FILE)) {
        printf("Pronasao sam datoteku '%s' s prethodnim podacima.\n", DATA_FILE);
        
        /* Čitamo korisnički odgovor (1 = DA, 0 = NE) */
        int choice = read_int("Zelis li ucitati postojece podatke? (1 = DA, 0 = NE): ");
        
        if (choice == 1) {
            /* Pokušaj učitati sve podatke iz datoteke */
            if (fileio_load_all(DATA_FILE, &products, &suppliers) != 0) {
                /* Ako je došlo do greške, ispiši upozorenje */
                fprintf(stderr, "Greska pri ucitavanju iz %s. Pokrece se prazno skladiste.\n", DATA_FILE);
            }
            /* Ako je ucitavanje uspjesno i ako je verbose mod ukljucen, potvrdi */
            else if (globalConfig.verbose)
                printf("Podaci uspjesno ucitani iz %s\n", DATA_FILE);
        } else {
            /* Korisnik nije htio ucitati podatke - krece se s praznim skladistem */
            if (globalConfig.verbose) printf("Pocetak s praznim skladistem.\n");
        }
    } else {
        /* Datoteka ne postoji - prvi put pokretanja programa */
        if (globalConfig.verbose) printf("Nema postojece datoteke. Pocetak s praznim skladistem.\n");
    }

    /*
     * GLAVNA PETLJA PROGRAMA
     * 
     * Prikazuje korisniku glavni izbornik i obrada njegovih izbora.
     * Funkcija se nastavlja dok korisnik ne odabere izlaz iz programa.
     */
    main_menu(&products, &suppliers, DATA_FILE);

    /*
     * SPREMINANJE PODATAKA PRIJE IZLAZA
     * 
     * Sprema sve izmijenjene podatke (proizvode, dobavljače) u binarnu datoteku
     * kako bi se sprječio gubitak podataka.
     */
    if (fileio_save_all(DATA_FILE, &products, &suppliers) == 0) {
        /* Spremanje je uspjesno */
        if (globalConfig.verbose) printf("Podaci spremljeni prije izlaza.\n");
    } else {
        /* Greška pri spremanju - ispiši upozorenje na stderr */
        fprintf(stderr, "Greska pri spremanju podataka.\n");
    }

    /*
     * OSLOBAĐANJE MEMORIJE
     * 
     * Oslobađa svu dinamički alociranu memoriju prije završetka programa
     * kako bi se izbjeglo curenje memorije.
     */
    product_free(&products);    /* Oslobađa niz proizvoda */
    supplier_free(&suppliers);  /* Oslobađa niz dobavljača */
    cleanup_utils();            /* Očisti resurse iz utils modula */
    
    /* Uspješan završetak programa */
    return 0;
}
