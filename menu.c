/*
  menu.c
  Izbornici i interakcija s korisnikom

  // 10 - Izbornik/podizbornici: ovaj fajl sadrzi glavni menu i sve podmenije

  Ovaj fajl sadrzi:
  - glavni menu
  - podmenu za proizvode
  - podmenu za dobavljace
  - report menu
*/

#include "header.h"


// ==========================================
// PROTOTIPI STATIC FUNKCIJA
// ==========================================

// 6 - static: funkcije su vidljive samo unutar menu.c (interni podmenii)
// 13 - Svaki menu je odvojena funkcija koja radi jedan logicki posao

// lokalni menu za proizvode
static void products_menu(ProductArray* products,
    SupplierArray* suppliers);

// lokalni menu za dobavljace
static void suppliers_menu(SupplierArray* suppliers);

// menu za izvjestaje
static void reports_menu(ProductArray* products,
    SupplierArray* suppliers);

// comparator za sortiranje po kolicini (desc)
// 23 - Sortiranje: comparator za qsort u reports_menu
// 26 - Pokazivac na funkciju: prosljedjuje se qsortu
static int cmp_qty_desc(const void* a,
    const void* b);


// ==========================================
// GLAVNI MENU
// ==========================================

// 10 - Izbornik: implementacija glavnog izbornika programa
// 11 - Enum tipovi: switch koristi MainOption enum vrijednosti (OPT_PRODUCTS, OPT_EXIT, ...)
// 13 - Funkcija: main_menu je ulazna tocka za korisnicku interakciju
// 14 - Zastita: svi pointeri prosljedjeni dalje u pod-funkcije koje ih provjeravaju
void main_menu(ProductArray* products,
    SupplierArray* suppliers,
    const char* dataFile) {

    int running = 1;

    // Glavna petlja programa
    while (running) {

        printf("\n=== Skladiste ribolovne opreme ===\n");
        printf("1. Upravljanje proizvodima\n");
        printf("2. Upravljanje dobavljacima\n");
        printf("3. Izvjestaji\n");
        printf("4. Generiraj testne podatke\n");
        printf("8. Backup datoteke\n");
        printf("9. Spremi podatke\n");
        printf("0. Izlaz\n");

        // 13 - Citanje korisnickog odabira kroz pomocnu funkciju
        int opt = read_int("Odaberi opciju: ");

        // 11 - Enum: switch koristi enum vrijednosti MainOption umjesto golih integera
        switch (opt) {

            // 10 - Podizbornik: OPT_PRODUCTS otvara products podmenu
        case OPT_PRODUCTS:
            products_menu(products, suppliers);
            break;

            // 10 - Podizbornik: OPT_SUPPLIERS otvara suppliers podmenu
        case OPT_SUPPLIERS:
            suppliers_menu(suppliers);
            break;

            // 10 - Podizbornik: OPT_REPORTS otvara reports podmenu
        case OPT_REPORTS:
            reports_menu(products, suppliers);
            break;

            // ======================================
            // GENERIRANJE TESTNIH PODATAKA
            // ======================================
        case OPT_GENERATE_TEST: {

            int pn = read_int(
                "Koliko proizvoda generirati? "
            );

            int sn = read_int(
                "Koliko dobavljaca generirati? "
            );

            if (pn > 0) {

                if (product_generate_test_data(
                    products,
                    (size_t)pn
                ) == 0)

                    printf("Proizvodi generirani.\n");

                else
                    printf("Greska pri proizvodima.\n");
            }

            if (sn > 0) {

                if (supplier_generate_test_data(
                    suppliers,
                    (size_t)sn
                ) == 0)

                    printf("Dobavljaci generirani.\n");

                else
                    printf("Greska pri dobavljacima.\n");
            }

            break;
        }

                              // ======================================
                              // BACKUP
                              // ======================================
        case OPT_BACKUP: {

            // 15 - Staticko polje fiksne velicine za ime backup datoteke
            char dst[256];

            read_string(
                "Unesi ime backup datoteke: ",
                dst,
                sizeof(dst)
            );

            // 21 - file_copy: kopiranje datoteke (implementacija kopiranja)
            if (file_copy(dataFile, dst) == 0)
                printf("Backup uspjesan.\n");
            else
                printf("Backup neuspjesan.\n");

            break;
        }

                       // ======================================
                       // SPREMANJE
                       // ======================================
        case OPT_SAVE:

            // 19 - Datoteke: spremanje podataka u binarnu datoteku
            if (fileio_save_all(
                dataFile,
                products,
                suppliers
            ) == 0)

                printf("Podaci spremljeni.\n");

            else
                printf("Greska pri spremanju.\n");

            break;

            // 11 - Enum: OPT_EXIT je enum vrijednost za izlaz
        case OPT_EXIT:
            running = 0;
            break;

        default:
            printf("Nepoznata opcija.\n");
        }
    }
}


