#ifndef POSTGRES_H
#define POSTGRES_H

typedef struct Record {
    int id;
    char name[20];
    int price;
} Record;

#endif /* POSTGRES_H */