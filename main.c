#include<stdio.h>
#include<sqlite3.h>


void insert_into_database(int id, const char *nombre);

int main() {
    char input[50];
    int id;
    printf("Enter the id: ");
    scanf("%d", &id);
    printf("Enter the string: ");
    scanf("%s", input);
    insert_into_database(id, input);
    
    return 0;
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
