#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
typedef struct radnik_t {
    char *ime;
    int id;
    struct radnik_t *sledeci;
} radnik;
typedef struct artikal_t {
    char *ime;
    int sifra, zalihe;
    float cena;
    struct artikal_t *sledeci;
} artikal;
typedef struct stavka_t {
    artikal *art;
    float ukupna_cena;
    int kolicina;
    struct stavka_t *sledeci;
} stavka;
bool nadji_artikal(int);
void nadji_artikal_sifra(void);
void unesi_ime(char *);
bool ukloni_artikal(void);
void sacuvaj_artikle(void);
bool dodaj_radnika(int);
bool nadji_radnika(int);
bool hoces_neces(char *);
FILE *radnici, *artikli;
const size_t radnik_s = sizeof(radnik), artikal_s = sizeof(artikal), stavka_s = sizeof(stavka);
artikal *prvi_artikal = NULL, *trenutni_artikal = NULL, *prethodni_artikal = NULL, *t_artikal;
radnik *pocetak_radnik, *trenutni_radnik, *t_radnik, *prethodni_radnik;
bool menjao_artikle = false, menjao_radnike = false;
char *putanja = NULL, *ime = NULL;
int main(void) {
    int i, ii, iii, iiii, podesavanja_stampe = 1;
    float f;
    char *stampaj = NULL;
    bool menjao_podesavanja = false;
    FILE *stampa_strana, *podesavanja;
    stavka *prva_stavka, *t_stavka, *racun;
    pocetak_radnik = trenutni_radnik = malloc(radnik_s);
    pocetak_radnik->id = 0;
    pocetak_radnik->ime = malloc(6 * sizeof(char));
    strcpy(pocetak_radnik->ime, "Gazda");
    trenutni_radnik->sledeci = NULL;
    putanja = malloc(261);
    ime = malloc(261);
    getcwd(putanja, 260);
    strcpy(ime, putanja);
    strcat(ime, "/artikli.txt");
    if((artikli = fopen(ime, "r")) != NULL) {
        for(ii = 0; fscanf(artikli, "%s %f %d %d", ime, &f, &i, &iii) != EOF; ii++) {
            if(ii == 0)
                prvi_artikal = trenutni_artikal = malloc(artikal_s);
            else {
                trenutni_artikal->sledeci = malloc(artikal_s);
                trenutni_artikal = trenutni_artikal->sledeci;
            }
            trenutni_artikal->cena = f;
            trenutni_artikal->zalihe = i;
            trenutni_artikal->sifra = iii;
            trenutni_artikal->ime = malloc(strlen(ime) * sizeof(char));
            strcpy(trenutni_artikal->ime, ime);
        }
        if(trenutni_artikal != NULL)
            trenutni_artikal->sledeci = NULL;
        fclose(artikli);
    }
    strcpy(ime, putanja);
    strcat(ime, "/radnici.txt");
    if((radnici = fopen(ime, "r")) != NULL) {
        while(fscanf(radnici, "%s %d", ime, &i) != EOF)
            dodaj_radnika(i);
        fclose(radnici);
    }
    strcpy(ime, putanja);
    strcat(ime, "/podesavanja.txt");
    if((podesavanja = fopen(ime, "r")) != NULL) {
        stampaj = malloc(261 * sizeof(char));
        fgets(stampaj, 260, podesavanja);
        stampaj[strlen(stampaj) - 1] = '\n';
        fscanf(podesavanja, "%d", &podesavanja_stampe);
        if(strcmp(stampaj, "NULL") == 0) {
            free(stampaj);
            stampaj = NULL;
        }
        fclose(podesavanja);
    } else
        menjao_podesavanja = true;
    ULAZ:
    do {
        if(pocetak_radnik->sledeci != NULL) {
            fputs("Unesite ID radnika: ", stdout);
            scanf("%d", &i);
            if(i < 0)
                goto KRAJ;
            if(nadji_radnika(i)) {
                trenutni_radnik = t_radnik;
                break;
            }
        } else
            puts("---\nPostoji samo jedan radnik.");
    } while(true);
    printf("---\nDobro dosli, %s\n", trenutni_radnik->ime);
    while(true) {
        fputs("---\n1. Zalihe robe\n2. Informacije o artiklu\n3. Kasa\n4. Azuriranje robe\n5. Promeni korisnika\n"
              "6. Sacuvaj izmene artikala\n7. Azuriraj radnike\n8. Podesavanja stampe\n9. O programu\n10. Izlaz\n > ", stdout);
        scanf("%d", &i);
        switch(i) {
            case 1:
                if(prvi_artikal == NULL) {
                    puts("---\nNema robe na raspolaganju.");
                    break;
                }
                puts("---\nArtikal | Sifra | Cena | Kolicina\n");
                for(trenutni_artikal = prvi_artikal; trenutni_artikal != NULL; trenutni_artikal = trenutni_artikal->sledeci)
                    printf("%s | %d | %.2f | %d\n", trenutni_artikal->ime, trenutni_artikal->sifra, trenutni_artikal->cena, trenutni_artikal->zalihe);
                break;
            case 2:
                nadji_artikal_sifra();
                break;
            case 3:
                if(prvi_artikal == NULL) {
                    puts("---\nNema artikala u ponudi.");
                    break;
                }
                f = 0.0;
                prva_stavka = racun = malloc(stavka_s);
                prva_stavka->sledeci = NULL;
                puts("---");
                while(true) {
                    fputs("Unesite sifru artikla: ", stdout);
                    scanf("%d", &i);
                    if(i < 0) {
                        if(f == 0.0)
                            break;
                        free(racun);
                        racun = t_stavka;
                        racun->sledeci = NULL;
                        i = podesavanja_stampe == 3 ? hoces_neces("Da li zelite da stampate racun? y/n") : podesavanja_stampe - 1;
                        if(i) {
                            strcpy(ime, putanja);
                            strcat(ime, "/stampa.txt");
                            stampa_strana = fopen(ime, "w");
                        } else
                            stampa_strana = stdout;
                        fprintf(stampa_strana, "Radnik: %s\nArtikal | Kolicina | Cena po komadu | Ukupna cena\n\n", trenutni_radnik->ime);
                        for(racun = prva_stavka; racun != NULL; racun = racun->sledeci) {
                            fprintf(stampa_strana, "%s | %d | %.2f | %.2f\n", racun->art->ime, racun->kolicina, racun->art->cena, racun->ukupna_cena);
                            racun->art->zalihe -= racun->kolicina;

                        }
                        fprintf(stampa_strana, "\nUkupno: %.2f\n", f);
                        menjao_artikle = true;
                        if(i) {
                            fclose(stampa_strana);
                            puts(stampaj);
                            system(stampaj);
                        }
                        break;
                    }
                    if(nadji_artikal(i)) {
                        if(trenutni_artikal->zalihe == 0) {
                            puts("\nArtikal nije dostupan.\n");
                            continue;
                        }
                        printf("\t%s %.2f\n", trenutni_artikal->ime, trenutni_artikal->cena);
                        fputs("Kolicina: ", stdout);
                        scanf("%d", &ii);
                        if(ii <= 0)
                            continue;
                        if(ii > trenutni_artikal->zalihe) {
                            printf("Na stanju ima samo %d komada.", trenutni_artikal->zalihe);
                            continue;
                        }
                        f += trenutni_artikal->cena * (float) ii;
                        racun->art = trenutni_artikal;
                        racun->kolicina = ii;
                        racun->ukupna_cena = racun->art->cena * (float) racun->kolicina;
                        t_stavka = racun;
                        racun->sledeci = malloc(stavka_s);
                        racun = racun->sledeci;
                    }
                }
                for(racun = prva_stavka; racun != NULL; racun = t_stavka) {
                    t_stavka = racun->sledeci;
                    free(racun);
                }
                break;
            case 4:
                puts("---\n1. Ukloni artikle kojih nema u zalihama\n2. Azuriraj arikal\n3. Ukloni artikal\n4. Novi artikal");
                scanf("%d", &i);
                switch(i) {
                    case 1:
                        for(trenutni_artikal = prvi_artikal; trenutni_artikal != NULL; prethodni_artikal = trenutni_artikal, trenutni_artikal = trenutni_artikal->sledeci)
                            if(trenutni_artikal->zalihe == 0)
                                ukloni_artikal();
                        break;
                    case 2:
                        nadji_artikal_sifra();
                        if(trenutni_artikal == NULL)
                            break;
                        fputs("1. Promena imena\n2. Promena sifre\n3. Promena cene\n4. Azuriraj zalihe\n > ", stdout);
                        scanf("%d", &i);
                        switch(i) {
                            case 1:
                                free(trenutni_artikal->ime);
                                unesi_ime(trenutni_artikal->ime);
                                menjao_artikle = true;
                                break;
                            case 2:
                                fputs("Nova sifra artikla: ", stdout);
                                scanf("%d", &i);
                                for(t_artikal = prvi_artikal; t_artikal != NULL; t_artikal = t_artikal->sledeci)
                                    if(t_artikal->sifra == i) {
                                        puts("Artikal sa tom sifrom vec postoji.");
                                        goto AZURIRAO_ARTIKAL;
                                    }
                                trenutni_artikal->sifra = i;
                                menjao_artikle = true;
                                break;
                            case 3:
                                fputs("Unesite cenu artikla: ", stdout);
                                scanf("%f", &f);
                                if(f > 0) {
                                    trenutni_artikal->cena = f;
                                    menjao_artikle = true;
                                } else
                                    puts("Nevazeca vrednost.");
                                break;
                            case 4:
                                fputs("Unesite zalihe artikla: ", stdout);
                                scanf("%d", &i);
                                if(i > 0) {
                                    trenutni_artikal->zalihe = i;
                                    menjao_artikle = true;
                                } else
                                    puts("Nevazeca vrednost.");
                                break;
                            default:
                                puts("---\nNeispravno uneta komanda");
                        }
                        AZURIRAO_ARTIKAL:
                        break;
                    case 3:
                        if(prvi_artikal == NULL) {
                            puts("---\nNema artikala na raspolaganju");
                            break;
                        }
                        nadji_artikal_sifra();
                        ukloni_artikal();
                        break;
                    case 4:
                        SIFRA_NOVI_ARTIKAL:
                        fputs("Unesite sifru novog artikla: ", stdout);
                        scanf("%d", &iiii);
                        if(i < 0)
                            break;
                        for(trenutni_artikal = prvi_artikal; trenutni_artikal != NULL; t_artikal = trenutni_artikal, trenutni_artikal = trenutni_artikal->sledeci)
                            if(trenutni_artikal->sifra == i) {
                                puts("Artikal sa datom sifrom vec postoji.");
                                goto SIFRA_NOVI_ARTIKAL;
                            }
                        unesi_ime(ime);
                        fputs("Unesite cenu novog artikla: ", stdout);
                        scanf("%f", &f);
                        if(f < 0)
                            break;
                        fputs("Unesite trenutne zalihe novog artikla: ", stdout);
                        scanf("%d", &ii);
                        if(ii < 0)
                            break;
                        trenutni_artikal = t_artikal;
                        if(prvi_artikal == NULL)
                            prvi_artikal = trenutni_artikal = malloc(artikal_s);
                        else {
                            trenutni_artikal->sledeci = malloc(artikal_s);
                            trenutni_artikal = trenutni_artikal->sledeci;
                        }
                        trenutni_artikal->sledeci = NULL;
                        trenutni_artikal->cena = f;
                        trenutni_artikal->zalihe = ii;
                        trenutni_artikal->sifra = iiii;
                        strcpy(trenutni_artikal->ime, ime);
                        menjao_artikle = true;
                        break;
                }
                break;
            case 5:
                sacuvaj_artikle();
                goto ULAZ;
            case 6:
                sacuvaj_artikle();
                break;
            case 7:
                if(trenutni_radnik->id != 0) {
                    puts("Samo gazda ima pristup ovim podesavanjima.");
                    break;
                }
                puts("---\n1. Dodaj radnika\n2. Ukloni radnika\n3. Azuriraj informacije o radniku");
                scanf("%d", &i);
                switch(i) {
                    case 1:
                        fputs("Unesite ID radnika: ", stdout);
                        scanf("%d", &i);
                        unesi_ime(ime);
                        dodaj_radnika(i);
                        break;
                    case 2:
                        fputs("Unesite ID radnika: ", stdout);
                        scanf("%d", &i);
                        if(i == 0)
                            puts("Nije moguce obrisati gazdin nalog.");
                        else if(nadji_radnika(i)) {
                            prethodni_radnik->sledeci = t_radnik->sledeci == NULL ? NULL : t_radnik->sledeci;
                            free(t_radnik);
                        }
                        break;
                    case 3:
                        fputs("Unesite ID radnika: ", stdout);
                        scanf("%d", &i);
                        if(nadji_radnika(i)) {
                            puts("---\n1. Ime\n2. ID");
                            scanf("%d", &i);
                            switch(i) {
                                case 1:
                                    free(t_radnik->ime);
                                    unesi_ime(t_radnik->ime);
                                    menjao_radnike = true;
                                    break;
                                case 2:
                                    fputs("Unesite ID radnika: ", stdout);
                                    scanf("%d", &i);
                                    if(nadji_radnika(i))
                                        puts("Radnik sa datim ID-jem vec postoji.");
                                    else
                                        t_radnik->id = i;
                                    break;
                            }
                        } else
                            puts("---\nNe postoji radnik sa datim ID-jem.");
                        break;
                }
                break;
            case 8:
                puts("1. Komanda za stampanje\n2. Podrazumevana podesavanja\n3. Odstampaj probni list");
                scanf("%d", &i);
                getchar();
                switch(i) {
                    case 1:
                        if(stampaj == NULL)
                            stampaj = malloc(261 * sizeof(char));
                        puts("Unesite komandu za stampanje: ");
                        fgets(stampaj, 260, stdin);
                        stampaj[strlen(stampaj) - 1] = '\0';
                        menjao_podesavanja = true;
                        break;
                    case 2:
                        puts("1. Nikad ne stampaj racun\n2. Uvek stampaj racun\n3. Uvek pitaj");
                        scanf("%d", &i);
                        if(i != 1 && i != 2 && i != 3)
                            puts("Pogresna vrednost");
                        else
                            podesavanja_stampe = i;
                        menjao_podesavanja = true;
                        break;
                    case 3:
                        if(stampaj == NULL) {
                            puts("Nema komande za stampu.");
                            break;
                        }
                        strcpy(ime, putanja);
                        strcat(ime, "/stampa.txt");
                        stampa_strana = fopen(ime, "w");
                        fputs("Test strana\n---\nRegistar v1.0\nBorisav Zivanovic 2017\ngithub.com/borisavz/registar\nborisavzivanovic@gmail.com", stampa_strana);
                        fclose(stampa_strana);
                        system(stampaj);
                        break;
                }
                break;
            case 9:
                puts("---\nRegistar v1.0\nBorisav Zivanovic 2017");
                break;
            case 10:
                if(hoces_neces("---\nDa li ste sigurni da zelite da izadjete? y/n"))
                    goto KRAJ;
            default:
                puts("---\nNeispravno uneta komanda");
        }
    }
    KRAJ:
    sacuvaj_artikle();
    if(!menjao_artikle)
        for(trenutni_artikal = prvi_artikal; trenutni_artikal != NULL; trenutni_artikal = t_artikal) {
            t_artikal = trenutni_artikal->sledeci;
            free(trenutni_artikal);
        }
    if(menjao_radnike) {
        strcpy(ime, putanja);
        strcat(ime, "/radnici.txt");
        radnici = fopen(ime, "w");
        if((trenutni_radnik = pocetak_radnik->sledeci) != NULL)
            while(true) {
                t_radnik = trenutni_radnik->sledeci;
                fprintf(radnici, "%s %d", trenutni_radnik->ime, trenutni_radnik->id);
                free(trenutni_radnik);
                if((trenutni_radnik = t_radnik) != NULL)
                    fputc('\n', radnici);
                else
                    break;
            }
        fclose(radnici);
    } else
        for(trenutni_radnik = pocetak_radnik; trenutni_radnik != NULL; trenutni_radnik = t_radnik) {
            t_radnik = trenutni_radnik->sledeci;
            free(trenutni_radnik->ime);
            free(trenutni_radnik);
        }
    if(menjao_podesavanja) {
        strcpy(ime, putanja);
        strcat(ime, "/podesavanja.txt");
        podesavanja = fopen(ime, "w");
        fprintf(podesavanja, "%s\n%d", stampaj == NULL ? "NULL" : stampaj, podesavanja_stampe);
        fclose(podesavanja);
    }
    if(putanja != NULL)
        free(putanja);
    if(ime != NULL)
        free(ime);
    if(stampaj != NULL)
        free(stampaj);
    return 0;
}
bool nadji_artikal(int sifra) {
    for(trenutni_artikal = prvi_artikal; trenutni_artikal != NULL; prethodni_artikal = trenutni_artikal, trenutni_artikal = trenutni_artikal->sledeci)
        if(trenutni_artikal->sifra == sifra)
            return true;
    return false;
}
void nadji_artikal_sifra() {
    int s;
    if(prvi_artikal == NULL) {
        trenutni_artikal = NULL;
        puts("---\nNema artikala na raspolaganju.");
        return;
    }
    fputs("---\nUnesite sifru artikla: ", stdout);
    scanf("%d", &s);
    if(nadji_artikal(s))
        printf("\nArtikal | Cena | Kolicina\n\n%s | %.2f | %d\n", trenutni_artikal->ime, trenutni_artikal->cena, trenutni_artikal->zalihe);
    else
        puts("Nepostojeci artikal.");
}
void unesi_ime(char *u_ime) {
    int i = 0;
    fputs("Unesite ime: ", stdout);
    getchar();
    fgets(ime, 260, stdin);
    while(ime[i++] != '\n')
        if(ime[i] == ' ')
            ime[i] = '_';
    ime[i - 1] = '\0';
    u_ime = malloc(i * sizeof(char));
    strcpy(u_ime, ime);
}
bool ukloni_artikal() {
    if((t_artikal = trenutni_artikal) == NULL)
        return false;
    if(trenutni_artikal == prvi_artikal)
        prvi_artikal = prvi_artikal->sledeci;
    else if(trenutni_artikal->sledeci == NULL)
        trenutni_artikal = prethodni_artikal;
    else
        prethodni_artikal->sledeci = trenutni_artikal->sledeci;
    free(t_artikal->ime);
    free(t_artikal);
    return menjao_artikle = true;
}
void sacuvaj_artikle() {
    if(menjao_artikle) {
        strcpy(ime, putanja);
        strcat(ime, "/artikli.txt");
        artikli = fopen(ime, "w");
        if(prvi_artikal == NULL) {
            fclose(artikli);
            return;
        }
        trenutni_artikal = prvi_artikal;
        while(true) {
            t_artikal = trenutni_artikal->sledeci;
            fprintf(artikli, "%s %f %d %d", trenutni_artikal->ime, trenutni_artikal->cena, trenutni_artikal->zalihe, trenutni_artikal->sifra);
            free(trenutni_artikal);
            if((trenutni_artikal = t_artikal) != NULL)
                fputc('\n', artikli);
            else
                break;
        }
        fclose(artikli);
    }
}
bool dodaj_radnika(int id) {
    for(t_radnik = pocetak_radnik; t_radnik != NULL; trenutni_radnik = t_radnik, t_radnik = t_radnik->sledeci)
        if(t_radnik->id == id)
            return false;
    trenutni_radnik->sledeci = malloc(radnik_s);
    trenutni_radnik = trenutni_radnik->sledeci;
    trenutni_radnik->ime = malloc(strlen(ime) * sizeof(char));
    strcpy(trenutni_radnik->ime, ime);
    trenutni_radnik->id = id;
    trenutni_radnik->sledeci = NULL;
    return menjao_radnike = true;
}
bool nadji_radnika(int id) {
    if(pocetak_radnik->sledeci == NULL)
        return false;
    for(t_radnik = pocetak_radnik; t_radnik != NULL; prethodni_radnik = t_radnik, t_radnik = t_radnik->sledeci)
        if(t_radnik->id == id)
            return true;
    return false;
}
bool hoces_neces(char *poruka) {
    char c;
    puts(poruka);
    do {
        fputs(" > ", stdout);
        getchar();
        c = getchar();
        getchar();
    } while(c != 'y' && c != 'n');
    return c == 'y';
}