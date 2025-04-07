#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_BUCHER 6100

struct Buch {
    char autor[100];
    int erscheinungsjahr;
    char titel[200];
    char isbn[14];
    float aktueller_preis;
};

void replace_comma_with_dot(char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == ',') {
            str[i] = '.';
        }
    }
}

int extract_year(const char *date_str) {
    int day, month, year;
    while (*date_str == ' ') date_str++;
    if (strlen(date_str) < 8) return -1;
    if (sscanf(date_str, "%d.%d.%d", &day, &month, &year) == 3) {
        if (year >= 1000 && year <= 9999) {
            return year;
        }
    }
    printf("Warnung: Konnte Erscheinungsjahr nicht extrahieren: %s\n", date_str);
    return -1;
}

void write_books_to_file(struct Buch buecher[], int count, const char *filename) {
    FILE *outfile = fopen(filename, "w");
    if (outfile == NULL) {
        printf("Fehler beim Offnen der Datei %s zum Schreiben!\n", filename);
        return;
    }
    fprintf(outfile, "Autor;Erscheinungsjahr;Titel;ISBN;aktueller_preis\n");
    for (int i = 0; i < count; i++) {
        fprintf(outfile, "%s;%d;\"%s\";%s;",
                buecher[i].autor,
                buecher[i].erscheinungsjahr,
                buecher[i].titel,
                (strlen(buecher[i].isbn) > 0 ? buecher[i].isbn : "Unbekannt"));
        if (buecher[i].aktueller_preis == -1.0)
            fprintf(outfile, "Unbekannt\n");
        else
            fprintf(outfile, "%.2f\n", buecher[i].aktueller_preis);
    }
    fclose(outfile);
    printf("Die aktualisierte Buchliste wurde in \"%s\" gespeichert.\n", filename);
}