// ==========================================
// MENU ZA PROIZVODE
// ==========================================

// 6 - static: products_menu vidljiva samo unutar menu.c
// 10 - Podizbornik za upravljanje proizvodima
// 13 - Funkcija: sve CRUD operacije nad proizvodima kroz jedan podmenu
static void products_menu(ProductArray* products,
    SupplierArray* suppliers) {

    (void)suppliers;

    int running = 1;

    while (running) {

        printf("\n--- Proizvodi ---\n");
        printf("1. Dodaj proizvod\n");
        printf("2. Prikazi sve\n");
        printf("3. Pretraga po ID\n");
        printf("4. Izmjena\n");
        printf("5. Brisanje\n");
        printf("6. Sortiraj po imenu\n");
        printf("7. Sortiraj po ID\n");
        printf("0. Nazad\n");

        int opt = read_int("Opcija: ");

        if (opt == 1) {

            // 3 - Slozeni tip: lokalna Product struktura
            Product p = { 0 };

            p.id = read_int("ID: ");

            // 14 - Provjera: ne dopusti duplikat ID-a
            if (product_id_exists(products, p.id)) {
                printf("ID vec postoji.\n");
                continue;
            }

            read_string("Naziv: ", p.name, sizeof(p.name));

            // 14 - Provjera: ne dopusti duplikat naziva
            if (product_name_exists(products, p.name)) {
                printf("Naziv vec postoji.\n");
                continue;
            }

            // 10 - Ispis kategorija kao dio interakcije izbornika
            print_all_categories();

            int cat = read_int("Kategorija: ");

            // 14 - Validacija: provjera ispravnosti kategorije
            if (!is_valid_category(cat)) {
                printf("Neispravna kategorija.\n");
                continue;
            }

            p.categoryId = cat;
            // 2 - double: cijena je realni broj
            p.price = read_double("Cijena: ");
            p.quantity = read_int("Kolicina: ");
            p.supplierId = read_int("ID dobavljaca: ");

            // 1 - CRUID - Create: dodavanje novog proizvoda
            int rc = product_add(products, &p);

            if (rc == 0)
                printf("Dodan proizvod.\n");
            else
                printf("Greska pri dodavanju.\n");
        }

        else if (opt == 2) {
            // 1 - CRUID - Read: prikaz svih proizvoda
            product_print_all(products);
        }

        else if (opt == 3) {

            int id = read_int("ID: ");

            // 1 - CRUID - Read: pretraga po ID-u
            // 12 - Pokazivac: product_find_by_id vraca pointer
            Product* p =
                product_find_by_id(products, id);

            if (p)
                product_print(p);
            else
                printf("Nije pronadeno.\n");
        }

        else if (opt == 4) {

            int id = read_int("ID za izmjenu: ");

            // 12 - Pokazivac: dohvacamo direktnu referencu na element
            Product* p =
                product_find_by_id(products, id);

            if (!p) {
                printf("Ne postoji.\n");
                continue;
            }

            // Kopija za uredivanje
            Product tmp = *p;

            // 15 - Staticko polje fiksne velicine za privremeni unos
            char buf[128];

            read_string(
                "Novi naziv (enter = skip): ",
                buf,
                sizeof(buf)
            );

            if (strlen(buf) > 0)
                strncpy(tmp.name, buf, sizeof(tmp.name) - 1);

            int newcat = read_int("Nova kategorija (-1 skip): ");
            if (newcat >= 0)
                tmp.categoryId = newcat;

            // 2 - double: cijena kao realni tip
            double newprice = read_double("Nova cijena (0 skip): ");
            if (newprice > 0)
                tmp.price = newprice;

            int newq = read_int("Nova kolicina (-1 skip): ");
            if (newq >= 0)
                tmp.quantity = newq;

            int newsup = read_int("Novi dobavljac (-1 skip): ");
            if (newsup >= 0)
                tmp.supplierId = newsup;

            // 1 - CRUID - Update: azuriranje podataka proizvoda
            product_update(products, id, &tmp);
        }

        else if (opt == 5) {

            int id = read_int("ID za brisanje: ");

            // 1 - CRUID - InsertDelete: brisanje proizvoda
            product_delete(products, id);
        }

        else if (opt == 6) {

            // 23 - Sortiranje: qsort s comparatorom po nazivu
            // 26 - Pokazivac na funkciju: product_compare_by_name prosljedjen qsortu
            qsort(products->items,
                products->size,
                sizeof(Product),
                product_compare_by_name);

            printf("Sortirano.\n");
        }

        else if (opt == 7) {

            // 23 - Sortiranje: qsort s comparatorom po ID-u
            // 26 - Pokazivac na funkciju: product_compare_by_id prosljedjen qsortu
            qsort(products->items,
                products->size,
                sizeof(Product),
                product_compare_by_id);

            printf("Sortirano.\n");
        }

        else if (opt == 0) {
            running = 0;
        }

        else {
            printf("Nepoznata opcija.\n");
        }
    }
}


