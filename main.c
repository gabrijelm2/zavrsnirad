#include "header.h"

int main(void) {

    init_utils();

    const char* DATA_FILE = "data.bin";

    ProductArray products;
    SupplierArray suppliers;

    product_init(&products);
    supplier_init(&suppliers);


    if (file_exists(DATA_FILE)) {


        long velicina = fileio_get_size(DATA_FILE);
        printf("Ucitana baza podataka ima velicinu od: %ld bajtova.\n", velicina);

        printf(
            "Pronadjena datoteka '%s' "
            "s prethodnim podacima.\n",

            DATA_FILE
        );

    
        int choice =
            read_int(
                "Zelis li ucitati postojece "
                "podatke? "
                "(1 = DA, 0 = NE): "
            );

        if (choice == 1) {

 
            if (fileio_load_all(
                DATA_FILE,
                &products,
                &suppliers
            ) != 0)

                fprintf(
                    stderr,
                    "Greska pri ucitavanju "
                    "iz %s. "
                    "Pokrece se prazno skladiste.\n",

                    DATA_FILE
                );

            else if (globalConfig.verbose)

                printf(
                    "Podaci uspjesno ucitani "
                    "iz %s\n",

                    DATA_FILE
                );
        }
        else {

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



    main_menu(
        &products,
        &suppliers,
        DATA_FILE
    );

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

        
        fprintf(
            stderr,
            "Greska pri spremanju podataka.\n"
        );
    }

    product_free(&products);
    supplier_free(&suppliers);

    cleanup_utils();

    return 0;
}