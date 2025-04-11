
#define BUCH_H

#include <stdbool.h>

#define MAX_BUCHER 6100 // Maximale Anzahl an Büchern

struct Buch { // Struct für Buchdaten
    char autor[100];
    int erscheinungsjahr;
    char titel[200];
    char isbn[14];
    float aktueller_preis;
};

extern bool ausgeliehen[MAX_BUCHER]; // Array für den Ausleihstatus

int extract_year(const char *date_str);
void replace_comma_with_dot(char *str);
void write_books_to_file(struct Buch buecher[], int count, const char *filename);
void save_ausgeliehen_ids(const bool ausgeliehen[], int count, const char* filename);
void load_ausgeliehen_ids(bool ausgeliehen[], const char* filename);
double stringindouble(const char *str);
// funktionen die in buch.c benutz und beschrieben wurden
