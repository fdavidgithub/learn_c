#include <stdio.h>
#include <stdlib.h>

struct Record {
    int id;
    char name[20];
};

int pop_array(struct Record** data)
{
    int i, rows = 3;

    *data = malloc(rows * (sizeof **data));
    if ( *data == NULL ) {
        printf("Error in malloc\n");
        exit(1);
    }

    (*data+0)->id = 1;
    sprintf((*data+0)->name, "%s", "ABC");

    (*data+1)->id = 2;
    sprintf((*data+1)->name, "%s", "ZZZ");

    (*data+2)->id = 3;
    sprintf((*data+2)->name, "%s", "XXX");

    return rows;
}

int main()
{
    struct Record *records;
    int i, rows;

    rows = pop_array(&records);
    
    for ( i=0; i<rows; i++ ) {
        printf("%d, %s\n", records[i].id, records[i].name);
    }

    return 0;
}

