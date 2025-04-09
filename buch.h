#ifndef BUCH_H
#define BUCH_H

#include <stdbool.h>

#define MAX_BUCHER 6100

struct Buch {
    char autor[100];
    int erscheinungsjahr;
    char titel[200];
    char isbn[14];
    float aktueller_preis;
};

extern bool ausgeliehen[MAX_BUCHER];

int extract_year(const char *date_str);
void replace_comma_with_dot(char *str);
void write_books_to_file(struct Buch buecher[], int count, const char *filename);
void save_ausgeliehen_ids(const bool ausgeliehen[], int count, const char* filename);
void load_ausgeliehen_ids(bool ausgeliehen[], const char* filename);

#endif // BUCH_H
