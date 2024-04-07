#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_OPTION_SIZE 100
#define NUM_OPTIONS 4

// -t shows names of files/folders in the archive

char Options[NUM_OPTIONS] = { 't', 'f', 'x', 'v' };

// TODO: Just make array of values for each option (some option can be like 'file1, file2,...')

void init_option_values() {
    for (int i = 0; i < NUM_OPTIONS; i++) {
        for (int j = 0; j < MAX_OPTION_SIZE + 1; j++) {
            Values[i].value[j] = 0;
        }
        Values[i].is_set = false;
    }
}

char is_valid_option(const char* option) {
    if (strlen(option) != 2) {
        return 0;
    }
    for (int i = 0; i < NUM_OPTIONS; i++) {
        if (option[1] == Options[i]) {
            return option[1];
        }
    }
    return 0;
}

void t_option(const char* archive_name) { 
    FILE *file = fopen(archive_name, "rb");
    if (file == NULL) {
        return;
    }
}

int main(int argc, char* argv[]) {
    for (int i = 0; i < argc; i++) {
        char temp = is_valid_option(argv[i]);
        if (temp != 0) {
            printf("option is %c %d", temp, i);
        }
    }
    return 0;
}
