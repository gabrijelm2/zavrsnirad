#include "header.h"

// Pomocne funkcije za unos
static int unesi_tekst(const char* prompt, char* buf, size_t velicina) {
    if (!prompt || !buf || velicina == 0) return 0;
    printf("%s", prompt);
    if (!fgets(buf, (int)velicina, stdin)) return 0;
    buf[strcspn(buf, "\n")] = '\0';
    return 1;
}

static int unesi_int(const char* prompt, int* out) {
    char buf[64];
    char* end = NULL;
    long val;

    if (!prompt || !out) return 0;

    for (;;) {
        printf("%s", prompt);
        if (!fgets(buf, sizeof(buf), stdin)) return 0;

        errno = 0;
        val = strtol(buf, &end, 10);

        if (end == buf || (*end != '\n' && *end != '\0') || errno != 0) {
            printf("Neispravan unos, pokusaj ponovno.\n");
            continue;
        }

        *out = (int)val;
        return 1;
    }
}

static int unesi_double(const char* prompt, double* out) {
    char buf[64];
    char* end = NULL;
    double val;

    if (!prompt || !out) return 0;

    for (;;) {
        printf("%s", prompt);
        if (!fgets(buf, sizeof(buf), stdin)) return 0;

        errno = 0;
        val = strtod(buf, &end);

        if (end == buf || (*end != '\n' && *end != '\0') || errno != 0) {
            printf("Neispravan unos, pokusaj ponovno.\n");
            continue;
        }

        *out = val;
        return 1;
    }
}

static void ispisi_izbornik(void) {
    printf("\n=========== \n1.Dodaj Proizvod \n2.Ispisi Proizvod \n3.Azuriraj Proizvod \n4.Obrisi Proizvod \n5.Dodaj Dobavljaca \n6.Trazi Proizvod \n7.Sort Proizvod \n8.Backup \n9.CSV \n0.Izlaz \n========== \nOdabir: ");
}

