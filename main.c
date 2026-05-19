/*
  main.c
  Glavna ulazna tocka programa

  // 7 - Organizacija izvornog koda: main.c sadrzi samo inicijalizaciju, pokretanje i cleanup
*/

#include "header.h"


// ==========================================
// main funkcija
// Program pocinje izvrsavanje ovdje
// ==========================================

int main(void) {

    // 13 - Funkcija: init_utils inicijalizira globalne postavke programa
    init_utils();


    // 15 - Staticko polje (string literal): const char* pokazivac na ime datoteke
    // 2 - Primitivni tip: const char* za naziv datoteke
    const char* DATA_FILE = "data.bin";


    // 3 - Slozeni tipovi podataka: ProductArray i SupplierArray su typedef struct
    ProductArray products;
    SupplierArray suppliers;


    // 13 - Inicijalizacija: postavljanje pocetnih vrijednosti struktura
    // 12 - Pokazivaci: prosljedjujemo adrese lokalnih struktura
    product_init(&products);
    supplier_init(&suppliers);


    // 19 - Datoteke: provjera postojanja datoteke prije otvaranja
    if (file_exists(DATA_FILE)) {

        // 20 - ftell/fseek/rewind: fileio_get_size koristi ove funkcije za velicinu
        long velicina = fileio_get_size(DATA_FILE);
        printf("Ucitana baza podataka ima velicinu od: %ld bajtova.\n", velicina);

        printf(
            "Pronadjena datoteka '%s' "
            "s prethodnim podacima.\n",

            DATA_FILE
        );

        // Korisnicko pitanje o ucitavanju
        int choice =
            read_int(
                "Zelis li ucitati postojece "
                "podatke? "
                "(1 = DA, 0 = NE): "
            );

        if (choice == 1) {

            // 19 - Datoteke: ucitavanje iz binarne datoteke
            // 1 - CRUID - Read: ucitavanje svih podataka iz datoteke
            if (fileio_load_all(
                DATA_FILE,
                &products,
                &suppliers
            ) != 0)

                // 22 - stderr: greske se ispisuju na standardni izlaz za greske
                fprintf(
                    stderr,
                    "Greska pri ucitavanju "
                    "iz %s. "
                    "Pokrece se prazno skladiste.\n",

                    DATA_FILE
                );

            // 8 - extern: pristup globalnoj varijabli globalConfig iz utils.c
            else if (globalConfig.verbose)

                printf(
                    "Podaci uspjesno ucitani "
                    "iz %s\n",

                    DATA_FILE
                );
        }
        else {

            // 8 - extern: globalConfig.verbose za uvjetni ispis
            if (globalConfig.verbose)

                printf(
                    "Pocetak s praznim "
                    "skladistem.\n"
                );
        }
    }
    else {

        if (globalConfig.verbose)

            printf(
                "Nema postojece datoteke. "
                "Pocetak s praznim "
                "skladistem.\n"
            );
    }


    // 10 - Izbornik: pokretanje glavnog izbornika programa
    // 12 - Pokazivaci: prosljedjujemo adrese struktura i string
    main_menu(
        &products,
        &suppliers,
        DATA_FILE
    );


    // 19 - Datoteke: automatsko spremanje prije izlaza
    // 1 - CRUID: zapis podataka u datoteku pri izlasku
    if (fileio_save_all(
        DATA_FILE,
        &products,
        &suppliers
    ) == 0) {

        if (globalConfig.verbose)

            printf(
                "Podaci spremljeni "
                "prije izlaza.\n"
            );
    }
    else {

        // 22 - stderr: greske na standardni izlaz za greske
        fprintf(
            stderr,
            "Greska pri spremanju podataka.\n"
        );
    }


    // 17 - free + 18 - anuliranje: oslobadjanje dinamicke memorije
    // 18 - Sigurno brisanje: product_free i supplier_free resetiraju pointer na NULL
    product_free(&products);
    supplier_free(&suppliers);


    // 13 - Funkcija: cleanup_utils za eventualni cleanup resursa
    cleanup_utils();


    // 0 = uspjesan zavrsetak programa (konvencija)
    return 0;
}
