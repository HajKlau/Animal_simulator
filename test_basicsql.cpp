#include <iostream>
#include <sqlite3.h>

int callback(void* NotUsed, int argc, char** argv, char** azColName) {
    for(int i = 0; i<argc; i++) {
        std::cout << azColName[i] << ": " << (argv[i] ? argv[i] : "NULL") << std::endl;
    }
    std::cout << "\n";
    return 0;
}

int main() {
    sqlite3* db;
    char* zErrMsg = 0;
    int rc;

    // Otwieranie połączenia z bazą danych
    rc = sqlite3_open("test.db", &db);
    if(rc) {
        std::cerr << "Błąd otwarcia bazy danych: " << sqlite3_errmsg(db) << std::endl;
        return 0;
    } else {
        std::cout << "Otwarto bazę danych pomyślnie\n";
    }

    // Tworzenie tabeli
    const char* sqlCreateTable =
        "CREATE TABLE IF NOT EXISTS Animals("
        "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
        "Name TEXT NOT NULL,"
        "Species TEXT NOT NULL);";

    // Wstawianie danych do tabeli
    const char* sqlInsert = 
        "INSERT INTO Animals (Name, Species) VALUES ('Burek', 'Pies');";
    
    // Wykonanie zapytania tworzącego tabelę
    rc = sqlite3_exec(db, sqlCreateTable, callback, 0, &zErrMsg);
    if(rc != SQLITE_OK) {
        std::cerr << "Błąd SQL: " << zErrMsg << std::endl;
        sqlite3_free(zErrMsg);
    } else {
        std::cout << "Tabela utworzona pomyślnie\n";
    }

    // Wykonanie zapytania wstawiającego dane
    rc = sqlite3_exec(db, sqlInsert, callback, 0, &zErrMsg);
    if(rc != SQLITE_OK) {
        std::cerr << "Błąd SQL: " << zErrMsg << std::endl;
        sqlite3_free(zErrMsg);
    } else {
        std::cout << "Dane wstawione pomyślnie\n";
    }

    // Odczyt danych z tabeli
    const char* sqlSelect = "SELECT * FROM Animals;";
    rc = sqlite3_exec(db, sqlSelect, callback, 0, &zErrMsg);
    if(rc != SQLITE_OK) {
        std::cerr << "Błąd SQL: " << zErrMsg << std::endl;
        sqlite3_free(zErrMsg);
    } else {
        std::cout << "Odczyt danych pomyślnie\n";
    }

    // Zamykanie połączenia z bazą danych
    sqlite3_close(db);
    return 0;
}
