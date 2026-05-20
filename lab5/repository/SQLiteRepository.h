#pragma once
#include "IRepository.h"
#include "../models/Apartment.h"
#include "../models/Commercial.h"
#include "../models/OwnHouse.h"
#include <sqlite3.h>
#include <iostream>
using namespace std;

class SQLiteRepository : public IRepository {
private:
    sqlite3* db;

    void exec(const string& sql) {
        char* err = nullptr;
        if (sqlite3_exec(db, sql.c_str(), NULL, 0, &err) != SQLITE_OK) {
            cerr << "[DB ERR] " << err << "\n";
            sqlite3_free(err);
        }
    }

public:
    explicit SQLiteRepository(const string& path = "estate.db") {
        sqlite3_open(path.c_str(), &db);
        exec("PRAGMA journal_mode=WAL;");
        exec("CREATE TABLE IF NOT EXISTS Properties ("
             "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
             "TYPE TEXT, NAME TEXT UNIQUE, ADDRESS TEXT,"
             "AREA REAL, PRICE REAL, RENT_PRICE REAL, REG_DATE TEXT,"
             "STATUS INTEGER DEFAULT 0, BOUGHT INTEGER DEFAULT 0,"
             "FLOOR INTEGER, ROOMS INTEGER,"
             "BUSINESS_TYPE TEXT, LAND_AREA REAL, HAS_GARAGE INTEGER);");
    }

    void save(Property* p) override   { exec(p->getInsertSQL()); }
    void update(Property* p) override { exec(p->getUpdateSQL()); }
    void remove(string name) override {
        exec("DELETE FROM Properties WHERE NAME='" + name + "';");
    }

    void loadAll(DoublyLinkedList<Property*>& list) override {
        const char* sql =
            "SELECT TYPE,NAME,ADDRESS,AREA,PRICE,RENT_PRICE,REG_DATE,"
            "STATUS,BOUGHT,FLOOR,ROOMS,BUSINESS_TYPE,LAND_AREA,HAS_GARAGE "
            "FROM Properties;";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) return;

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            auto col = [&](int i) -> string {
                auto* t = sqlite3_column_text(stmt, i);
                return t ? (const char*)t : "";
            };
            string type = col(0), name = col(1), addr = col(2);
            double area = sqlite3_column_double(stmt, 3);
            double price = sqlite3_column_double(stmt, 4);
            double rent  = sqlite3_column_double(stmt, 5);
            string date  = col(6);
            bool   status = sqlite3_column_int(stmt, 7);
            bool   bought = sqlite3_column_int(stmt, 8);

            Property* p = nullptr;
            if (type == "Apartment") {
                p = new Apartment(name, addr, area, price, rent,
                                  sqlite3_column_int(stmt,9),
                                  sqlite3_column_int(stmt,10), date);
            } else if (type == "Commercial") {
                p = new Commercial(name, addr, area, price, rent, col(11));
            } else if (type == "OwnHouse") {
                p = new OwnHouse(name, addr, area, price, rent,
                                 sqlite3_column_double(stmt,12),
                                 sqlite3_column_int(stmt,13));
            }
            if (p) { p->setRented(status); p->setBought(bought); list.push_back(p); }
        }
        sqlite3_finalize(stmt);
    }

    ~SQLiteRepository() { sqlite3_close(db); }
};
