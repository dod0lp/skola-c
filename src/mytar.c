#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_OPTION_SIZE 50
#define NUM_OPTIONS_STRINGS 10
#define MAX_ARCHIVENAME_LEN 50
#define ALPHABET_LENGTH 256

// -f specify Archive Filename [1 arg]
// -t List the contents of an archive. Arguments are optional.
// -v Verbosely list files processed
// -x Extract files from an archive.  Arguments are optional.
    //   When given, they specify names of the archive members to be extracted.

#define NUM_OPTIONS 4
char Options[NUM_OPTIONS] = { 'f', 't', 'x', 'v' };

static bool OptionsSet[ALPHABET_LENGTH] = { false }; // make true OptionSet[char] to show that this option -char was used as argument


// TODO: fix arrays below to properly know which option needs no value/only 1value etc so there
//       is not space allocated for no reason
int t_iterator = 0;
char option_t_values[NUM_OPTIONS_STRINGS][MAX_OPTION_SIZE];

// Files that were found for -t option
bool found[NUM_OPTIONS_STRINGS] = { false };
char f_ArchiveName[MAX_ARCHIVENAME_LEN] = "";
// char option_x_values
// char option_v_values

void init_option_values() {
    // TODO: Fill arrays with zeros or something
}

// Checks if there is atleast one option set, and if there are some options set, check if they are all valid
// If no option set, return 1
// If invalid option set, return 2
int check_set_options() {
    int option_set = 1;

    for (int i = 0; i < ALPHABET_LENGTH; i++) {
        if (OptionsSet[i] == true) {
            option_set = 0;
            bool temp = false;

            for (int j = 0; j < NUM_OPTIONS; ++j) {
                printf("%d\n", i);
                if (i == Options[j]) {
                    temp = true;
                }
            }
            if (temp == false) {
                return 2;
            }
        }
    }
    return option_set;
}

// MAYBE: If option was already encountered return -1
// If not valid returns false (int == 0)
// Otherwise return char value as int
int is_valid_option(const char* option) {
    if (strlen(option) != 2) {
        return 0;
    }
    char first = option[0];
    char second = option[1];

    if (first == '-') {
        OptionsSet[second - 0] = true;
        return second;
    }

    return 0;
}

void t_option() { 
    FILE *archive = fopen(f_ArchiveName, "rb");
    if (archive == NULL) {
        fprintf(stderr, "mytar: Archive file is NULL.");
        return;
    }
}

// Function to check if a file name is in the requested list
bool is_requested_file() {
    for (int i = 0; i < t_iterator; ++i) {
        if (strcmp(f_ArchiveName, option_t_values[i]) == 0) {
            return true;
        }
    }
    return false;
}

// Function to mark file as found in the requested list
void mark_file_found() {
    for (int i = 0; i < t_iterator; ++i) {
        if (strcmp(f_ArchiveName, option_t_values[i]) == 0) {
            found[i] = true;
        }
    }
}

void copy_and_ensure_null_termination(char* to, const char* from, int max_len) { // Maybe not the best name
    strncpy(to, from, max_len);
    to[max_len - 1] = '\0'; // apparently this is needed because we are in C
}

int main(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        if (is_valid_option(argv[i]) == 'f') {
            if (i+1 < argc && !is_valid_option(argv[i + 1])) {
                ++i;
                copy_and_ensure_null_termination(f_ArchiveName, argv[i], MAX_ARCHIVENAME_LEN);
            } else {
                fprintf(stderr, "No archive name provided.\n");
                return 1;
            }
        }

        if (is_valid_option(argv[i]) == 't') {
            while (i+1 < argc && !is_valid_option(argv[i + 1])) {
                ++i;
                copy_and_ensure_null_termination(option_t_values[t_iterator++], argv[i], MAX_OPTION_SIZE);
            }
        }
    }

    printf("%d", check_set_options());

    // PRINT OPTIONS
    // printf("%s\n", f_ArchiveName);

    // for (int i = 0; i < t_iterator; ++i) {
    //     printf("%s\n", option_t_values[i]);
    // }
    return 0;
}
