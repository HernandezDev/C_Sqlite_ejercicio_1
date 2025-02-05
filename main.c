#include<stdio.h>
#include<stdbool.h>
#include<sqlite3.h>

int menu();
void insert_into_database();

int main() {
    bool salir = false;
    int opcion;
    while (!salir)
    {
        opcion = menu();
        switch (opcion)
        {
        case 1:
            insert_into_database();
            break;
        case 2:
            /* code */
            break;
        case 3:
            /* code */
            break;
        case 4:
            /* code */
            break;
        case 5:
            /* code */
            break;
        case 6:
            /* code */
            break;
        case 7:
            /* code */
            break;
        case 8:
            salir = true;
            break;
        default:
            printf("Opción no válida\n");
            break;
        }
        
    }
    
    return 0;
}

int menu()
{
    int opcion;
    printf("1. Registrar notas de un alumno\n");
    printf("2. Mostrar notas y promedio de alumno\n");
    printf("3. Mejor y Peor promedio\n");
    printf("4. Editar notas de un alumno\n");
    printf("5. Editar nombre de un alumno\n");
    printf("6. Eliminar Alumno\n");
    printf("7. Guardar en archivo .CSV\n");
    printf("8. Salir del programa\n");
    printf("Elije opcion: ");
    if(scanf("%d", &opcion)!=1)
    {
        printf("Entrada no válida. Por favor, ingrese un número\n");
        // Limpiar el buffer de entrada
        while (getchar() != '\n');
        // Volver a pedir el número
        opcion = menu();
    }
    return opcion;
}

void insert_into_database()
{
    int id;
    char nombre[50];
    sqlite3 *db;
    char *err_msg = 0;

    printf("Enter the id: ");
    scanf("%d", &id);
    printf("Enter the string: ");
    scanf("%s", nombre);
    
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
