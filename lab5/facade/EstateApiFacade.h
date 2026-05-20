#pragma once

// ═══════════════════════════════════════════════════════════════════
//  ПАТЕРН: Facade (Structural)
//  Спрощує інтерфейс між HTTP-сервером і бізнес-логікою.
//  main() просто реєструє маршрути → кожен виклик іде у Facade.
//  Facade перетворює JSON ↔ доменні об'єкти і повертає JSON-відповіді.
// ═══════════════════════════════════════════════════════════════════

#include "../service/EstateService.h"
#include "../factory/PropertyFactory.h"
#include "../monitoring/HealthCheck.h"
#include "../monitoring/TransactionLog.h"
#include "../monitoring/ApiInsights.h"
#include "../utils/JsonParser.h"
#include <memory>
#include <string>
using namespace std;

// Проста структура HTTP-відповіді (статус + тіло)
struct ApiResponse {
    int    status = 200;
    string body;
};

class EstateApiFacade {
private:
    shared_ptr<EstateService> _svc;
    shared_ptr<HealthCheck>   _health;
    shared_ptr<TransactionLog>_txLog;
    shared_ptr<ApiInsights>   _insights;

    ApiResponse ok(const string& body) { return {200, body}; }
    ApiResponse err(int code, const string& msg) {
        return {code, "{\"error\":\"" + msg + "\"}"};
    }

public:
    EstateApiFacade(shared_ptr<EstateService>  svc,
                    shared_ptr<HealthCheck>    health,
                    shared_ptr<TransactionLog> txLog,
                    shared_ptr<ApiInsights>    insights)
        : _svc(svc), _health(health), _txLog(txLog), _insights(insights) {}

    // ── GET /api/properties ───────────────────────────────────────────────────
    ApiResponse getAll() {
        return ok(_svc->getAllJson());
    }

    // ── POST /api/properties ─────────────────────────────────────────────────
    ApiResponse addProperty(const string& json) {
        PropertyParams p;
        p.type        = jsonField (json, "type");
        p.name        = jsonField (json, "name");
        p.address     = jsonField (json, "address");
        p.area        = jsonDouble(json, "area");
        p.price       = jsonDouble(json, "price");
        p.rentPrice   = jsonDouble(json, "rentPrice");
        p.floor       = jsonInt   (json, "floor");
        p.rooms       = jsonInt   (json, "rooms");
        p.date        = jsonField (json, "date");
        p.businessType= jsonField (json, "businessType");
        p.landArea    = jsonDouble(json, "landArea");
        p.hasGarage   = jsonBool  (json, "hasGarage");

        if (p.type.empty() || p.name.empty())
            return err(400, "Поля 'type' і 'name' обов'язкові");

        try {
            Property* prop = PropertyFactory::create(p);
            _svc->addProperty(prop);
            return ok("{\"ok\":true,\"name\":\"" + p.name + "\"}");
        } catch (const exception& e) {
            return err(400, string(e.what()));
        }
    }

    // ── DELETE /api/properties/:name ─────────────────────────────────────────
    ApiResponse deleteProperty(const string& name) {
        if (name.empty()) return err(400, "Не вказано ім'я");
        _svc->deleteProperty(name);
        return ok("{\"ok\":true}");
    }

    // ── PUT /api/properties/:name/rent ────────────────────────────────────────
    ApiResponse toggleRent(const string& name) {
        _svc->toggleRent(name);
        return ok("{\"ok\":true}");
    }

    // ── PUT /api/properties/:name/buy ─────────────────────────────────────────
    ApiResponse buy(const string& name) {
        _svc->buyProperty(name);
        return ok("{\"ok\":true}");
    }

    // ── PUT /api/properties/:name/sell ────────────────────────────────────────
    ApiResponse sell(const string& name) {
        _svc->sellProperty(name);
        return ok("{\"ok\":true}");
    }

    // ── PUT /api/properties/:name/price ───────────────────────────────────────
    ApiResponse updatePrice(const string& name, const string& json) {
        double price = jsonDouble(json, "price");
        if (price <= 0) return err(400, "Невірна ціна");
        _svc->updatePrice(name, price);
        return ok("{\"ok\":true}");
    }

    // ── GET /api/income ───────────────────────────────────────────────────────
    ApiResponse getIncome() {
        double income = _svc->calculateTotalIncome();
        return ok("{\"income\":" + to_string(income) + "}");
    }

    // ── GET /api/health ───────────────────────────────────────────────────────
    ApiResponse health() {
        return ok(_health->toJson());
    }

    // ── GET /api/transactions ─────────────────────────────────────────────────
    ApiResponse transactions() {
        return ok(_txLog->getAllJson());
    }

    // ── GET /api/insights ─────────────────────────────────────────────────────
    ApiResponse insights() {
        return ok(_insights->toJson());
    }
};
