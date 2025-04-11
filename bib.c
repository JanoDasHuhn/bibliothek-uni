#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "buch.h"

int main() { //csv Datei öffnen
    FILE* file = fopen("buchliste_origin.csv", "r");
    if (file == NULL) {
        printf("Fehler beim Offnen der Datei buchliste_origin.csv!\n");
        return 1;
    }
//Array zum speichern der Bücher und Zählen der gelesenen Bücher
    struct Buch buecher[MAX_BUCHER];
    int count = 0;
    char line[256];

    fgets(line, sizeof(line), file);

    while (fgets(line, sizeof(line), file) && count < MAX_BUCHER) { //csv Datei zeilenweise auslesen
        struct Buch buch;
        char date_str[20];  //strings für die jeweiligen daten (title, datum usw)
        char preis_str[20];
        char titel_temp[200];

        if (sscanf(line, "%99[^;];%19[^;];\"%199[^\"]\";%13[^;];%19s", //zerlegen der Zeilen
                   buch.autor, date_str, titel_temp, buch.isbn, preis_str) == 5) {
            buch.erscheinungsjahr = extract_year(date_str);
            strcpy(buch.titel, titel_temp);
            replace_comma_with_dot(preis_str);
            buch.aktueller_preis = (strcmp(preis_str, "Unbekannt") == 0) ? -1.0 : stringindouble(preis_str);
            buecher[count++] = buch;
        }
    }
    fclose(file);
// Menü und ausleihstatus
    load_ausgeliehen_ids(ausgeliehen, "ausgeliehen.txt");
    int choice;
    do {
        printf("\n--- Buchverwaltung ---\n");
        printf("1. Buch hinzufuegen\n");
        printf("2. Buch loeschen\n");
        printf("3. Alle Buecher anzeigen\n");
        printf("4. Buecher suchen\n");
        printf("5. Buch ausleihen\n");
        printf("6. Buch zurueckgeben\n");
        printf("7. Ausleihstatus abfragen\n");
        printf("0. Beenden\n");
        printf("Auswahl: ");
        scanf("%d", &choice);
        getchar();

        if (choice == 1) { // Buch hinzufügen
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

                neues_buch.aktueller_preis = (strcmp(preis, "Unbekannt") == 0) ? -1.0 : stringindouble(preis);

                buecher[count++] = neues_buch;
                printf("Buch hinzugefuegt!\n");
            } else {
                printf("Maximale Anzahl erreicht!\n");
            }
        } else if (choice == 2) { // Buch löschen
            char suchbegriff[200];
            printf("Gib den Titel oder die ISBN des zu loeschenden Buchs ein: ");
            fgets(suchbegriff, sizeof(suchbegriff), stdin);
            suchbegriff[strcspn(suchbegriff, "\n")] = 0;

            bool gefunden = false;
            for (int i = 0; i < count; i++) {
                if (strcmp(buecher[i].titel, suchbegriff) == 0 || strcmp(buecher[i].isbn, suchbegriff) == 0) {
                    for (int j = i; j < count - 1; j++) {
                        buecher[j] = buecher[j + 1];
                        ausgeliehen[j] = ausgeliehen[j + 1];
                    }
                    count--;
                    gefunden = true;
                    printf("Buch geloescht!\n");
                    break;
                }
            }
            if (!gefunden) {
                printf("Kein Buch mit diesem Titel oder dieser ISBN gefunden.\n");
            }
        } else if (choice == 3) {
            for (int i = 0; i < count; i++) {
                printf("\nBuch %d:\n", i);
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

            switch (suchwahl) { // Suche nach Autor, Jahr, Titel, ISBN oder Preis
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
                    printf("Ungueltige Auswahl.\n");
                    break;
            }
            if (!gefunden) {
                printf("Kein passendes Buch gefunden.\n");
            }
        } else if (choice == 5) { // Buch ausleihen
            int id;
            printf("Buch-ID zum Ausleihen eingeben: ");
            scanf("%d", &id);
            if (id >= 0 && id < count) {
                if (!ausgeliehen[id]) {
                    ausgeliehen[id] = true;
                    printf("Buch '%s' (ID %d) wurde ausgeliehen.\n", buecher[id].titel, id);
                } else {
                    printf("Buch '%s' (ID %d) ist bereits ausgeliehen.\n", buecher[id].titel, id);
                }
            } else {
                printf("Ungueltige ID.\n");
            }
        } else if (choice == 6) { // Buch zurückgeben
            int id;
            printf("Buch-ID zum Zurueckgeben eingeben: ");
            scanf("%d", &id);
            if (id >= 0 && id < count) {
                if (ausgeliehen[id]) {
                    ausgeliehen[id] = false;
                    printf("Buch '%s' (ID %d) wurde zurueckgegeben.\n", buecher[id].titel, id);
                } else {
                    printf("Buch '%s' (ID %d) war nicht ausgeliehen.\n", buecher[id].titel, id);
                }
            } else {
                printf("Ungueltige ID.\n");
            }
        } else if (choice == 7) { // Ausleihstatus abfragen
            int id;
            printf("Buch-ID zum Statuscheck eingeben: ");
            scanf("%d", &id);
            if (id >= 0 && id < count) {
                if (ausgeliehen[id]) {
                    printf("Buch '%s' (ID %d) ist ausgeliehen.\n", buecher[id].titel, id);
                } else {
                    printf("Buch '%s' (ID %d) ist verfuegbar.\n", buecher[id].titel, id);
                }
            } else {
                printf("Ungueltige ID.\n");
            }
        }

    } while (choice != 0);

    write_books_to_file(buecher, count, "buchliste_neu.csv"); //speichern der neuen Buchliste
    save_ausgeliehen_ids(ausgeliehen, count, "ausgeliehen.txt"); //speichern der ausgeliehenen Bücher
    return 0;
}