// ==========================================
// MENU ZA DOBAVLJACE
// ==========================================

// 6 - static: suppliers_menu vidljiva samo unutar menu.c
// 10 - Podizbornik za upravljanje dobavljacima
// 1 - CRUID: sve operacije nad dobavljacima dostupne kroz ovaj podmenu
static void suppliers_menu(SupplierArray* suppliers) {

    int running = 1;

    while (running) {

        printf("\n--- Dobavljaci ---\n");
        printf("1. Dodaj\n");
        printf("2. Prikazi sve\n");
        printf("3. Pretraga\n");
        printf("4. Izmjena\n");
        printf("5. Brisanje\n");
        printf("0. Nazad\n");

        int opt = read_int("Opcija: ");

        if (opt == 1) {

            // 3 - Slozeni tip: lokalna Supplier struktura
            Supplier s = { 0 };

            s.id = read_int("ID: ");
            read_string("Naziv: ", s.name, sizeof(s.name));
            read_string("Kontakt: ", s.contact, sizeof(s.contact));

            // 1 - CRUID - Create
            supplier_add(suppliers, &s);
        }

        else if (opt == 2) {
            // 1 - CRUID - Read: ispis svih
            supplier_print_all(suppliers);
        }

        else if (opt == 3) {

            int id = read_int("ID: ");

            // 1 - CRUID - Read: pretraga po ID-u
            // 12 - Pokazivac: supplier_find_by_id vraca pointer
            Supplier* s =
                supplier_find_by_id(suppliers, id);

            if (s)
                supplier_print(s);
        }

        else if (opt == 4) {

            int id = read_int("ID za izmjenu: ");

            // 12 - Pokazivac: direktna referenca na element
            Supplier* s =
                supplier_find_by_id(suppliers, id);

            if (!s) continue;

            Supplier tmp = *s;

            // 15 - Staticko polje fiksne velicine
            char buf[128];

            read_string("Novi naziv: ", buf, sizeof(buf));
            if (strlen(buf) > 0)
                strncpy(tmp.name, buf, sizeof(tmp.name) - 1);

            read_string("Novi kontakt: ", buf, sizeof(buf));
            if (strlen(buf) > 0)
                strncpy(tmp.contact, buf, sizeof(tmp.contact) - 1);

            // 1 - CRUID - Update
            supplier_update(suppliers, id, &tmp);
        }

        else if (opt == 5) {

            int id = read_int("ID za brisanje: ");

            // 1 - CRUID - InsertDelete
            supplier_delete(suppliers, id);
        }

        else if (opt == 0) {
            running = 0;
        }
    }
}


