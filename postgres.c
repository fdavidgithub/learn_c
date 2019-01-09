/*
 * Simples test of connect with postgres
 * Using libpq.c
 * Francis David (www.fdavid.com.br)
 *
 *      Test the C version of libpq, the PostgreSQL frontend library.
 *      More references: http://zetcode.com/db/postgresqlc/
 *                       https://www.postgresql.org/docs/9.1/libpq.html
 *
 * Compile
 * gcc -o postgres postgres.c -lpq
 */
#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>

/*
 * PGRES_COMMAND_OK
 * Successful completion of a command returning no data.
 *
 * PGRES_TUPLES_OK
 * Successful completion of a command returning data (such as a SELECT or SHOW).
 */

/* 
 * Global 
 */
struct Record {
    int id;
    char name[20];
    int price;
};

/* 
 * Functions
 */
void do_exit(PGconn *conn) 
{
    /*
     * PQfinish
     * Closes the connection to the server. Also frees memory used by the PGconn object.
     *
     * void PQfinish(PGconn *conn);
     */
    PQfinish(conn);
    exit(1);
}
 
PGconn* do_connect()
{
    /* PQconnectdb
     * Makes a new connection to the database server.
     *
     * PGconn *PQconnectdb(const char *conninfo);
     */
    PGconn *conn = PQconnectdb("password=masterkey user=postgres dbname=concrete");
   
    if (PQstatus(conn) == CONNECTION_BAD) {
        fprintf(stderr, "Connection to database failed: %s\n", PQerrorMessage(conn));
        do_exit(conn);
    }

    printf("Database connected!\n");
    printf("User: %s\n", PQuser(conn));
    printf("Database name: %s\n", PQdb(conn));
    printf("Password: %s\n", PQpass(conn));

    return conn;
}

void do_insert(PGconn *conn)
{
    PGresult *res;

    res = PQexec(conn, "INSERT INTO Cars VALUES(1,'Audi',52642)");
        
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Insert record failed: %s\n", PQerrorMessage(conn));
        do_exit(conn);
    }
    
    PQclear(res);  

    res = PQexec(conn, "INSERT INTO Cars VALUES(2,'Mercedes',57127)");
        
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Insert record failed: %s\n", PQerrorMessage(conn));
        do_exit(conn);
    }
    
    PQclear(res);    
    
    res = PQexec(conn, "INSERT INTO Cars VALUES(3,'Skoda',9000)");
        
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Insert record failed: %s\n", PQerrorMessage(conn));
        do_exit(conn);
    }
    
    PQclear(res);      
    printf("Records inserted!\n");
}

void do_create(PGconn *conn)
{
    PGresult *res = PQexec(conn, "DROP TABLE IF EXISTS Cars");
    
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Drop table failed: %s\n", PQerrorMessage(conn));
        do_exit(conn);
    }
    
    /*
     * PQclear
     * Frees the storage associated with a PGresult. 
     * Every command result should be freed via PQclear when it is no longer needed.
     *
     * void PQclear(PGresult *res);
     */
    PQclear(res); 
   
    /* PQexec
     * Submits a command to the server and waits for the result.
     *
     * PGresult *PQexec(PGconn *conn, const char *command);
     */
    res = PQexec(conn, "CREATE TABLE Cars(Id INTEGER PRIMARY KEY," \
                       "Name VARCHAR(20), Price INT)");
    
    /*
     * PQresultStatus
     * Returns the result status of the command.
     *
     * ExecStatusType PQresultStatus(const PGresult *res);
     */
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Create table failed: %s\n", PQerrorMessage(conn));
        do_exit(conn);
    }
    
    PQclear(res);

    printf("\nTables created!\n");
}

struct Record* do_select_simple(PGconn *conn)
{
    PGresult *res;
    struct Record *data;
    
    res = PQexec(conn, "SELECT * FROM Cars LIMIT 1");    

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        printf("No data retrieved\n");        
        PQclear(res);
        do_exit(conn);
    } 

    data = malloc(sizeof *data);
    if ( data == NULL ) {
        printf("Error malloc\n");
        do_exit(conn);
    }
 
    /* PQgetvalue
     * Returns a single field value of one row of a PGresult. 
     * Row and column numbers start at 0. 
     * The caller should not free the result directly. 
     * It will be freed when the associated PGresult handle is passed to PQclear.
     *
     * char *PQgetvalue(const PGresult *res,
     *                  int row_number,
     *                  int column_number);
     */
    data->id = atoi(PQgetvalue(res, 0, 0));
    sprintf(data->name,  "%s", PQgetvalue(res, 0, 1));
    data->price = atoi(PQgetvalue(res, 0, 2));
   
    PQclear(res);
    return data;
}

int do_select_multiple(PGconn *conn, struct Record** data)
{
    PGresult *res;   
    int rows, i;

    res = PQexec(conn, "SELECT * FROM Cars"); 
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        printf("No data retrieved\n");        
        PQclear(res);
        do_exit(conn);
    } 

    /* PQntuples
     * Returns the number of rows (tuples) in the query result. 
     * (Note that PGresult objects are limited to no more than INT_MAX rows, so an int result is sufficient.)
     * 
     * int PQntuples(const PGresult *res);
     */
    rows = PQntuples(res);
   
    *data = malloc(rows * (sizeof **data));
    if ( *data == NULL ) {
        printf("Error malloc\n");
        do_exit(conn);
    }
 
    /* Get value and put in array of struct */
    for(i=0; i<rows; i++) {
        (*data+i)->id = atoi(PQgetvalue(res, i, 0));
        sprintf((*data+i)->name,  "%s", PQgetvalue(res, i, 1));
        (*data+i)->price = atoi(PQgetvalue(res, i, 2));
    }    

    PQclear(res);
    return rows;
}

/*
 * Program
 */
int main() 
{
    PGconn *conn = do_connect();
    struct Record *records;
    int rows, i;

    do_create(conn);
    do_insert(conn);

    records = do_select_simple(conn);
    printf("\nSimple SELECT: %d %s %d\n", records->id, records->name, records->price);
 
    rows = do_select_multiple(conn, &records);
    printf("\nMultiple SELECT...\n");
    for ( i=0; i<rows; i++ ) {
        printf("%d %s %d\n", records[i].id, records[i].name, records[i].price);
    }

    do_exit(conn);
    return 0;
}

