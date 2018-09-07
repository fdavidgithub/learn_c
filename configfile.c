/*
 * Read config file
 * Francis David (www.fdavid.com.br)
 *
 * Compile
 * gcc -o configfile configfile.c
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define FILENAME    "configfile.cfg"
#define BUFFER      100

int main()
{
    FILE *fd;

    char buffer[BUFFER];
    char string_value[256] = "EMPTY";
    char field_name[256];
    int integer_value = -1;


    fd = fopen(FILENAME, "r");

    if ( fd == NULL ) {
        printf("Can't open file\n");
        return 1;
    }

    while ( ! feof(fd) ) {
        fgets( buffer, BUFFER, fd );

        if ( buffer[0] == '#' ) {
            continue;
        }

        if ( strstr(buffer, "Param1") ) {
            sscanf(buffer, "%s %s\n", field_name, string_value);
        }

        if ( strstr(buffer, "Param2") ) {
            sscanf(buffer, "%s %d\n", field_name, &integer_value);     
        }
    }

    fclose(fd);
    
    printf("Parameter #1 %s\n", string_value);
    printf("Parameter #2 %d\n", integer_value);

    return 0;
}