int main() {
    FILE* file = fopen("buchliste_origin.csv", "r");
    if (file == NULL) {
        printf("Fehler beim Offnen der Datei buchliste_origin.csv!\n");
        return 1;
    }

    struct Buch buecher[MAX_BUCHER];
    int count = 0;
    char line[256];

    if (fgets(line, sizeof(line), file) == NULL) {
        printf("Fehler beim Lesen der Header-Zeile!\n");
        fclose(file);
        return 1;
    }

    while (fgets(line, sizeof(line), file) && count < MAX_BUCHER) {
        struct Buch buch;
        char date_str[20];
        char preis_str[20];
        char titel_temp[200];

        if (sscanf(line, "%99[^;];%19[^;];\"%199[^\"]\";%13[^;];%19s",
                   buch.autor, date_str, titel_temp, buch.isbn, preis_str) == 5) {
            buch.erscheinungsjahr = extract_year(date_str);
            strcpy(buch.titel, titel_temp);
            replace_comma_with_dot(preis_str);
            if (strcmp(preis_str, "Unbekannt") == 0) {
                buch.aktueller_preis = -1.0;
            } else {
                buch.aktueller_preis = atof(preis_str);
            }
            buecher[count++] = buch;
        }
    }
    fclose(file);

    int choice;
    do {
        printf("\n--- Buchverwaltung ---\n");
        printf("1. Buch hinzufuegen\n");
        printf("2. Buch loeschen\n");
        printf("3. Alle Buecher anzeigen\n");
        printf("4. Buecher suchen\n");
        printf("0. Beenden\n");
        printf("Auswahl: ");
        scanf("%d", &choice);
        getchar();

        if (choice == 1) {
            if (count < MAX_BUCHER) {
                struct Buch neues_buch;
                printf("Autor: ");
                fgets(neues_buch.autor, sizeof(neues_buch.autor), stdin);
                neues_buch.autor[strcspn(neues_buch.autor, "\n")] = 0;

                char erscheinung[20];
                printf("Erscheinungsdatum (TT.MM.JJJJ): ");
                fgets(erscheinung, sizeof(erscheinung), stdin);
                neues_buch.erscheinungsjahr = extract_year(erscheinung);

                printf("Titel: ");
                fgets(neues_buch.titel, sizeof(neues_buch.titel), stdin);
                neues_buch.titel[strcspn(neues_buch.titel, "\n")] = 0;

                printf("ISBN: ");
                fgets(neues_buch.isbn, sizeof(neues_buch.isbn), stdin);
                neues_buch.isbn[strcspn(neues_buch.isbn, "\n")] = 0;

                char preis[20];
                printf("Preis (z.B. 12.99 oder 'Unbekannt'): ");
                fgets(preis, sizeof(preis), stdin);
                preis[strcspn(preis, "\n")] = 0;
                replace_comma_with_dot(preis);

                if (strcmp(preis, "Unbekannt") == 0) {
                    neues_buch.aktueller_preis = -1.0;
                } else {
                    neues_buch.aktueller_preis = atof(preis);
                }

                buecher[count++] = neues_buch;
                printf("✅ Buch hinzugefuegt!\n");
            } else {
                printf("❌ Maximale Anzahl erreicht!\n");
            }
        } else if (choice == 2) {
            char suchbegriff[200];
            printf("Gib den Titel oder die ISBN des zu loeschenden Buchs ein: ");
            fgets(suchbegriff, sizeof(suchbegriff), stdin);
            suchbegriff[strcspn(suchbegriff, "\n")] = 0;

            bool gefunden = false;
            for (int i = 0; i < count; i++) {
                if (strcmp(buecher[i].titel, suchbegriff) == 0 || strcmp(buecher[i].isbn, suchbegriff) == 0) {
                    for (int j = i; j < count - 1; j++) {
                        buecher[j] = buecher[j + 1];
                    }
                    count--;
                    gefunden = true;
                    printf("🗑️ Buch geloescht!\n");
                    break;
                }
            }
            if (!gefunden) {
                printf("❗ Kein Buch mit diesem Titel oder dieser ISBN gefunden.\n");
            }
        } else if (choice == 3) {
            for (int i = 0; i < count; i++) {
                printf("\nBuch %d:\n", i + 1);
                printf("Autor: %s\n", buecher[i].autor);
                printf("Erscheinungsjahr: %d\n", buecher[i].erscheinungsjahr);
                printf("Titel: %s\n", buecher[i].titel);
                printf("ISBN: %s\n", (strlen(buecher[i].isbn) > 0 ? buecher[i].isbn : "Unbekannt"));
                if (buecher[i].aktueller_preis == -1.0)
                    printf("Aktueller Preis: Unbekannt\n");
                else
                    printf("Aktueller Preis: %.2f EUR\n", buecher[i].aktueller_preis);
            }
        } else if (choice == 4) {
            int suchwahl;
            char suchtext[200];
            float suchpreis;
            int suchjahr;
            bool gefunden = false;

            printf("\nNach welchem Kriterium moechten Sie suchen?\n");
            printf("1. Autor\n2. Erscheinungsjahr\n3. Titel\n4. ISBN\n5. Preis\nAuswahl: ");
            scanf("%d", &suchwahl);
            getchar();

            switch (suchwahl) {
                case 1:
                    printf("Autor eingeben: ");
                    fgets(suchtext, sizeof(suchtext), stdin);
                    suchtext[strcspn(suchtext, "\n")] = 0;
                    for (int i = 0; i < count; i++) {
                        if (strstr(buecher[i].autor, suchtext)) {
                            gefunden = true;
                            printf("\nAutor: %s\nTitel: %s\n", buecher[i].autor, buecher[i].titel);
                        }
                    }
                    break;
                case 2:
                    printf("Erscheinungsjahr eingeben: ");
                    scanf("%d", &suchjahr);
                    for (int i = 0; i < count; i++) {
                        if (buecher[i].erscheinungsjahr == suchjahr) {
                            gefunden = true;
                            printf("\nAutor: %s\nTitel: %s\n", buecher[i].autor, buecher[i].titel);
                        }
                    }
                    break;
                case 3:
                    printf("Titel eingeben: ");
                    fgets(suchtext, sizeof(suchtext), stdin);
                    suchtext[strcspn(suchtext, "\n")] = 0;
                    for (int i = 0; i < count; i++) {
                        if (strstr(buecher[i].titel, suchtext)) {
                            gefunden = true;
                            printf("\nAutor: %s\nTitel: %s\n", buecher[i].autor, buecher[i].titel);
                        }
                    }
                    break;
                case 4:
                    printf("ISBN eingeben: ");
                    fgets(suchtext, sizeof(suchtext), stdin);
                    suchtext[strcspn(suchtext, "\n")] = 0;
                    for (int i = 0; i < count; i++) {
                        if (strcmp(buecher[i].isbn, suchtext) == 0) {
                            gefunden = true;
                            printf("\nAutor: %s\nTitel: %s\n", buecher[i].autor, buecher[i].titel);
                        }
                    }
                    break;
                case 5:
                    printf("Preis eingeben (z.B. 12.99): ");
                    scanf("%f", &suchpreis);
                    for (int i = 0; i < count; i++) {
                        if (buecher[i].aktueller_preis == suchpreis) {
                            gefunden = true;
                            printf("\nAutor: %s\nTitel: %s\n", buecher[i].autor, buecher[i].titel);
                        }
                    }
                    break;
                default:
                    printf("❌ Ungueltige Auswahl.\n");
                    break;
            }
            if (!gefunden) {
                printf("🔍 Kein passendes Buch gefunden.\n");
            }
        }
    } while (choice != 0);

    write_books_to_file(buecher, count, "buchliste_neu.csv");
    return 0;
}
