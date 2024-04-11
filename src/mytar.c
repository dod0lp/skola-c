#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_OPTION_SIZE 50
#define NUM_OPTIONS_STRINGS 10
#define NUM_OPTIONS 4

// -t shows names of files/folders in the archive

char Options[NUM_OPTIONS] = { 't', 'f', 'x', 'v' };

// TODO: Make bools to know what option was set

// TODO: fix arrays below to properly know which option needs no value/only 1value etc so there
//       is not space allocated for no reason
char option_t_values[NUM_OPTIONS_STRINGS][MAX_OPTION_SIZE];
char option_f_values[NUM_OPTIONS_STRINGS][MAX_OPTION_SIZE];
char option_x_values[NUM_OPTIONS_STRINGS][MAX_OPTION_SIZE];
char option_v_values[NUM_OPTIONS_STRINGS][MAX_OPTION_SIZE];

// TODO: Just make array of values for each option (some option can be like 'file1, file2,...')

void init_option_values() {
    // TODO: Fill arrays with zeros or something
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
