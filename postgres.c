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
#include <errno.h>
#include <string.h>
#include <libpq-fe.h>
#include "postgres.h"

/* 
 * Prototypes 
 */
static PGconn* do_connect(void);
static PGconn* do_connect(void);
static int do_insert(PGconn *);
static int do_create(PGconn *);
static Record* do_select_simple(PGconn *);
static int do_select_multiple(PGconn *, Record**);

/*
 * Program
 */
int 
main(void) 
{
    PGconn *conn;
    Record *records;
    int rows;
    int i;
    int rc;

    if ((conn = do_connect()) == NULL) 
        {
            return EACCES;
        }
    if ((rc = do_create(conn)))
        {
            PQfinish(conn);
            return rc;
        }
    if ((rc = do_insert(conn)))
        {
            PQfinish(conn);
            return rc;
        }
    if ((records = do_select_simple(conn)))
        {
            printf("\nSimple SELECT: %d %s %d\n", records->id, records->name, records->price);
            free(records);
        }
    if ((rows = do_select_multiple(conn, &records)) != 0x0)
        {
            printf("\nMultiple SELECT...\n");
            for ( i=0; i<rows; i++ ) 
                {
                    printf("%d %s %d\n", records[i].id, records[i].name, records[i].price);
                }
            free (records);
        }
    PQfinish(conn);
    return 0;
}

/* 
 * Functions
 */
static PGconn* 
do_connect(void)
{
    PGconn *conn = PQconnectdb("password=masterkey user=postgres dbname=concrete");
    if (PQstatus(conn) == CONNECTION_BAD) 
        {
            fprintf(stderr, "Connection to database failed: %s\n", PQerrorMessage(conn));
            errno = EACCES;
            return NULL;
        }
    printf("Database connected!\n");
    printf("User: %s\n", PQuser(conn));
    printf("Database name: %s\n", PQdb(conn));
    return conn;
}

static int 
do_insert(PGconn *conn)
{
    char *dataset[] = {
        "1,'Audi',52642",
        "2,'Mercedes',57127",
        "3,'Skoda',9000",
        NULL
    };
    char **data = dataset;
    char *stmt = NULL;
    int rc = 0x0;
    while (*data) 
        {
            const char *INSERT = "INSERT INTO Cars VALUES(%s)";
            PGresult *res;
            stmt = (char *)realloc(stmt, sizeof(char) * (strlen(INSERT) + strlen(*data)));
            if (!stmt)
                {
                    rc = ENOMEM;
                    break;
                }
            sprintf(stmt, INSERT, *data);
            res = PQexec(conn, stmt);
            if (PQresultStatus(res) != PGRES_COMMAND_OK)
                {
                    PQclear(res);
                    rc = EIO;
                    break;
                }
            ++data;
        }
    if (stmt)
        {
            free(stmt);
        }
    printf("Records inserted!\n");
    return rc;
}

static int 
do_create(PGconn *conn)
{
    PGresult *res = PQexec(conn, "DROP TABLE IF EXISTS Cars");
    
    if (PQresultStatus(res) != PGRES_COMMAND_OK) 
        {
            fprintf(stderr, "Drop table failed: %s\n", PQerrorMessage(conn));
            return EIO;
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
    if (PQresultStatus(res) != PGRES_COMMAND_OK) 
        {
            fprintf(stderr, "Create table failed: %s\n", PQerrorMessage(conn));
            return EIO;
        }
    
    PQclear(res);

    printf("\nTables created!\n");
    return 0;
}

static Record* 
do_select_simple(PGconn *conn)
{
    PGresult *res;
    Record *data;
    
    res = PQexec(conn, "SELECT * FROM Cars LIMIT 1");    

    if (PQresultStatus(res) != PGRES_TUPLES_OK) 
        {
            printf("No data retrieved\n");        
            PQclear(res);
            errno = EIO;
            return NULL;
        } 

    data = (Record *)malloc(sizeof(Record));
    if ( data == NULL ) 
        {
            printf("Error malloc\n");
            errno = ENOMEM;
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

static int 
do_select_multiple(PGconn *conn, Record** data)
{
    PGresult *res;   
    int rows, i;

    res = PQexec(conn, "SELECT * FROM Cars"); 
    if (PQresultStatus(res) != PGRES_TUPLES_OK) 
        {
            printf("No data retrieved\n");        
            PQclear(res);
            return ENOENT;
        } 

    /* PQntuples
     * Returns the number of rows (tuples) in the query result. 
     * (Note that PGresult objects are limited to no more than INT_MAX rows, so an int result is sufficient.)
     * 
     * int PQntuples(const PGresult *res);
     */
    rows = PQntuples(res);
   
    *data = (Record *)malloc(rows * (sizeof(Record)));
    if ( *data == NULL ) 
        {
            fprintf(stderr, "Error malloc\n");
            return ENOMEM;
        }
 
    /* Get value and put in array of struct */
    for(i=0; i<rows; i++)
        {
            data[i]->id = atoi(PQgetvalue(res, i, 0));
            strcpy(data[i]->name,  PQgetvalue(res, i, 1));
            data[i]->price = atoi(PQgetvalue(res, i, 2));
        }    

    PQclear(res);
    return rows;
}
