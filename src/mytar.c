#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_OPTION_SIZE 50
#define NUM_OPTIONS_STRINGS 10
#define MAX_ARCHIVENAME_LEN 50
#define NUM_OPTIONS 4

// -f specify Archive Filename [1 arg]
// -t List the contents of an archive. Arguments are optional.
// -v Verbosely list files processed
// -x Extract files from an archive.  Arguments are optional.
    //   When given, they specify names of the archive members to be extracted.

char Options[NUM_OPTIONS] = { 'f', 't', 'x', 'v' };
static bool OptionsSet[26] = { false }; // make true OptionSet[char] to show that this option -char was used as argument


// TODO: fix arrays below to properly know which option needs no value/only 1value etc so there
//       is not space allocated for no reason
char option_t_values[NUM_OPTIONS_STRINGS][MAX_OPTION_SIZE];
int t_iterator = 0;
char f_ArchiveName[MAX_ARCHIVENAME_LEN] = "";
// char option_x_values
// char option_v_values

void init_option_values() {
    // TODO: Fill arrays with zeros or something
}

// If not valid returns false (int == 0), otherwise char that was used as short option
char is_valid_option(const char* option) {
    if (strlen(option) != 2) {
        return 0;
    }
    for (int i = 0; i < NUM_OPTIONS; i++) {
        if (option[1] == Options[i] && option[0] == '-') {
            OptionsSet[option[1] - 'a'] = true;
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

void copy_and_ensure_null_termination(char* to, const char* from, int max_len) { // Maybe not the best name
    strncpy(to, from, max_len);
    to[max_len - 1] = '\0'; // apparently this is needed because we are in C
}

int main(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        if (is_valid_option(argv[i]) == 'f') {
            if (i+1 < argc && !is_valid_option(argv[i + 1])) {
                ++i;
                // printf("%s", argv[i]);
                // strncpy(f_ArchiveName, argv[i], MAX_ARCHIVENAME_LEN);
                // f_ArchiveName[MAX_ARCHIVENAME_LEN - 1] = '\0';
                copy_and_ensure_null_termination(f_ArchiveName, argv[i], MAX_ARCHIVENAME_LEN);
            } else {
                fprintf(stderr, "No archive name provided.\n");
                return 1;
            }
        }

        if (is_valid_option(argv[i]) == 't') {
            while (i+1 < argc && !is_valid_option(argv[i + 1])) {
                ++i;
                // strncpy(option_t_values[t_iterator], argv[i], MAX_OPTION_SIZE);
                // option_t_values[t_iterator][MAX_OPTION_SIZE - 1] = '\0';
                copy_and_ensure_null_termination(option_t_values[t_iterator++], argv[i], MAX_OPTION_SIZE);
            }
        }
    }


    printf("%s\n", f_ArchiveName);

    for (int i = 0; i < t_iterator; ++i) {
        printf("%s\n", option_t_values[i]);
    }
    return 0;
}
