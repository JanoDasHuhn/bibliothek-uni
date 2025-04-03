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

void replace_comma_with_dot(char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == ',') {
            str[i] = '.';
        }
    }
}

int extract_year(const char *date_str) {
    int day, month, year;

    // Entferne führende und nachfolgende Leerzeichen (Trim)
    while (*date_str == ' ') date_str++;

    // Prüfe, ob das Datum korrekt formatiert ist
    if (strlen(date_str) < 8) return -1;  // Zu kurz für ein gültiges Datum

    if (sscanf(date_str, "%d.%d.%d", &day, &month, &year) == 3) {
        if (year >= 1000 && year <= 9999) { // Gültiges Jahr checken
            return year;
        }
    }

    printf("Warnung: Konnte Erscheinungsjahr nicht extrahieren: %s\n", date_str);
    return -1;
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
        char date_str[20];  // Für das Erscheinungsdatum
        char preis_str[20]; // Für den Preis
        char titel_temp[200];

        if (sscanf(line, "%99[^;];%19[^;];\"%199[^\"]\";%13[^;];%19s",
                   buch.autor, date_str, titel_temp, buch.isbn, preis_str) == 5) {

            // Erscheinungsjahr aus dem Datum extrahieren
            buch.erscheinungsjahr = extract_year(date_str);

            // Titel speichern
            strcpy(buch.titel, titel_temp);

            // Komma in Preis ersetzen und dann konvertieren
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

    for (int i = 0; i < count; i++) {
        printf("\nBuch %d:\n", i + 1);
        printf("Autor: %s\n", buecher[i].autor);
        printf("Erscheinungsjahr: %d\n", buecher[i].erscheinungsjahr);
        printf("Titel: %s\n", buecher[i].titel);
        printf("ISBN: %s\n", (strlen(buecher[i].isbn) > 0) ? buecher[i].isbn : "Unbekannt");
        if (buecher[i].aktueller_preis == -1.0) {
            printf("Aktueller Preis: Unbekannt\n");
        } else {
            printf("Aktueller Preis: %.2f EUR\n", buecher[i].aktueller_preis);
        }
    }

    return 0;
}
