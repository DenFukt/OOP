#pragma once
#include <string>
#include <chrono>
#include <sqlite3.h>
#include <sstream>
using namespace std;
using namespace std::chrono;

// ─── HealthCheck ─────────────────────────────────────────────────────────────
// GET /api/health — повертає JSON зі станом сервісу:
//   status:     "ok" | "degraded"
//   uptime:     секунди з моменту старту
//   db:         "connected" | "error"
//   version:    версія застосунку
//   properties: кількість об'єктів у БД

class HealthCheck {
private:
    steady_clock::time_point startTime;
    string dbPath;
    string version;

    bool testDb() {
        sqlite3* db;
        if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) return false;
        sqlite3_stmt* stmt;
        bool ok = (sqlite3_prepare_v2(db,
            "SELECT COUNT(*) FROM Properties;", -1, &stmt, NULL) == SQLITE_OK);
        if (ok) sqlite3_finalize(stmt);
        sqlite3_close(db);
        return ok;
    }

    int countProperties() {
        sqlite3* db;
        if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) return -1;
        sqlite3_stmt* stmt;
        int count = -1;
        if (sqlite3_prepare_v2(db,
            "SELECT COUNT(*) FROM Properties;", -1, &stmt, NULL) == SQLITE_OK) {
            if (sqlite3_step(stmt) == SQLITE_ROW) count = sqlite3_column_int(stmt, 0);
            sqlite3_finalize(stmt);
        }
        sqlite3_close(db);
        return count;
    }

public:
    HealthCheck(const string& db = "estate.db", const string& ver = "1.0.0")
        : startTime(steady_clock::now()), dbPath(db), version(ver) {}

    string toJson() {
        long uptimeSec = duration_cast<seconds>(
                             steady_clock::now() - startTime).count();
        bool   dbOk  = testDb();
        int    count = dbOk ? countProperties() : -1;
        string status = dbOk ? "ok" : "degraded";

        ostringstream oss;
        oss << "{"
            << "\"status\":\""     << status    << "\","
            << "\"version\":\""    << version   << "\","
            << "\"uptime\":"        << uptimeSec << ","
            << "\"db\":\""         << (dbOk ? "connected" : "error") << "\","
            << "\"properties\":"   << count
            << "}";
        return oss.str();
    }
};
