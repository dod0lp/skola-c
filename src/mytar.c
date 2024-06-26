#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define NUM_OPTIONS 4
#define MAX_OPTION_SIZE 50
#define NUM_OPTIONS_STRINGS 10
#define MAX_ARCHIVENAME_LEN 50
#define ALPHABET_LENGTH 256

#define TAR_HEADER_SIZE 512
#define REGTYPE '0' // new tar header[156] check
#define REGTYPE_OLD '\0' // old tar header[156] check
#define ARCHIVE_FILEDATA_SIZE 124

// Error messages
#define Error_File_Not_Found "mytar: %s: Not found in archive"
#define Error_Invalid_Option "mytar: Unknown option: -%c"
#define Error_Unsupported_Header "mytar: Unsupported header type: %d"
#define ErrorLoneBlock "mytar: A lone zero block at %d"
#define Error_No_Name_Provided "mytar: No archive name provided."
#define Error_File_Is_Null "mytar: Archive file is NULL."
#define Error_Exit_On_Failure "mytar: Exiting with failure status due to previous errors"
#define Error_Unexpected_EOF "mytar: Unexpected EOF in archive"
#define Error_Not_Recoverable "mytar: Error is not recoverable: exiting now"

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
int t_iterator_archive = 0;
char option_t_values_archive[NUM_OPTIONS_STRINGS][MAX_OPTION_SIZE];

// Files that were found for -t option
bool t_found_files[NUM_OPTIONS_STRINGS] = { false };

// Sets f archive name to what was found on command line, defautl is empty
// even though by tar std it should be defined in Path or somewhere
char f_archive_name[MAX_ARCHIVENAME_LEN] = "";

// char option_x_values
// char option_v_values

// String assingment strncpy
void copy_and_ensure_null_termination(char* to, const char* from, int max_len) { // Maybe not the best name
    strncpy(to, from, max_len);
    to[max_len - 1] = '\0'; // apparently this is needed because we are in C
}

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
                fprintf(stderr, "\n");
                return 2;
            }
        }
    }
    if (option_set == 1) {
        fprintf(stderr, "mytar: need at least one option");
        return 2; // i need to differentiate what happened, what if there is in the future some other error that needs implementation
                    // .. what IF, butmaybe i shouldnt think about that and do this consistently idk
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
            copy_and_ensure_null_termination(option_t_values_archive[t_iterator_archive++], file, MAX_OPTION_SIZE);
        }
    }
}

// Report files that were specified by -t and were not found
int t_report_not_found_files() {
    int ret_code = 0;
    for (int i = 0; i < t_iterator; ++i) {
        if (!t_found_files[i]) {
            fprintf(stderr, Error_File_Not_Found, option_t_values[i]);
            fprintf(stderr, "\n");
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

void t_report_found_files_archive_order() {
    for (int i = 0; i < t_iterator_archive; ++i) {
        printf("%s\n", option_t_values_archive[i]);
    }
}

int check_for_lone_block(FILE* archive, unsigned long filesize, int* cur_bytes) {
    *cur_bytes += filesize;
    while (filesize > 0) {
        char buffer[TAR_HEADER_SIZE * 2];
        int bytes_to_read = filesize < (int)sizeof(buffer) ? filesize : (int)sizeof(buffer);
        int bytes_read = fread(buffer, 1, bytes_to_read, archive);
        fseek(archive, -bytes_to_read, SEEK_CUR);

        if (bytes_read == 0) {
            fprintf(stderr, ErrorLoneBlock, *cur_bytes/(TAR_HEADER_SIZE * 2) + 2);
            fprintf(stderr, "\n");
            fclose(archive);
            return 0;
        }
        filesize -= bytes_read;
    }

    return 1;
}

int check_for_truncated_file(FILE* archive, unsigned long filesize) {
    char temp[filesize];
    unsigned long tempnum = fread(temp, 1, filesize, archive);
    if (tempnum != filesize) {
        fprintf(stderr, "%s\n%s\n", Error_Unexpected_EOF, Error_Not_Recoverable);
        return 2;
    }
    fseek(archive, -tempnum, SEEK_CUR);
    return 0;
}

int cur_bytes = 0;
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
            fprintf(stderr, "\n");
            fclose(archive);
            return 2;
        }

        // Ensure null termination (:
        // I can rewrite this because it is saved as local variable, not file pointer
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
        // %lo means "read this as octa int" which it is saved as, maybe I should specify also length? like %12lo
        sscanf(header + ARCHIVE_FILEDATA_SIZE, "%lo", &filesize);

        int loneblock_check = check_for_lone_block(archive, filesize, &cur_bytes);
        if (loneblock_check == 0) {
            return 0;
        }

        int truncated_check = check_for_truncated_file(archive, filesize);
        if (truncated_check != 0) {
            return truncated_check;
        }

        fseek(archive, ((filesize + TAR_HEADER_SIZE - 1) / TAR_HEADER_SIZE) * TAR_HEADER_SIZE, SEEK_CUR);
    }
    fclose(archive);

    t_report_found_files_archive_order();

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
        // Found -f option and getting next value, that should be Archive name
        if (is_valid_option(argv[i]) == 'f') {
            if (i+1 < argc && !is_valid_option(argv[i + 1])) {
                ++i;
                copy_and_ensure_null_termination(f_archive_name, argv[i], MAX_ARCHIVENAME_LEN);
            } else {
                fprintf(stderr, Error_No_Name_Provided);
                return 1;
            }
        }
        // Found -t option, getting all values that are not option after this
        else if (is_valid_option(argv[i]) == 't') {
            while (i+1 < argc && !is_valid_option(argv[i + 1])) {
                ++i;
                copy_and_ensure_null_termination(option_t_values[t_iterator++], argv[i], MAX_OPTION_SIZE);
            } // for example if -t option is first thing on commandline, then there is -f archiveName. PROBABLY will need refactoring after new options
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