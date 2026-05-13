/*
  menu.c
  Izbornici i interakcija s korisnikom
*/

#include "header.h"

static void products_menu(ProductArray* products, SupplierArray* suppliers);
static void suppliers_menu(SupplierArray* suppliers);
static void reports_menu(ProductArray* products, SupplierArray* suppliers);
static int  cmp_qty_desc(const void* a, const void* b);

void main_menu(ProductArray* products, SupplierArray* suppliers, const char* dataFile) {
    int running = 1;
    while (running) {
        printf("\n=== Skladiste ribolovne opreme ===\n");
        printf("1. Upravljanje proizvodima\n");
        printf("2. Upravljanje dobavljacima\n");
        printf("3. Izvjestaji\n");
        printf("4. Generiraj testne podatke (samo na zahtjev)\n");
        printf("8. Backup datoteke\n");
        printf("9. Spremi podatke\n");
        printf("0. Izlaz\n");

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
        case OPT_GENERATE_TEST: {
            int pn = read_int("Koliko proizvoda generirati? (npr. 200): ");
            int sn = read_int("Koliko dobavljaca generirati? (npr. 20): ");
            if (pn > 0) {
                if (product_generate_test_data(products, (size_t)pn) == 0)
                    printf("Generirano %d proizvoda.\n", pn);
                else
                    printf("Greska pri generiranju proizvoda.\n");
            }
            if (sn > 0) {
                if (supplier_generate_test_data(suppliers, (size_t)sn) == 0)
                    printf("Generirano %d dobavljaca.\n", sn);
                else
                    printf("Greska pri generiranju dobavljaca.\n");
            }
            break;
        }
        case OPT_BACKUP: {
            char dst[256];
            read_string("Unesi ime backup datoteke: ", dst, sizeof(dst));
            if (file_copy(dataFile, dst) == 0)
                printf("Backup uspjesan.\n");
            else
                printf("Backup nije uspio.\n");
            break;
        }
        case OPT_SAVE:
            if (fileio_save_all(dataFile, products, suppliers) == 0)
                printf("Podaci spremljeni.\n");
            else
                printf("Greska pri spremanju.\n");
            break;
        case OPT_EXIT:
            running = 0;
            break;
        default:
            printf("Nepoznata opcija.\n");
        }
    }
}

