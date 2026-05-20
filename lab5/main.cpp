/*
 * ═══════════════════════════════════════════════════════════════════
 *  Лабораторна робота №5 — Керування нерухомістю
 *  Технології: C++17 / SQLite / httplib / Firebase (optional)
 *
 *  Дизайн-патерни:
 *    Factory Method  (Creational)  → factory/PropertyFactory.h
 *    Observer        (Behavioral)  → observer/EventLogger.h
 *    Facade          (Structural)  → facade/EstateApiFacade.h
 *
 *  Моніторинг:
 *    GET /api/health        — стан сервісу та БД
 *    GET /api/transactions  — лог транзакцій
 *    GET /api/insights      — статистика API-запитів
 *
 *  Запуск: ./lab5 [port]   (дефолт 8080)
 *  Docker: docker-compose up --build
 * ═══════════════════════════════════════════════════════════════════
 */

#include "httplib.h"

#include "repository/SQLiteRepository.h"
#include "cloud/FirebaseSync.h"
#include "observer/EventLogger.h"
#include "monitoring/TransactionLog.h"
#include "monitoring/HealthCheck.h"
#include "monitoring/ApiInsights.h"
#include "service/EstateService.h"
#include "facade/EstateApiFacade.h"

#include <iostream>
#include <memory>
#include <string>
#include <chrono>
using namespace std;

// ── Хелпер: застосовує метрики і повертає JSON-відповідь ─────────────────────
#define ROUTE(method, path, handler)                                           \
    svr.method(path, [&](const httplib::Request& req, httplib::Response& res) {\
        auto _t0 = chrono::steady_clock::now();                                \
        insights->begin(path);                                                 \
        auto resp = handler;                                                   \
        insights->end(path, _t0);                                              \
        res.status = resp.status;                                              \
        res.set_header("Access-Control-Allow-Origin", "*");                    \
        res.set_header("Content-Type", "application/json");                    \
        res.set_content(resp.body, "application/json");                        \
    })

int main(int argc, char** argv) {
    int port = (argc > 1) ? stoi(argv[1]) : 8080;

    // ── DI: ручне з'єднання залежностей (Constructor Injection) ─────────────
    auto repo     = make_shared<SQLiteRepository>("estate.db");
    auto cloud    = make_shared<FirebaseSync>();
    auto svc      = make_shared<EstateService>(repo, cloud);
    auto logger   = make_shared<EventLogger>("events.log");
    auto txLog    = make_shared<TransactionLog>("estate.db");
    auto health   = make_shared<HealthCheck>("estate.db", "1.0.0");
    auto insights = make_shared<ApiInsights>();

    // ── Observer: підписуємо спостерігачів ──────────────────────────────────
    svc->subscribe(logger.get());   // → виводить у консоль + events.log
    svc->subscribe(txLog.get());    // → пише у TransactionLog таблицю SQLite

    // ── Завантажуємо дані з БД ───────────────────────────────────────────────
    svc->loadData();
    cout << "[Server] Дані завантажено з estate.db\n";

    auto facade = make_shared<EstateApiFacade>(svc, health, txLog, insights);

    // ── HTTP Сервер ──────────────────────────────────────────────────────────
    httplib::Server svr;

    // Фронтенд — роздаємо статичні файли з папки frontend/
    svr.set_mount_point("/", "./frontend");

    // CORS preflight
    svr.Options(".*", [](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin",  "*");
        res.set_header("Access-Control-Allow-Methods", "GET,POST,PUT,DELETE,OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
        res.status = 204;
    });

    // ── Properties ───────────────────────────────────────────────────────────
    ROUTE(Get,    "/api/properties",               facade->getAll());
    ROUTE(Post,   "/api/properties",               facade->addProperty(req.body));
    ROUTE(Delete, "/api/properties/:name",         facade->deleteProperty(req.path_params.at("name")));
    ROUTE(Put,    "/api/properties/:name/rent",    facade->toggleRent   (req.path_params.at("name")));
    ROUTE(Put,    "/api/properties/:name/buy",     facade->buy          (req.path_params.at("name")));
    ROUTE(Put,    "/api/properties/:name/sell",    facade->sell         (req.path_params.at("name")));
    ROUTE(Put,    "/api/properties/:name/price",   facade->updatePrice  (req.path_params.at("name"), req.body));

    // ── Доходи ───────────────────────────────────────────────────────────────
    ROUTE(Get,    "/api/income",                   facade->getIncome());

    // ── Моніторинг ───────────────────────────────────────────────────────────
    ROUTE(Get,    "/api/health",                   facade->health());
    ROUTE(Get,    "/api/transactions",             facade->transactions());
    ROUTE(Get,    "/api/insights",                 facade->insights());

    cout << "[Server] Запущено на http://0.0.0.0:" << port << "\n";
    cout << "[Server] Відкрий у браузері: http://localhost:" << port << "\n";

    svr.listen("0.0.0.0", port);
    return 0;
}