// ==========================================
// REPORTS MENU
// ==========================================

// 23 - Sortiranje: comparator za sortiranje po kolicini silazno
// 26 - Pokazivac na funkciju: prosljedjuje se qsortu kao callback
// 6 - static: vidljivo samo unutar menu.c
static int cmp_qty_desc(const void* a,
    const void* b) {

    // 12 - Pokazivaci: void* pretvoreni u Product*
    const Product* pa = a;
    const Product* pb = b;

    return pb->quantity - pa->quantity;
}


// 6 - static: reports_menu vidljiva samo unutar menu.c
// 10 - Podizbornik za izvjestaje
// 13 - Funkcija: generira razlicite izvjestaje o skladistu
static void reports_menu(ProductArray* products,
    SupplierArray* suppliers) {

    int running = 1;

    while (running) {

        printf("\n--- Izvjestaji ---\n");
        printf("1. Top 10 po kolicini\n");
        printf("2. Niska zaliha\n");
        printf("3. Broj po dobavljacu\n");
        printf("4. Po kategoriji\n");
        printf("0. Nazad\n");

        int opt = read_int("Opcija: ");

        if (opt == 1) {

            // 16 - Dinamicko zauzimanje memorije: privremena kopija niza za sortiranje
            // 17 - malloc: alociranje memorije za privremeni niz
            Product* tmp =
                malloc(products->size * sizeof(Product));

            memcpy(tmp,
                products->items,
                products->size * sizeof(Product));

            // 23 - Sortiranje: qsort s comparatorom po kolicini
            // 26 - Pokazivac na funkciju: cmp_qty_desc prosljedjen qsortu
            qsort(tmp,
                products->size,
                sizeof(Product),
                cmp_qty_desc);

            size_t limit =
                products->size < 10 ? products->size : 10;

            for (size_t i = 0; i < limit; ++i)
                product_print(&tmp[i]);

            // 17 - free: oslobadjanje privremene memorije
            // 18 - Oslobadjanje odmah nakon upotrebe (nema dangling pointera - tmp je lokalan)
            free(tmp);
        }

        else if (opt == 2) {

            int t = read_int("Prag: ");

            for (size_t i = 0; i < products->size; ++i)
                if (products->items[i].quantity < t)
                    product_print(&products->items[i]);
        }

        else if (opt == 3) {

            for (size_t i = 0; i < suppliers->size; ++i) {

                int count = 0;

                for (size_t j = 0; j < products->size; ++j)
                    if (products->items[j].supplierId ==
                        suppliers->items[i].id)
                        count++;

                printf("%s -> %d proizvoda\n",
                    suppliers->items[i].name,
                    count);
            }
        }

        else if (opt == 4) {

            // 11 - Enum: CAT_COUNT za petlju kroz sve kategorije
            for (int c = 0; c < CAT_COUNT; ++c) {

                printf("\n%s:\n",
                    category_get_name(c));

                for (size_t i = 0; i < products->size; ++i)
                    if (products->items[i].categoryId == c)
                        product_print(&products->items[i]);
            }
        }

        else if (opt == 0) {
            running = 0;
        }
    }
}