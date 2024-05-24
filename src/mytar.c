#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define NUM_OPTIONS 4
#define MAX_OPTION_SIZE 50
#define NUM_OPTIONS_STRINGS 10
#define MAX_ARCHIVENAME_LEN 50
#define ALPHABET_LENGTH 256

#define TAR_HEADER_SIZE 512
#define REGTYPE '0' // new tar or something, otherwise it is \0 or w.e
#define REGTYPE_OLD '\0'
#define ARCHIVE_FILEDATA_SIZE 124

// Error messages
#define Error_File_Not_Found "mytar: %s: Not found in archive\n"
#define Error_Invalid_Option "mytar: Unknown option: -%c\n"
#define Error_Unsupported_Header "mytar: Unsupported header type: %d\n"
const char* Error_No_Name_Provided = "mytar: No archive name provided.";
const char* Error_File_Is_Null = "mytar: Archive file is NULL.";
const char* Error_Exit_On_Failure = "mytar: Exiting with failure status due to previous errors";

// -f specify Archive Filename [1 arg]
// -t List the contents of an archive. Arguments are optional.
// -v Verbosely list files processed
// -x Extract files from an archive.  Arguments are optional.
    //   When given, they specify names of the archive members to be extracted.

// Valid options for checking validity of -char
char Options[NUM_OPTIONS] = { 'f', 't', 'x', 'v' };

// If short option -c is found, sets [(int)c] to true
static bool OptionsSet[ALPHABET_LENGTH] = { false };

char header[TAR_HEADER_SIZE];


// Usage: for (int i = 0; i < t_iterator; ++i) { 
    //        char* ch = option_t_values[i];...
    //    }
int t_iterator = 0;
char option_t_values[NUM_OPTIONS_STRINGS][MAX_OPTION_SIZE];

// Files that were found for -t option
bool t_found_files[NUM_OPTIONS_STRINGS] = { false };

// Sets f archive name to what was found on command line, defautl is empty
// even though by tar std it should be defined in Path or somewhere
char f_archive_name[MAX_ARCHIVENAME_LEN] = "";

// char option_x_values
// char option_v_values

// Checks if there is atleast one option set, and if there are some options set, check if they are all valid
// If no option set, return 1
// If invalid option set, return 2
// Else return 0
int check_set_options() {
    int option_set = 1;

    for (int i = 0; i < ALPHABET_LENGTH; i++) {
        if (OptionsSet[i] == true) {
            option_set = 0;
            bool temp = false;

            for (int j = 0; j < NUM_OPTIONS; ++j) {
                if (i == Options[j]) {
                    temp = true;
                }
            }
            if (temp == false) {
                fprintf(stderr, Error_Invalid_Option, (char)i);
                return 2;
            }
        }
    }
    if (option_set == 1) {
        fprintf(stderr, "mytar: need at least one option");
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

    char delimiter = option[0];
    char option_name = option[1];

    // If it really is an option
    if (delimiter == '-') {
        OptionsSet[option_name - 0] = true;
        return option_name;
    }

    return 0;
}

// Function to check if a file name is in the requested -t files list
bool t_is_requested_file(char* file) {
    for (int i = 0; i < t_iterator; ++i) {
        if (strcmp(file, option_t_values[i]) == 0) {
            return true;
        }
    }
    return false;
}

// Function to mark file as found in the requested -t list
void t_mark_file_found(char* file) {
    for (int i = 0; i < t_iterator; ++i) {
        if (strcmp(file, option_t_values[i]) == 0) {
            t_found_files[i] = true;
        }
    }
}

// Report files that were specified by -t and were not found
int t_report_not_found_files() {
    int ret_code = 0;
    for (int i = 0; i < t_iterator; ++i) {
        if (!t_found_files[i]) {
            fprintf(stderr, Error_File_Not_Found, option_t_values[i]);
            ret_code = 2;
        }
    }

    return ret_code;
}

void t_report_found_files() {
    for (int i = 0; i < t_iterator; ++i) {
        if (t_found_files[i]) {
            printf("%s", option_t_values[i]);
            printf("\n");
        }
    }
}

// String assingment strncpy
void copy_and_ensure_null_termination(char* to, const char* from, int max_len) { // Maybe not the best name
    strncpy(to, from, max_len);
    to[max_len - 1] = '\0'; // apparently this is needed because we are in C
}

// When encountered t_option either list all, or when there was -t set, find only files needed
// TODO: When more options are added, this should be decomposed into
// function for traversing file, and to function doing things for specific option...
int t_option() { 
    bool eof_detected = false;
    FILE *archive = fopen(f_archive_name, "rb");
    if (archive == NULL) {
        fprintf(stderr, Error_File_Is_Null);
        return 2;
    }

    // Read archive file header
    while (!eof_detected && fread(header, 1, TAR_HEADER_SIZE, archive) == TAR_HEADER_SIZE) {
        // Check file type
        if (header[156] != REGTYPE && header[156] != REGTYPE_OLD) {
            fprintf(stderr, Error_Unsupported_Header, header[156]);
            fclose(archive);
            return 2;
        }

        // Ensure null termination (:
        header[100] = '\0';
        
        // If we had files specified by -t
        if (t_iterator > 0 && t_is_requested_file(header)) {
            t_mark_file_found(header);
        }  
        // If there were no files specified by -t, just write this file name
        else if (strlen(header) > 0 && t_iterator == 0) {
            printf("%s\n", header);
        }
        // Else it will print error at the end

        // Ignore everything but filenames
        unsigned long filesize;
        sscanf(header + ARCHIVE_FILEDATA_SIZE, "%lo", &filesize);
        fseek(archive, ((filesize + TAR_HEADER_SIZE - 1) / TAR_HEADER_SIZE) * TAR_HEADER_SIZE, SEEK_CUR);
    }
    fclose(archive);

    t_report_found_files();

    int ret = t_report_not_found_files();
    if (ret != 0) {
        fprintf(stderr, Error_Exit_On_Failure);
        fprintf(stderr, "\n");
        return ret;
    }

    return 0;
}

int main(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        if (is_valid_option(argv[i]) == 'f') {
            if (i+1 < argc && !is_valid_option(argv[i + 1])) {
                ++i;
                copy_and_ensure_null_termination(f_archive_name, argv[i], MAX_ARCHIVENAME_LEN);
            } else {
                fprintf(stderr, Error_No_Name_Provided);
                return 1;
            }
        }

        else if (is_valid_option(argv[i]) == 't') {
            while (i+1 < argc && !is_valid_option(argv[i + 1])) {
                ++i;
                copy_and_ensure_null_termination(option_t_values[t_iterator++], argv[i], MAX_OPTION_SIZE);
            }
        } else if (i - 1 >= 1 && is_valid_option(argv[i-1]) == 0 && is_valid_option(argv[i]) == 0) {
            copy_and_ensure_null_termination(option_t_values[t_iterator++], argv[i], MAX_OPTION_SIZE);
        }
    }

    int arguments_check = check_set_options();
    if (arguments_check != 0) {
        return arguments_check;
    }

    if (OptionsSet['t'] == true) {
        int t_option_ret = t_option();
        if (t_option_ret != 0) {
            return t_option_ret;
        }
    }
    return 0;
}
