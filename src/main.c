#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_OPTION_SIZE 100
#define NUM_KEYS 4

// -t shows names of files/folders in the archive

char Options[NUM_KEYS] = { 't', 'f', 'x', 'v' };

typedef struct {
    char value[MAX_OPTION_SIZE + 1];
    bool is_set;
} Values;

void initializeValues() {
    for (int i = 0; i < NUM_KEYS; i++) {
        values[i].value[MAX_OPTION_SIZE] = '\0';
        values[i].is_set = false;
    }
}

bool is_t_option(const char* option) {
    if (strcmp(option, "-t") == 0) {
        return true;
    }
    return false;
}

void t_option(const char* archive_name) { 
    FILE *file = fopen(archive_name, "rb");
    if (file == NULL) {
        return;
    }
}

int main(int argc, char* argv[]) {
    // printf("Number of arguments: %d\n", argc);

    for (int i = 0; i < argc; i++) {
        // printf("Argument %d: %s\n", i, argv[i]);
        if (is_t_option(argv[i])) {
            printf("option is %s %d", argv[i], i);
        }
    }
    return 0;
}
