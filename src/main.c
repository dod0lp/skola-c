#include <stdio.h>
#include <string.h>

// -t shows names of files/folders in the archive

void t_option(const char* archive_name) { 
    
}

int main(int argc, char* argv[]) {
    printf("Number of arguments: %d\n", argc);

    for (int i = 0; i < argc; i++) {
        printf("Argument %d: %s\n", i, argv[i]);
    }
    return 0;
}
