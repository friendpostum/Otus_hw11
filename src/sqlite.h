#include <iostream>
#include <vector>
#include "sqlite3.h"


struct Sqlite {
    Sqlite() {
        open();
    }

    std::string insert(const std::string& table, int id, const std::string& name) {
        std::string sql = "INSERT INTO ";
        sql += table;
        sql += "(id, name) VALUES(";
        sql += std::to_string(id);
        sql += ",\'";
        sql += name;
        sql += "\')";

        char *errmsg{};
        int rc = sqlite3_exec(db_ptr, sql.c_str(), nullptr, nullptr, &errmsg);

        std::cout << "query: " << sql << std::endl;

        if (rc == SQLITE_CONSTRAINT && SQLITE_CONSTRAINT_PRIMARYKEY == sqlite3_extended_errcode(db_ptr)) {
            return "ERR duplicate " + std::to_string(id) ;
        }

        if (rc != SQLITE_OK) {
            std::cerr << "Can't execute query: " << errmsg << std::endl;
            sqlite3_free(errmsg);
            return "ERR";
        }
        return "OK";
    }

    std::string truncate(const std::string& table) {
        query("DELETE FROM " + table);
        return "OK";
    }
    
    std::string selectEntities(const std::string& sql) {
        std::string answer;
        query(sql, select, &answer);
        return answer;
    }

    ~Sqlite() {
        if (db_ptr) {
            sqlite3_close(db_ptr);
           // std::cout << "~Sqlite()" << std::endl;
        }
    }

private:
    using Callback = int (*)(void*,int,char**,char**);
    void query(const std::string& sql, Callback cb = nullptr, void* cbParam = nullptr) {
        char* errMsg = 0;
        int status = sqlite3_exec(db_ptr, sql.c_str(), cb, cbParam, &errMsg);
        if (status != SQLITE_OK) {
            if (errMsg != nullptr) {
                std::cerr << "SQL error (" << status << "): " << std::string(errMsg) << std::endl;
                sqlite3_free(errMsg);
            }
        }


    }

    int open() {
        if (sqlite3_open(db_name, &db_ptr)) {
            std::cerr << "Can't open database: " << sqlite3_errmsg(db_ptr) << std::endl;
            sqlite3_close(db_ptr);
            return EXIT_FAILURE;
        }
        query("CREATE TABLE IF NOT EXISTS A (id INTEGER PRIMARY KEY, name text);");
        query("CREATE TABLE IF NOT EXISTS B (id INTEGER PRIMARY KEY, name text);");

        return EXIT_SUCCESS;
    }

    static int select (void* param, int columns, char** data, char** colName) {
        enum cort{ID_INDEX, A_INDEX, B_INDEX};

        std::string id = data[ID_INDEX];
        std::string nameA = data[A_INDEX] ? data[A_INDEX] : std::string{};
        std::string nameB = data[B_INDEX] ? data[B_INDEX] : std::string{};

        *static_cast<std::string *>(param) += id + "," + nameA + "," + nameB + '\n';

        return 0;
    }

    const char *db_name = "db.sqlite";
    sqlite3 *db_ptr = nullptr;
};