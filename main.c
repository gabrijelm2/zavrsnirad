/*
  main.c
  Glavna ulazna tocka programa
*/

#include "header.h"


// ==========================================
// main funkcija
// Program pocinje izvrsavanje ovdje
// ==========================================

int main(void) {

    // Inicijalizacija utility sustava
    init_utils();


    // Naziv glavne binarne datoteke
    const char* DATA_FILE = "data.bin";


    // Kreiranje struktura za proizvode
    // i dobavljace
    ProductArray products;
    SupplierArray suppliers;


    // Inicijalizacija nizova
    product_init(&products);
    supplier_init(&suppliers);


    // Provjera postoji li data.bin
    if (file_exists(DATA_FILE)) {

        printf(
            "Pronasao sam datoteku '%s' "
            "s prethodnim podacima.\n",

            DATA_FILE
        );

        // Pitanje korisniku
        int choice =
            read_int(
                "Zelis li ucitati postojece "
                "podatke? "
                "(1 = DA, 0 = NE): "
            );

        // Ako korisnik zeli ucitati podatke
        if (choice == 1) {

            // Ucitavanje iz datoteke
            if (fileio_load_all(
                    DATA_FILE,
                    &products,
                    &suppliers
                ) != 0)

                // Greska pri ucitavanju
                fprintf(
                    stderr,
                    "Greska pri ucitavanju "
                    "iz %s. "
                    "Pokrece se prazno skladiste.\n",

                    DATA_FILE
                );

            // Ako je verbose mod ukljucen
            else if (globalConfig.verbose)

                printf(
                    "Podaci uspjesno ucitani "
                    "iz %s\n",

                    DATA_FILE
                );
        }
        else {

            // Korisnik ne zeli ucitati podatke
            if (globalConfig.verbose)

                printf(
                    "Pocetak s praznim "
                    "skladistem.\n"
                );
        }
    }
    else {

        // Datoteka ne postoji
        if (globalConfig.verbose)

            printf(
                "Nema postojece datoteke. "
                "Pocetak s praznim "
                "skladistem.\n"
            );
    }


    // Pokretanje glavnog izbornika
    main_menu(
        &products,
        &suppliers,
        DATA_FILE
    );


    // Automatsko spremanje prije izlaza
    if (fileio_save_all(
            DATA_FILE,
            &products,
            &suppliers
        ) == 0) {

        // Uspjesno spremanje
        if (globalConfig.verbose)

            printf(
                "Podaci spremljeni "
                "prije izlaza.\n"
            );
    }
    else {

        // Greska pri spremanju
        fprintf(
            stderr,
            "Greska pri spremanju podataka.\n"
        );
    }


    // Oslobadjanje memorije proizvoda
    product_free(&products);

    // Oslobadjanje memorije dobavljaca
    supplier_free(&suppliers);


    // Cleanup utility sustava
    cleanup_utils();


    // Povratna vrijednost operativnom sustavu
    // 0 = uspjesan zavrsetak programa
    return 0;
}
