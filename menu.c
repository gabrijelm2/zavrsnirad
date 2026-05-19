/*
  menu.c
  Izbornici i interakcija s korisnikom

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

// lokalni menu za proizvode
static void products_menu(ProductArray* products,
    SupplierArray* suppliers);

// lokalni menu za dobavljace
static void suppliers_menu(SupplierArray* suppliers);

// menu za izvjestaje
static void reports_menu(ProductArray* products,
    SupplierArray* suppliers);

// comparator za sortiranje po kolicini (desc)
static int cmp_qty_desc(const void* a,
    const void* b);


// ==========================================
// GLAVNI MENU
// ==========================================

void main_menu(ProductArray* products,
    SupplierArray* suppliers,
    const char* dataFile) {

    int running = 1;

    // glavna petlja programa
    while (running) {

        printf("\n=== Skladiste ribolovne opreme ===\n");
        printf("1. Upravljanje proizvodima\n");
        printf("2. Upravljanje dobavljacima\n");
        printf("3. Izvjestaji\n");
        printf("4. Generiraj testne podatke\n");
        printf("8. Backup datoteke\n");
        printf("9. Spremi podatke\n");
        printf("0. Izlaz\n");

        // citanje izbora korisnika
        int opt = read_int("Odaberi opciju: ");

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

            char dst[256];

            read_string(
                "Unesi ime backup datoteke: ",
                dst,
                sizeof(dst)
            );

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

            if (fileio_save_all(
                dataFile,
                products,
                suppliers
            ) == 0)

                printf("Podaci spremljeni.\n");

            else
                printf("Greska pri spremanju.\n");

            break;

            // izlaz iz programa
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

            Product p = { 0 };

            p.id = read_int("ID: ");

            if (product_id_exists(products, p.id)) {
                printf("ID vec postoji.\n");
                continue;
            }

            read_string("Naziv: ", p.name, sizeof(p.name));

            if (product_name_exists(products, p.name)) {
                printf("Naziv vec postoji.\n");
                continue;
            }

            print_all_categories();

            int cat = read_int("Kategorija: ");

            if (!is_valid_category(cat)) {
                printf("Neispravna kategorija.\n");
                continue;
            }

            p.categoryId = cat;
            p.price = read_double("Cijena: ");
            p.quantity = read_int("Kolicina: ");
            p.supplierId = read_int("ID dobavljaca: ");

            int rc = product_add(products, &p);

            if (rc == 0)
                printf("Dodan proizvod.\n");
            else
                printf("Greska pri dodavanju.\n");
        }

        else if (opt == 2) {
            product_print_all(products);
        }

        else if (opt == 3) {

            int id = read_int("ID: ");

            Product* p =
                product_find_by_id(products, id);

            if (p)
                product_print(p);
            else
                printf("Nije pronadeno.\n");
        }

        else if (opt == 4) {

            int id = read_int("ID za izmjenu: ");

            Product* p =
                product_find_by_id(products, id);

            if (!p) {
                printf("Ne postoji.\n");
                continue;
            }

            Product tmp = *p;

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

            double newprice = read_double("Nova cijena (0 skip): ");
            if (newprice > 0)
                tmp.price = newprice;

            int newq = read_int("Nova kolicina (-1 skip): ");
            if (newq >= 0)
                tmp.quantity = newq;

            int newsup = read_int("Novi dobavljac (-1 skip): ");
            if (newsup >= 0)
                tmp.supplierId = newsup;

            product_update(products, id, &tmp);
        }

        else if (opt == 5) {

            int id = read_int("ID za brisanje: ");

            product_delete(products, id);
        }

        else if (opt == 6) {

            qsort(products->items,
                products->size,
                sizeof(Product),
                product_compare_by_name);

            printf("Sortirano.\n");
        }

        else if (opt == 7) {

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

            Supplier s = { 0 };

            s.id = read_int("ID: ");

            read_string("Naziv: ", s.name, sizeof(s.name));
            read_string("Kontakt: ", s.contact, sizeof(s.contact));

            supplier_add(suppliers, &s);
        }

        else if (opt == 2) {
            supplier_print_all(suppliers);
        }

        else if (opt == 3) {

            int id = read_int("ID: ");

            Supplier* s =
                supplier_find_by_id(suppliers, id);

            if (s)
                supplier_print(s);
        }

        else if (opt == 4) {

            int id = read_int("ID za izmjenu: ");

            Supplier* s =
                supplier_find_by_id(suppliers, id);

            if (!s) continue;

            Supplier tmp = *s;

            char buf[128];

            read_string("Novi naziv: ", buf, sizeof(buf));
            if (strlen(buf) > 0)
                strncpy(tmp.name, buf, sizeof(tmp.name) - 1);

            read_string("Novi kontakt: ", buf, sizeof(buf));
            if (strlen(buf) > 0)
                strncpy(tmp.contact, buf, sizeof(tmp.contact) - 1);

            supplier_update(suppliers, id, &tmp);
        }

        else if (opt == 5) {

            int id = read_int("ID za brisanje: ");

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

static int cmp_qty_desc(const void* a,
    const void* b) {

    const Product* pa = a;
    const Product* pb = b;

    return pb->quantity - pa->quantity;
}


// Top 10, zalihe, statistika po dobavljacu itd.
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

            Product* tmp =
                malloc(products->size * sizeof(Product));

            memcpy(tmp,
                products->items,
                products->size * sizeof(Product));

            qsort(tmp,
                products->size,
                sizeof(Product),
                cmp_qty_desc);

            size_t limit =
                products->size < 10 ? products->size : 10;

            for (size_t i = 0; i < limit; ++i)
                product_print(&tmp[i]);

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