static void products_menu(ProductArray* products, SupplierArray* suppliers) {
    (void)suppliers;
    int running = 1;
    while (running) {
        printf("\n--- Proizvodi ---\n");
        printf("1. Dodaj proizvod\n");
        printf("2. Prikazi sve proizvode\n");
        printf("3. Pretrazi proizvod po ID\n");
        printf("4. Izmijeni proizvod\n");
        printf("5. Obrisi proizvod\n");
        printf("6. Sortiraj po imenu\n");
        printf("7. Sortiraj po ID\n");
        printf("0. Nazad\n");

        int opt = read_int("Opcija: ");

        if (opt == 1) {
            Product p = { 0 };
            p.id = read_int("Unesi ID: ");
            if (product_id_exists(products, p.id)) {
                printf("Greska: ID %d je vec unesen.\n", p.id);
                continue;
            }
            read_string("Unesi naziv: ", p.name, sizeof(p.name));
            if (product_name_exists(products, p.name)) {
                printf("Greska: Naziv '%s' je vec unesen.\n", p.name);
                continue;
            }
            print_all_categories();
            int catChoice = read_int("Odaberi kategoriju: ");
            if (!is_valid_category(catChoice)) {
                printf("Greska: Neispravna kategorija.\n");
                continue;
            }
            p.categoryId = catChoice;
            p.price = read_double("Unesi cijenu: ");
            p.quantity = read_int("Unesi kolicinu: ");
            p.supplierId = read_int("Unesi ID dobavljaca: ");
            int rc = product_add(products, &p);
            if (rc == 0) { printf("Proizvod dodan uspjesno!\n"); product_print(&p); }
            else if (rc == -2) printf("Greska: ID vec postoji.\n");
            else if (rc == -3) printf("Greska: Naziv vec postoji.\n");
            else               printf("Greska pri dodavanju.\n");
        }
        else if (opt == 2) { product_print_all(products); }
        else if (opt == 3) {
            int      id = read_int("Unesi ID: ");
            Product* p = product_find_by_id(products, id);
            if (p) product_print(p); else printf("Nije pronadeno.\n");
        }
        else if (opt == 4) {
            int      id = read_int("Unesi ID za izmjenu: ");
            Product* p = product_find_by_id(products, id);
            if (!p) { printf("Nema takvog proizvoda.\n"); continue; }
            Product tmp = *p;
            printf("\nTrenutni podaci:\n"); product_print(p); printf("\n");
            char buf[128];
            read_string("Unesi novi naziv (enter za ostaviti): ", buf, sizeof(buf));
            if (strlen(buf) > 0) strncpy(tmp.name, buf, sizeof(tmp.name) - 1);
            print_all_categories();
            int newcat = read_int("Unesi novu kategoriju (-1 za ostaviti): ");
            if (newcat >= 0 && is_valid_category(newcat)) tmp.categoryId = newcat;
            double newprice = read_double("Unesi novu cijenu (0 za ostaviti): ");
            if (newprice > 0.0) tmp.price = newprice;
            int newq = read_int("Unesi novu kolicinu (-1 za ostaviti): ");
            if (newq >= 0) tmp.quantity = newq;
            int newsup = read_int("Unesi novi ID dobavljaca (-1 za ostaviti): ");
            if (newsup >= 0) tmp.supplierId = newsup;
            if (product_update(products, id, &tmp) == 0) {
                printf("Azurirano uspjesno!\n"); product_print(&tmp);
            }
            else { printf("Greska pri azuriranju.\n"); }
        }
        else if (opt == 5) {
            int      id = read_int("Unesi ID za brisanje: ");
            Product* p = product_find_by_id(products, id);
            if (p) {
                product_print(p);
                int confirm = read_int("Sigurno zelite obrisati? (1=DA, 0=NE): ");
                if (confirm == 1 && product_delete(products, id) == 0) printf("Obrisano.\n");
                else printf("Brisanje otkazano.\n");
            }
            else { printf("Nije moguce obrisati.\n"); }
        }
        else if (opt == 6) {
            if (products->size > 0) {
                qsort(products->items, products->size, sizeof(Product), product_compare_by_name);
                printf("Sortirano po imenu.\n");
            }
            else { printf("Nema proizvoda.\n"); }
        }
        else if (opt == 7) {
            if (products->size > 0) {
                qsort(products->items, products->size, sizeof(Product), product_compare_by_id);
                printf("Sortirano po ID.\n");
            }
            else { printf("Nema proizvoda.\n"); }
        }
        else if (opt == 0) { running = 0; }
        else { printf("Nepoznata opcija.\n"); }
    }
}

static void suppliers_menu(SupplierArray* suppliers) {
    int running = 1;
    while (running) {
        printf("\n--- Dobavljaci ---\n");
        printf("1. Dodaj dobavljaca\n");
        printf("2. Prikazi sve dobavljace\n");
        printf("3. Pretrazi po ID\n");
        printf("4. Izmijeni dobavljaca\n");
        printf("5. Obrisi dobavljaca\n");
        printf("0. Nazad\n");

        int opt = read_int("Opcija: ");

        if (opt == 1) {
            Supplier s = { 0 };
            s.id = read_int("Unesi ID: ");
            if (supplier_id_exists(suppliers, s.id)) {
                printf("Greska: ID %d je vec unesen.\n", s.id); continue;
            }
            read_string("Unesi naziv: ", s.name, sizeof(s.name));
            if (supplier_name_exists(suppliers, s.name)) {
                printf("Greska: Naziv '%s' je vec unesen.\n", s.name); continue;
            }
            read_string("Unesi kontakt: ", s.contact, sizeof(s.contact));
            int rc = supplier_add(suppliers, &s);
            if (rc == 0) { printf("Dobavljac dodan uspjesno!\n"); supplier_print(&s); }
            else if (rc == -2) printf("Greska: ID vec postoji.\n");
            else if (rc == -3) printf("Greska: Naziv vec postoji.\n");
            else               printf("Greska pri dodavanju.\n");
        }
        else if (opt == 2) { supplier_print_all(suppliers); }
        else if (opt == 3) {
            int       id = read_int("Unesi ID: ");
            Supplier* s = supplier_find_by_id(suppliers, id);
            if (s) supplier_print(s); else printf("Nije pronadeno.\n");
        }
        else if (opt == 4) {
            int       id = read_int("Unesi ID za izmjenu: ");
            Supplier* s = supplier_find_by_id(suppliers, id);
            if (!s) { printf("Nema takvog.\n"); continue; }
            Supplier tmp = *s;
            printf("\nTrenutni podaci:\n"); supplier_print(s); printf("\n");
            char buf[128];
            read_string("Unesi novi naziv (enter za ostaviti): ", buf, sizeof(buf));
            if (strlen(buf) > 0) strncpy(tmp.name, buf, sizeof(tmp.name) - 1);
            read_string("Unesi novi kontakt (enter za ostaviti): ", buf, sizeof(buf));
            if (strlen(buf) > 0) strncpy(tmp.contact, buf, sizeof(tmp.contact) - 1);
            if (supplier_update(suppliers, id, &tmp) == 0) {
                printf("Azurirano uspjesno!\n"); supplier_print(&tmp);
            }
            else { printf("Greska pri azuriranju.\n"); }
        }
        else if (opt == 5) {
            int       id = read_int("Unesi ID za brisanje: ");
            Supplier* s = supplier_find_by_id(suppliers, id);
            if (s) {
                supplier_print(s);
                int confirm = read_int("Sigurno zelite obrisati? (1=DA, 0=NE): ");
                if (confirm == 1 && supplier_delete(suppliers, id) == 0) printf("Obrisano.\n");
                else printf("Brisanje otkazano.\n");
            }
            else { printf("Nije moguce obrisati.\n"); }
        }
        else if (opt == 0) { running = 0; }
        else { printf("Nepoznata opcija.\n"); }
    }
}

