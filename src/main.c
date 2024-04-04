#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// -t shows names of files/folders in the archive

void t_option(const char* archive_name) { 
    FILE *file = fopen(archive_name, "rb");
    if (file == NULL) {
        return;
    }
}

int main(int argc, char* argv[]) {
    printf("Number of arguments: %d\n", argc);

    for (int i = 0; i < argc; i++) {
        printf("Argument %d: %s\n", i, argv[i]);
    }
    return 0;
}
