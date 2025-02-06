#include<stdio.h>
#include<stdbool.h>
#include<sqlite3.h>

int menu();
float CargarNota();
void CargarAlumno();

int main() {
    bool salir = false;
    int opcion;
    while (!salir)
    {
        switch (opcion = menu())
        {
        case 1:
            CargarAlumno();
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

float CargarNota()
{
    float Nota;
    if(scanf("%f", &Nota)!=1||Nota<1||Nota>10)
    {
        printf("Entrada no válida. Por favor, ingrese un número valido\n");
        // Limpiar el buffer de entrada
        while (getchar() != '\n');
        // Volver a pedir el número
        Nota = CargarNota();
    }
    return Nota;
}

void CargarAlumno()
{
    int id;
    char nombre[50];
    sqlite3 *db;
    char *err_msg = 0;

    printf("Nombre: ");
    scanf("%s", nombre);
    printf("Lengua: ");
    float lengua = CargarNota();
    printf("Matemáticas: ");
    float matematicas = CargarNota();
    printf("Ciencias: ");
    float ciencias = CargarNota();

    // Abre la base de datos
    int rc = sqlite3_open("Registro.db", &db);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    // Crea la tabla si no existe
    char *sql_create = "CREATE TABLE IF NOT EXISTS Alumnos(Id INTEGER PRIMARY KEY AUTOINCREMENT, Nombre TEXT, Lengua REAL, Matematicas REAL, Ciencias REAL, Promedio REAL);";
    rc = sqlite3_exec(db, sql_create, 0, 0, &err_msg);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, "SQL error (create table): %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return;
    }

    // Crea el trigger si no existe
    char *sql_trigger = "CREATE TRIGGER IF NOT EXISTS ActualizarPromedio AFTER INSERT ON Alumnos BEGIN UPDATE Alumnos SET Promedio = (Lengua + Matematicas + Ciencias) / 3 WHERE rowid = new.rowid; END;";
    rc = sqlite3_exec(db, sql_trigger, 0, 0, &err_msg);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, "SQL error (create trigger): %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return;
    }

    // Prepara la sentencia SQL
    char *sql_insert = "INSERT INTO Alumnos(Nombre, Lengua, Matematicas, Ciencias) VALUES(?, ?, ?, ?);";
    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db, sql_insert, -1, &stmt, 0);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    // Enlaza los valores y verifica que no sean NULL
    sqlite3_bind_text(stmt, 1, nombre, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 2, lengua);
    sqlite3_bind_double(stmt, 3, matematicas);
    sqlite3_bind_double(stmt, 4, ciencias);

    // Ejecuta la sentencia
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) 
    {
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
    } else 
    {
        printf("Record inserted successfully\n");
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}
