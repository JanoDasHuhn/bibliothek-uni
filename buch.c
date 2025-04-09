#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "buch.h"

bool ausgeliehen[MAX_BUCHER] = {false};

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

void save_ausgeliehen_ids(const bool ausgeliehen[], int count, const char* filename) {
    FILE* f = fopen(filename, "w");
    if (!f) {
        printf("Fehler beim Schreiben der Datei %s!\n", filename);
        return;
    }
    for (int i = 0; i < count; i++) {
        if (ausgeliehen[i]) {
            fprintf(f, "%d\n", i);
        }
    }
    fclose(f);
    printf("Ausgeliehene Buch-IDs wurden in \"%s\" gespeichert.\n", filename);
}

void load_ausgeliehen_ids(bool ausgeliehen[], const char* filename) {
    FILE* f = fopen(filename, "r");
    if (!f) return;
    int id;
    while (fscanf(f, "%d", &id) == 1) {
        if (id >= 0 && id < MAX_BUCHER) {
            ausgeliehen[id] = true;
        }
    }
    fclose(f);
}
