#include<stdio.h>
#include<stdlib.h>
#include<libcharset.h>
#include<iconv.h>
#include<string.h>
#include<sqlite3.h>

void convert_encoding(const char *input, char *output, const char *from_encoding, const char *to_encoding);
void insert_into_database(int id, const char *nombre);

int main() {
    char input[50];
    char output[100];
    const char *encoding = locale_charset();
    int id;
    
    printf("Enter the id: ");
    scanf("%d", &id);
    printf("Enter the string %s: ", encoding);
    scanf("%s", input);
    
    convert_encoding(input, output, encoding, "UTF-8");
    insert_into_database(id, output);
    
    return 0;
}

void convert_encoding(const char *input, char *output, const char *from_encoding, const char *to_encoding) {
    iconv_t cd = iconv_open(to_encoding, from_encoding);
    if (cd == (iconv_t)-1) {
        perror("iconv_open");
        return;
    }

    size_t inbytesleft = strlen(input);
    size_t outbytesleft = inbytesleft * 2; // Allocate more space for the output buffer
    char *inbuf = (char *)input;
    char *outbuf = output;
    memset(output, 0, outbytesleft);

    if (iconv(cd, &inbuf, &inbytesleft, &outbuf, &outbytesleft) == (size_t)-1) {
        perror("iconv");
        iconv_close(cd);
        return;
    }

    // Ensure the output string is null-terminated
    *outbuf = '\0';

    iconv_close(cd);
}

void insert_into_database(int id, const char *nombre) {
    sqlite3 *db;
    char *err_msg = 0;

    int rc = sqlite3_open("test.db", &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    char *sql_create = "CREATE TABLE IF NOT EXISTS Strings(Id INTEGER PRIMARY KEY, Nombre TEXT);";
    rc = sqlite3_exec(db, sql_create, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error (create table): %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return;
    }

    char *sql_insert = "INSERT INTO Strings(Id, Nombre) VALUES(?, ?);";
    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db, sql_insert, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    sqlite3_bind_int(stmt, 1, id);
    sqlite3_bind_text(stmt, 2, nombre, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
    } else {
        printf("Record inserted successfully\n");
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}
