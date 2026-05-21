#pragma once
#include "../observer/IObserver.h"
#include <sqlite3.h>
#include <string>
#include <iostream>
using namespace std;

class TransactionLog : public IObserver{
private:
    sqlite3* db;

    void exec(const string& sql) {
        char* err = nullptr;
        sqlite3_exec(db, sql.c_str(), NULL, 0, &err);
        if (err) sqlite3_free(err);
    }

public:
    explicit TransactionLog(const string& dbPath = "estate.db"){
        sqlite3_open(dbPath.c_str(), &db);
        exec("PRAGMA journal_mode=WAL;"); 
        exec("CREATE TABLE IF NOT EXISTS TransactionLog ("
             "ID        INTEGER PRIMARY KEY AUTOINCREMENT,"
             "TIMESTAMP TEXT    DEFAULT (datetime('now','localtime')),"
             "ACTION    TEXT    NOT NULL,"
             "PROPERTY  TEXT    NOT NULL,"
             "DETAILS   TEXT);");
    }

    void onEvent(const string& action, const string& propertyName) override {
        string sql = "INSERT INTO TransactionLog (ACTION, PROPERTY) VALUES ('"
                   + action + "','" + propertyName + "');";
        exec(sql);
    }

    string getAllJson() {
        const char* sql =
            "SELECT ID, TIMESTAMP, ACTION, PROPERTY "
            "FROM TransactionLog ORDER BY ID DESC LIMIT 100;";
        sqlite3_stmt* stmt;
        string result = "[";
        bool first = true;

        if(sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK){
            while(sqlite3_step(stmt) == SQLITE_ROW){
                if (!first) result += ",";
                first = false;
                auto col = [&](int i) -> string {
                    auto* t = sqlite3_column_text(stmt, i);
                    return t ? (const char*)t : "";
                };
                result += "{\"id\":"       + col(0) + ","
                          "\"timestamp\":\"" + col(1) + "\","
                          "\"action\":\""    + col(2) + "\","
                          "\"property\":\""  + col(3) + "\"}";
            }
            sqlite3_finalize(stmt);
        }
        return result + "]";
    }

    int getCount(){
        sqlite3_stmt* stmt;
        int count = 0;
        if (sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM TransactionLog;",
                               -1, &stmt, NULL) == SQLITE_OK) {
            if (sqlite3_step(stmt) == SQLITE_ROW) count = sqlite3_column_int(stmt, 0);
            sqlite3_finalize(stmt);
        }
        return count;
    }

    ~TransactionLog() { sqlite3_close(db); }
};
