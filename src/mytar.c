#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_OPTION_SIZE 50
#define NUM_OPTIONS_STRINGS 10
#define MAX_ARCHIVENAME_LEN 50
#define NUM_OPTIONS 4

// -t List the contents of an archive. Arguments are optional.
// -f specify Archive Filename [1arg]
// -v Verbosely list files processed
// -x Extract files from an archive.  Arguments are optional.
    //   When given, they specify names of the archive members to be extracted.

char Options[NUM_OPTIONS] = { 'f', 't', 'x', 'v' };
static bool OptionsSet[26] = { false }; // make true OptionSet[char] to show that this option -char was used as argument


// TODO: fix arrays below to properly know which option needs no value/only 1value etc so there
//       is not space allocated for no reason
char option_f_values[NUM_OPTIONS_STRINGS][MAX_OPTION_SIZE];
// char option_t_values
// char option_x_values
// char option_v_values

void init_option_values() {
    // TODO: Fill arrays with zeros or something
}

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

int main(int argc, char* argv[]) {
    char ArchiveName[MAX_ARCHIVENAME_LEN] = "";
    for (int i = 1; i < argc; ++i) {
        if (is_valid_option(argv[i]) == 'f') {
            if (i+1 < argc && !is_valid_option(argv[i + 1])) {
                printf("%s", argv[i + 1]);
                strncpy(ArchiveName, argv[i + 1], MAX_ARCHIVENAME_LEN);
                ArchiveName[MAX_ARCHIVENAME_LEN - 1] = '\0';
            }
        }
    }
    return 0;
}
