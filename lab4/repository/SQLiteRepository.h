#pragma once

#include "IRepository.h"
#include "../models/Apartment.h"
#include "../models/Commercial.h"
#include "../models/OwnHouse.h"
#include <sqlite3.h>
#include <iostream>
#include <string>

using namespace std;

class SQLiteRepository : public IRepository {
private:
    sqlite3* db;

public:
    SQLiteRepository() {
        sqlite3_open("estate.db", &db);
        const char* sql =
            "CREATE TABLE IF NOT EXISTS Properties ("
            "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
            "TYPE TEXT, NAME TEXT, ADDRESS TEXT, "
            "AREA REAL, PRICE REAL, RENT_PRICE REAL, REG_DATE TEXT, "
            "STATUS INTEGER, BOUGHT INTEGER, "
            "FLOOR INTEGER, ROOMS INTEGER, "
            "BUSINESS_TYPE TEXT, "
            "LAND_AREA REAL, HAS_GARAGE INTEGER);";
        sqlite3_exec(db, sql, NULL, 0, NULL);
    }

    void save(Property* p) override {
        string sql = p->getInsertSQL();
        char* errMsg = nullptr;

        cout << "\n[DEBUG SQL] Спроба виконати: " << sql << endl;

        int rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &errMsg);
        if(rc != SQLITE_OK){
            cout << "[DB ПОМИЛКА] SQLite каже: " << errMsg << endl;
            sqlite3_free(errMsg);
        }
        else{
            cout << "[DB УСПІХ] Дані фізично записано у файл estate.db!" << endl;
        }
    }

    void update(Property* p) override {
        string sql = p->getUpdateSQL();
        char* errMsg = nullptr;

        cout << "\n[DEBUG SQL] Спроба оновлення: " << sql << endl;

        int rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &errMsg);
        if(rc != SQLITE_OK){
            cout << "[DB ПОМИЛКА] SQLite каже: " << errMsg << endl;
            sqlite3_free(errMsg);
        }
        else{
            cout << "[DB УСПІХ] Дані об'єкта успішно оновлено в базі!" << endl;
        }
    }

    void remove(string name) override {
        string sql = "DELETE FROM Properties WHERE NAME = '" + name + "';";
        char* errMsg = nullptr;

        cout << "\n[DEBUG SQL] Спроба видалення: " << sql << endl;

        int rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &errMsg);
        if(rc != SQLITE_OK){
            cout << "[DB ПОМИЛКА] SQLite каже: " << errMsg << endl;
            sqlite3_free(errMsg);
        }
        else{
            cout << "[DB УСПІХ] Об'єкт успішно видалено з бази даних!" << endl;
        }
    }

    void loadAll(DoublyLinkedList<Property*>& list) override {
        const char* sql =
            "SELECT TYPE, NAME, ADDRESS, AREA, PRICE, RENT_PRICE, REG_DATE, "
            "STATUS, BOUGHT, FLOOR, ROOMS, BUSINESS_TYPE, LAND_AREA, HAS_GARAGE "
            "FROM Properties;";
        sqlite3_stmt* stmt;

        if(sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK){
            while(sqlite3_step(stmt) == SQLITE_ROW){
                string type = (const char*)sqlite3_column_text(stmt, 0);
                string name = (const char*)sqlite3_column_text(stmt, 1);
                string addr = (const char*)sqlite3_column_text(stmt, 2);
                double area = sqlite3_column_double(stmt, 3);
                double price = sqlite3_column_double(stmt, 4);
                double rentPrice = sqlite3_column_double(stmt, 5);
                string date = sqlite3_column_text(stmt, 6) ? (const char*)sqlite3_column_text(stmt, 6) : "";
                bool status = sqlite3_column_int(stmt, 7);
                bool bought = sqlite3_column_int(stmt, 8);

                Property* p = nullptr;

                if(type == "Apartment"){
                    int floor = sqlite3_column_int(stmt, 9);
                    int rooms = sqlite3_column_int(stmt, 10);
                    p = new Apartment(name, addr, area, price, rentPrice, floor, rooms, date);
                }
                else if(type == "Commercial"){
                    string bType = sqlite3_column_text(stmt, 11) ? (const char*)sqlite3_column_text(stmt, 11) : "";
                    p = new Commercial(name, addr, area, price, rentPrice, bType);
                }
                else if(type == "OwnHouse"){
                    double land = sqlite3_column_double(stmt, 12);
                    bool garage = sqlite3_column_int(stmt, 13);
                    p = new OwnHouse(name, addr, area, price, rentPrice, land, garage);
                }

                if(p){
                    p->setRented(status);
                    p->setBought(bought);
                    list.push_back(p);
                }
            }
            sqlite3_finalize(stmt);
            cout << "[DB] Дані успішно завантажено з бази при старті!" << endl;
        }
        else{
            cout << "[DB ПОМИЛКА] Не вдалося прочитати базу: "
                 << sqlite3_errmsg(db) << endl;
        }
    }

    ~SQLiteRepository() { sqlite3_close(db); }
};
