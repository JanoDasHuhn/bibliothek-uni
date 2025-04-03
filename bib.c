#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_BUCHER 100

struct Buch {
    char autor[100];
    int erscheinungsjahr;
    char titel[200];
    char isbn[14]; // 13 Zeichen + Nullterminierung
    float aktueller_preis;
};

bool is_valid_isbn(const char *isbn) {
    if (isbn == NULL || *isbn == '\0') return false;
    int len = strlen(isbn);
    return (len == 10 || len == 13);
}

int main() {
    FILE* file = fopen("buchliste_origin.csv", "r"); // Datei im Lesemodus öffnen
    if (file == NULL) {
        printf("Fehler beim Öffnen der Datei!\n");
        return 1;
    }

    struct Buch buecher[MAX_BUCHER];
    int count = 0;
    char line[256];
    if (fgets(line, sizeof(line), file) == NULL) { // Erste Zeile (Header) überspringen
        printf("Fehler beim Lesen der Header-Zeile!\n");
        fclose(file);
        return 1;
    }

    while (fgets(line, sizeof(line), file) && count < MAX_BUCHER) {
        struct Buch buch;
        char preis_str[20];

        if (sscanf(line, "%99[^;];%d;%199[^;];%13[^;];%19s", buch.autor, &buch.erscheinungsjahr, buch.titel, buch.isbn, preis_str) == 5) {
            buch.aktueller_preis = (strcmp(preis_str, "Unbekannt") == 0) ? -1.0 : atof(preis_str);
            buecher[count++] = buch;
        }
    }
    fclose(file);

    for (int i = 0; i < count; i++) {
        printf("\nBuch %d:\n", i + 1);
        printf("Autor: %s\n", buecher[i].autor);
        printf("Erscheinungsjahr: %d\n", buecher[i].erscheinungsjahr);
        printf("Titel: %s\n", buecher[i].titel);
        printf("ISBN: %s\n", buecher[i].isbn);
        printf("Aktueller Preis: ");
        if (buecher[i].aktueller_preis == -1.0) {
            printf("Unbekannt\n");
        } else {
            printf("%.2f EUR\n", buecher[i].aktueller_preis);
        }
    }

    return 0;
}