static int cmp_qty_desc(const void* a, const void* b) {
    const Product* pa = (const Product*)a;
    const Product* pb = (const Product*)b;
    return (pb->quantity - pa->quantity);
}

static void reports_menu(ProductArray* products, SupplierArray* suppliers) {
    int running = 1;
    while (running) {
        printf("\n--- Izvjestaji ---\n");
        printf("1. Top 10 proizvoda po kolicini\n");
        printf("2. Proizvodi s niskom zalihom (ispod praga)\n");
        printf("3. Broj proizvoda po dobavljacu\n");
        printf("4. Proizvodi po kategoriji\n");
        printf("0. Nazad\n");

        int opt = read_int("Opcija: ");

        if (opt == 1) {
            if (products->size == 0) { printf("Nema proizvoda.\n"); continue; }
            Product* tmp = (Product*)malloc(products->size * sizeof(Product));
            if (!tmp) { printf("Memorija.\n"); continue; }
            memcpy(tmp, products->items, products->size * sizeof(Product));
            qsort(tmp, products->size, sizeof(Product), cmp_qty_desc);
            size_t limit = products->size < 10 ? products->size : 10;
            printf("Top %zu proizvoda po kolicini:\n", limit);
            for (size_t i = 0; i < limit; ++i) product_print(&tmp[i]);
            free(tmp);
        }
        else if (opt == 2) {
            int threshold = read_int("Unesi prag zalihe: ");
            printf("Proizvodi s kolicinom manjom od %d:\n", threshold);
            int count = 0;
            for (size_t i = 0; i < products->size; ++i) {
                if (products->items[i].quantity < threshold) {
                    product_print(&products->items[i]); count++;
                }
            }
            if (count == 0) printf("Nema proizvoda s niskom zalihom.\n");
        }
        else if (opt == 3) {
            if (suppliers->size == 0) { printf("Nema dobavljaca.\n"); continue; }
            for (size_t i = 0; i < suppliers->size; ++i) {
                int sid = suppliers->items[i].id, count = 0;
                for (size_t j = 0; j < products->size; ++j)
                    if (products->items[j].supplierId == sid) count++;
                printf("Dobavljac ID:%d Naziv:%s -> %d proizvoda\n",
                    sid, suppliers->items[i].name, count);
            }
        }
        else if (opt == 4) {
            printf("\nProizvodi po kategoriji:\n");
            for (int cat = 0; cat < CAT_COUNT; ++cat) {
                printf("\n--- %s ---\n", category_get_name(cat));
                int count = 0;
                for (size_t i = 0; i < products->size; ++i) {
                    if (products->items[i].categoryId == cat) {
                        product_print(&products->items[i]); count++;
                    }
                }
                if (count == 0) printf("Nema proizvoda u ovoj kategoriji.\n");
            }
        }
        else if (opt == 0) { running = 0; }
        else { printf("Nepoznata opcija.\n"); }
    }
}
