#include<stdio.h>
#include<stdbool.h>
#include<string.h>
#include<sqlite3.h>

int menu();
int MenuEditar();
void IniciarBase();
float CargarNota();
void DecimalExel(char numero[6],float decimal);
void CargarAlumno();
void MostrarAlumno();
void MejorPeor();
void EditarAlumno();
void BorrarAlumno();
void ExportAlumnos();

int main() {
    bool salir = false;
    int opcion;
    char respuesta[2];
    IniciarBase();
    while (!salir)
    {
        opcion = menu();
        switch (opcion)
        {
        case 1:
            CargarAlumno();
            break;
        case 2:
            MostrarAlumno();
            break;
        case 3:
            MejorPeor();
            break;
        case 4:
            EditarAlumno();
            break;
        case 5:
            BorrarAlumno();
            break;
        case 6:
            ExportAlumnos();
            break;
        case 7:
            salir = true;
            break;
        default:
            printf("Opción no válida\n");
            break;
        }
        if (!salir)
        {
            printf("¿Otra acción? (s/n): ");
            scanf("%s", respuesta);
            if (strcasecmp(respuesta, "n") == 0)
            {
                salir = true;
            }
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
    printf("4. Editar  un alumno\n");    
    printf("5. Eliminar Alumno\n");
    printf("6. Guardar en archivo .csv\n");
    printf("7. Salir del programa\n");
    printf("Elije opción: ");
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

int MenuEditar()
{
    int opcion;
    if(scanf("%d", &opcion)!=1 ||opcion < 1 || opcion > 4)
    {
        printf("Entrada no válida. Por favor, ingrese un número\n");
        // Limpiar el buffer de entrada
        while (getchar() != '\n');
        // Volver a pedir el número
        opcion = MenuEditar();
    }
    return opcion;
}

void IniciarBase()
{
    sqlite3 *db;
    char *err_msg = 0;
    // Abre la base de datos
    int rc = sqlite3_open("Registro.db", &db);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, "No se puede abrir la base de datos: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }
    // Crea la tabla si no existe
    char *sql_create = "CREATE TABLE IF NOT EXISTS Alumnos(Id INTEGER PRIMARY KEY AUTOINCREMENT, Nombre TEXT, Lengua REAL, Matematicas REAL, Ciencias REAL, Promedio REAL);";
    rc = sqlite3_exec(db, sql_create, 0, 0, &err_msg);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, "Error de SQL (crear tabla): %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return;
    }

    // Crea el trigger de inserción si no existe
    char *sql_trigger_insert = "CREATE TRIGGER IF NOT EXISTS ActualizarPromedio_Insert AFTER INSERT ON Alumnos BEGIN UPDATE Alumnos SET Promedio = (Lengua + Matematicas + Ciencias) / 3 WHERE rowid = new.rowid; END;";
    rc = sqlite3_exec(db, sql_trigger_insert, 0, 0, &err_msg);
    if (rc != SQLITE_OK) 
    {
    fprintf(stderr, "Error de SQL (crear trigger de inserción): %s\n", err_msg);
    sqlite3_free(err_msg);
    sqlite3_close(db);
    return;
    }
    // Crea el trigger de actualización si no existe
    char *sql_trigger_update = "CREATE TRIGGER IF NOT EXISTS ActualizarPromedio_Update AFTER UPDATE ON Alumnos BEGIN UPDATE Alumnos SET Promedio = (Lengua + Matematicas + Ciencias) / 3 WHERE rowid = new.rowid; END;";
    rc = sqlite3_exec(db, sql_trigger_update, 0, 0, &err_msg);
    if (rc != SQLITE_OK) 
    {
    fprintf(stderr, "Error de SQL (crear trigger de actualización): %s\n", err_msg);
    sqlite3_free(err_msg);
    sqlite3_close(db);
    return;
    }
    //serrar base de datos
    sqlite3_close(db);
}

float CargarNota()
{
    float Nota;
    if(scanf("%f", &Nota)!=1||Nota<1||Nota>10)
    {
        printf("Entrada no válida. Por favor, ingrese un número válido\n");
        // Limpiar el buffer de entrada
        while (getchar() != '\n');
        // Volver a pedir el número
        Nota = CargarNota();
    }
    return Nota;
}

void DecimalExel(char numero[6], float decimal)
{
    //2 (parte entera) + 1 (coma) + 2 (decimales) + 1 (carácter nulo) = 6 (tamaño)
    int entero = (int)decimal;
    float resto = decimal - entero;
    snprintf(numero, 6, "%d,%.0f", entero, resto*100);
}

void CargarAlumno()
{
    int id;
    char nombre[50];
    sqlite3 *db;
    char *err_msg = 0;
    // Solicita los datos del alumno
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
        fprintf(stderr, "No se puede abrir la base de datos: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }
    

    // Prepara la sentencia SQL
    char *sql_insert = "INSERT INTO Alumnos(Nombre, Lengua, Matematicas, Ciencias) VALUES(?, ?, ?, ?);";
    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db, sql_insert, -1, &stmt, 0);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, "No se pudo preparar la sentencia: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }
    // Enlaza los valores 
    sqlite3_bind_text(stmt, 1, nombre, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 2, lengua);
    sqlite3_bind_double(stmt, 3, matematicas);
    sqlite3_bind_double(stmt, 4, ciencias);
    // Ejecuta la sentencia
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) 
    {
        fprintf(stderr, "Ejecución fallida: %s\n", sqlite3_errmsg(db));
    } else 
    {
        printf("Registro insertado exitosamente\n");
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

void MostrarAlumno()
{
    sqlite3 *db;
    sqlite3_stmt *stmt;
    // Abre la base de datos
    int rc = sqlite3_open("Registro.db", &db);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, "No se puede abrir la base de datos: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }
    // Prepara la sentencia SQL
    char *sql_select = "SELECT Nombre, Lengua, Matematicas, Ciencias, Promedio FROM Alumnos WHERE Nombre = ?;";
    rc = sqlite3_prepare_v2(db, sql_select, -1, &stmt, 0);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, "No se pudo preparar la sentencia: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }
    // Solicita el nombre del alumno
    char nombre[50];
    printf("Nombre: ");
    scanf("%s", nombre);
    // Enlaza los valores 
    sqlite3_bind_text(stmt, 1, nombre, -1, SQLITE_STATIC);
    // Ejecuta la sentencia
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) 
    {
        printf("Nombre: %s\n", sqlite3_column_text(stmt, 0));
        printf("Lengua: %.2f\n", sqlite3_column_double(stmt, 1));
        printf("Matemáticas: %.2f\n", sqlite3_column_double(stmt, 2));
        printf("Ciencias: %.2f\n", sqlite3_column_double(stmt, 3));
        printf("Promedio: %.2f\n", sqlite3_column_double(stmt, 4));
        printf("\n");
    }
}

void MejorPeor()
{
    sqlite3 *db;
    sqlite3_stmt *stmt;
    // Abre la base de datos
    int rc = sqlite3_open("Registro.db", &db);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, "No se puede abrir la base de datos: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }
    // Prepara la sentencia
    char *sql_select = "SELECT Nombre, Promedio FROM Alumnos WHERE Promedio = (SELECT MAX(Promedio) FROM Alumnos)";
    rc = sqlite3_prepare_v2(db, sql_select, -1, &stmt, 0);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, "No se pudo preparar la sentencia: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }
    // Ejecuta la sentencia
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) 
    {
        printf("Mejor promedio: %s %.2f\n", sqlite3_column_text(stmt, 0), sqlite3_column_double(stmt, 1));
    } 
    sqlite3_finalize(stmt);
    // Prepara la sentencia
    sql_select = "SELECT Nombre, Promedio FROM Alumnos WHERE Promedio = (SELECT MIN(Promedio) FROM Alumnos)";
    rc = sqlite3_prepare_v2(db, sql_select, -1, &stmt, 0);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, "No se pudo preparar la sentencia: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }
    // Ejecuta la sentencia
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) 
    {
        printf("Peor promedio: %s %.2f\n", sqlite3_column_text(stmt, 0), sqlite3_column_double(stmt, 1));
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

void EditarAlumno()
{
    sqlite3 *db;
    sqlite3_stmt *stmt;
    char Nombre[50];
    int id = 0;
    // Abre la base de datos
    int rc = sqlite3_open("Registro.db", &db);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, "No se puede abrir la base de datos: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }
    // Pedir el nombre a buscar
    printf("Nombre: ");
    scanf("%s", Nombre);
    // Prepara la sentencia
    char *sql_select = "SELECT id FROM Alumnos WHERE Nombre = ?;";
    rc = sqlite3_prepare_v2(db, sql_select, -1, &stmt, 0);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, "No se pudo preparar la sentencia: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }
    // Enlaza los valores y verifica que no sean NULL
    sqlite3_bind_text(stmt, 1, Nombre, -1, SQLITE_STATIC);
    // Ejecuta la sentencia
    if ((rc = sqlite3_step(stmt)) == SQLITE_ROW) 
    {
        id = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    //Comprobar si el alumno existe
    if(id == 0)
    {
        printf("El alumno no existe\n");
        sqlite3_close(db);
        return;
    }
    // Pedir la Elemento a editar
    printf("Editar: 1. Nombre 2. Lengua 3. Matemáticas 4. Ciencias\n");
    int opcion = MenuEditar(); 
    char *sql_update;
    float nuevaNota;

    switch (opcion)
    {
        case 1:
            // Nuevo nombre
            char NuevoNombre[50];
            printf("Nuevo nombre: ");
            scanf("%s", NuevoNombre);
            // Prepara la sentencia
            sql_update = "UPDATE Alumnos SET Nombre = ? WHERE id = ?;";
            rc = sqlite3_prepare_v2(db, sql_update, -1, &stmt, 0);
            if (rc != SQLITE_OK) 
            {
                fprintf(stderr, "No se pudo preparar la sentencia: %s\n", sqlite3_errmsg(db));
                sqlite3_close(db);
                return;
            }
            // Enlaza los valores 
            sqlite3_bind_text(stmt, 1, NuevoNombre, -1, SQLITE_STATIC);
            sqlite3_bind_int(stmt, 2, id);
            break;
        case 2:
            // Nueva nota de Lengua
            printf("Nueva nota de Lengua: ");
            nuevaNota = CargarNota();
            sql_update = "UPDATE Alumnos SET Lengua = ? WHERE id = ?;";
            rc = sqlite3_prepare_v2(db, sql_update, -1, &stmt, 0);
            if (rc != SQLITE_OK) 
            {
                fprintf(stderr, "No se pudo preparar la sentencia: %s\n", sqlite3_errmsg(db));
                sqlite3_close(db);
                return;
            }
            sqlite3_bind_double(stmt, 1, nuevaNota);
            sqlite3_bind_int(stmt, 2, id);
            break;
        case 3:
            // Nueva nota de Matemáticas
            printf("Nueva nota de Matemáticas: ");
            nuevaNota = CargarNota();
            sql_update = "UPDATE Alumnos SET Matematicas = ? WHERE id = ?;";
            rc = sqlite3_prepare_v2(db, sql_update, -1, &stmt, 0);
            if (rc != SQLITE_OK) 
            {
                fprintf(stderr, "No se pudo preparar la sentencia: %s\n", sqlite3_errmsg(db));
                sqlite3_close(db);
                return;
            }
            sqlite3_bind_double(stmt, 1, nuevaNota);
            sqlite3_bind_int(stmt, 2, id);
            break;
        case 4:
            // Nueva nota de Ciencias
            printf("Nueva nota de Ciencias: ");
            nuevaNota = CargarNota();
            sql_update = "UPDATE Alumnos SET Ciencias = ? WHERE id = ?;";
            rc = sqlite3_prepare_v2(db, sql_update, -1, &stmt, 0);
            if (rc != SQLITE_OK) 
            {
                fprintf(stderr, "No se pudo preparar la sentencia: %s\n", sqlite3_errmsg(db));
                sqlite3_close(db);
                return;
            }
            sqlite3_bind_double(stmt, 1, nuevaNota);
            sqlite3_bind_int(stmt, 2, id);
            break;
        
    }

    // Ejecuta la sentencia de actualización
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) 
    {
        fprintf(stderr, "Ejecución fallida: %s\n", sqlite3_errmsg(db));
    } else 
    {
        printf("Registro actualizado exitosamente\n");
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

void BorrarAlumno()
{
    sqlite3 *db;
    sqlite3_stmt *stmt;
    char Nombre[50];
    int id = 0;
    // Abre la base de datos
    int rc = sqlite3_open("Registro.db", &db);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, "No se puede abrir la base de datos: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }
    // Pedir el nombre a buscar
    printf("Nombre: ");
    scanf("%s", Nombre);
    // Prepara la sentencia
    char *sql_select = "SELECT id FROM Alumnos WHERE Nombre = ?;";
    rc = sqlite3_prepare_v2(db, sql_select, -1, &stmt, 0);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, "No se pudo preparar la sentencia: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }
    // Enlaza los valores 
    sqlite3_bind_text(stmt, 1, Nombre, -1, SQLITE_STATIC);
    // Ejecuta la sentencia
    if ((rc = sqlite3_step(stmt)) == SQLITE_ROW) 
    {
        id = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    //Comprobar si el alumno existe
    if(id == 0)
    {
        printf("El alumno no existe\n");
        sqlite3_close(db);
        return;
    }
    //Preparar sentencia
    char *sql_drop = "DELETE FROM Alumnos WHERE Id = ?;";
    rc = sqlite3_prepare_v2(db,sql_drop,-1, &stmt,0 );
    if(rc!=SQLITE_OK)
    {
        fprintf(stderr, "No se pudo preparar la sentencia: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }
    //Enlazar los valores 
    sqlite3_bind_int(stmt,1,id);
    // Ejecuta la sentencia deborrado
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) 
    {
        fprintf(stderr, "Ejecución fallida: %s\n", sqlite3_errmsg(db));
    } else 
    {
        printf("Registro borrado exitosamente\n");
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);


}

void ExportAlumnos()
{
    sqlite3 *db;
    sqlite3_stmt *stmt;
    FILE *fp;
    char lengua[6];
    char matematicas[6];
    char ciencias[6];
    char promedio[6];
    const char *sql = "SELECT Id, Nombre, Lengua, Matematicas, Ciencias, Promedio FROM Alumnos";

    // Abre la base de datos
    if (sqlite3_open("Registro.db", &db))
    {
        fprintf(stderr, "No se puede abrir la base de datos: %s\n", sqlite3_errmsg(db));
        return;
    }

    // Prepara la consulta SQL
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK)
    {
        fprintf(stderr, "Error al preparar la consulta SQL: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    // Abre el archivo de salida
    fp = fopen("Alumnos.csv", "w");
    if (!fp)
    {
        fprintf(stderr, "No se puede abrir el archivo: Alumnos.csv\n");
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return;
    }

    // Escribe el encabezado del CSV
    fprintf(fp, "Id;Nombre;Lengua;Matematicas;Ciencias;Promedio\n");

    // Ejecuta la consulta y escribe los datos en el archivo
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        DecimalExel(lengua,sqlite3_column_double(stmt, 2));
        DecimalExel(matematicas,sqlite3_column_double(stmt, 3));
        DecimalExel(ciencias,sqlite3_column_double(stmt, 4));
        DecimalExel(promedio,sqlite3_column_double(stmt, 5));
        fprintf(fp, "%d;%s;%s;%s;%s;%s\n",
                sqlite3_column_int(stmt, 0),
                sqlite3_column_text(stmt, 1),
                lengua,
                matematicas,
                ciencias,
                promedio);
    }

    // Limpia y cierra
    sqlite3_finalize(stmt);
    fclose(fp);
    sqlite3_close(db);
}