int main(void) {
    ProductArray pNiz;
    SupplierArray sNiz;
    const char* BAZA = "skladiste.bin";

    p_init(&pNiz);
    s_init(&sNiz);

    if (datoteka_postoji(BAZA)) {
        long vel = dohvati_velicinu_datoteke(BAZA);
        if (vel >= 0 && globalnaKonfiguracija.verbose)
            printf("Velicina ucitane baze: %ld bajtova.\n", vel);

        if (ucitaj_sve(BAZA, &pNiz, &sNiz) != 0)
            printf("Greska pri ucitavanju baze.\n");
    }

    int odabir = -1;
    char unosBuffer[64];

    while (odabir != OPT_IZLAZ) {
        ispisi_izbornik();

        if (!fgets(unosBuffer, sizeof(unosBuffer), stdin) ||
            sscanf(unosBuffer, "%d", &odabir) != 1) {
            printf("Neispravan odabir.\n");
            continue;
        }

        if (odabir == OPT_DODAJ_P) {
            Product p;
            memset(&p, 0, sizeof(p));

            for (;;) {
                int id;
                if (!unesi_int("Unesi ID proizvoda: ", &id)) break;
                if (p_id_postoji(&pNiz, id))
                    printf("Greska: Taj ID vec postoji!\n");
                else { p.id = id; break; }
            }

            for (;;) {
                if (!unesi_tekst("Unesi ime proizvoda: ", p.ime, sizeof(p.ime))) break;
                if (p_ime_postoji(&pNiz, p.ime))
                    printf("Greska: To ime vec postoji!\n");
                else break;
            }

            int kat;
            for (;;) {
                if (!unesi_int("Unesi kategoriju (0=Stapovi,1=Role,2=Najlon): ", &kat)) continue;
                if (!JE_VALJANA_KAT(kat)) {
                    printf("Neispravna kategorija.\n");
                    continue;
                }
                p.kategorijaId = kat;
                break;
            }

            if (!unesi_double("Unesi cijenu: ", &p.cijena)) continue;
            if (!unesi_int("Unesi kolicinu: ", &p.kolicina)) continue;

            p.dobavljacId = 1;

            if (p_add(&pNiz, &p) == 0)
                printf("Proizvod dodan!\n");
            else
                printf("Greska pri dodavanju proizvoda!\n");
        }

        else if (odabir == OPT_ISPISI_P) {
            if (pNiz.size == 0) printf("Nema proizvoda.\n");
            else {
                for (size_t i = 0; i < pNiz.size; i++) {
                    printf("ID:%d | %s | Kat:%d | Kol:%d | Cijena:%.2f | Dob:%d\n",
                        pNiz.items[i].id,
                        pNiz.items[i].ime,
                        pNiz.items[i].kategorijaId,
                        pNiz.items[i].kolicina,
                        pNiz.items[i].cijena,
                        pNiz.items[i].dobavljacId);
                }
            }
        }

        else if (odabir == OPT_AZURIRAJ_P) {
            if (pNiz.size == 0) { printf("Nema proizvoda.\n"); continue; }

            qsort(pNiz.items, pNiz.size, sizeof(Product), usporedi_proizvode);

            int id;
            if (!unesi_int("Unesi ID za izmjenu: ", &id)) continue;

            int idx = p_search_rekurzivni(pNiz.items, 0, (int)pNiz.size - 1, id);
            if (idx == -1) { printf("Nije pronaden.\n"); continue; }

            unesi_double("Nova cijena: ", &pNiz.items[idx].cijena);
            unesi_int("Nova kolicina: ", &pNiz.items[idx].kolicina);

            obnovi_datoteku_na_disku(BAZA, &pNiz, &sNiz);
            printf("Azurirano!\n");
        }

        else if (odabir == OPT_OBRISI_P) {
            if (pNiz.size == 0) { printf("Nema proizvoda.\n"); continue; }

            qsort(pNiz.items, pNiz.size, sizeof(Product), usporedi_proizvode);

            int id;
            if (!unesi_int("Unesi ID za brisanje: ", &id)) continue;

            int idx = p_search_rekurzivni(pNiz.items, 0, (int)pNiz.size - 1, id);
            if (idx == -1) { printf("Nije pronaden.\n"); continue; }

            for (size_t i = idx; i < pNiz.size - 1; i++)
                pNiz.items[i] = pNiz.items[i + 1];

            pNiz.size--;

            obnovi_datoteku_na_disku(BAZA, &pNiz, &sNiz);
            printf("Obrisano!\n");
        }

        else if (odabir == OPT_DODAJ_D) {
            Supplier s;
            memset(&s, 0, sizeof(s));

            for (;;) {
                int id;
                if (!unesi_int("Unesi ID dobavljaca: ", &id)) break;
                if (s_id_postoji(&sNiz, id))
                    printf("ID vec postoji!\n");
                else { s.id = id; break; }
            }

            for (;;) {
                if (!unesi_tekst("Unesi ime dobavljaca: ", s.ime, sizeof(s.ime))) break;
                if (s_ime_postoji(&sNiz, s.ime))
                    printf("Ime vec postoji!\n");
                else break;
            }

            unesi_tekst("Unesi kontakt: ", s.kontakt, sizeof(s.kontakt));

            if (s_add(&sNiz, &s) == 0)
                printf("Dobavljac dodan!\n");
            else
                printf("Greska pri dodavanju dobavljaca!\n");
        }

        else if (odabir == OPT_TRAZI_P) {
            if (pNiz.size == 0) { printf("Nema proizvoda.\n"); continue; }

            qsort(pNiz.items, pNiz.size, sizeof(Product), usporedi_proizvode);

            int id;
            if (!unesi_int("Unesi ID: ", &id)) continue;

            int idx = p_search_rekurzivni(pNiz.items, 0, (int)pNiz.size - 1, id);
            if (idx == -1) printf("Nije pronaden.\n");
            else printf("Pronaden: %s\n", pNiz.items[idx].ime);
        }

        else if (odabir == OPT_SORT_P) {
            qsort(pNiz.items, pNiz.size, sizeof(Product), usporedi_proizvode);
            printf("Sortirano!\n");
        }

        else if (odabir == OPT_BACKUP) {
            if (kopiraj_datoteku(BAZA, "backup.bin") == 0)
                printf("Backup OK!\n");
            else
                printf("Backup nije uspio.\n");
        }

        else if (odabir == OPT_CSV_EXPORT) {
            if (export_csv("skladiste.csv", &pNiz) == 0)
                printf("CSV izvezen u 'skladiste.csv'!\n");
            else
                printf("Greska pri CSV izvozu!\n");
        }
    }

    spasi_sve(BAZA, &pNiz, &sNiz);
    p_free(&pNiz);
    s_free(&sNiz);

    return 0;
}