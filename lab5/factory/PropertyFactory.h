#pragma once
#include "../models/Apartment.h"
#include "../models/Commercial.h"
#include "../models/OwnHouse.h"
#include <stdexcept>
using namespace std;

// ═══════════════════════════════════════════════════════════════════
//  ПАТЕРН: Factory Method (Creational)
//  Централізує логіку створення об'єктів нерухомості.
//  Замість розкиданих new Apartment(...) скрізь — один PropertyFactory.
//  OCP: щоб додати новий тип — розшир PropertyParams + додай гілку.
// ═══════════════════════════════════════════════════════════════════

struct PropertyParams {
    string type;
    string name, address;
    double area      = 0;
    double price     = 0;
    double rentPrice = 0;
    // Apartment
    int    floor = 0, rooms = 0;
    string date  = "";
    // Commercial
    string businessType = "";
    // OwnHouse
    double landArea  = 0;
    bool   hasGarage = false;
};

class PropertyFactory {
public:
    static Property* create(const PropertyParams& p) {
        if (p.type == "Apartment")
            return new Apartment(p.name, p.address, p.area, p.price, p.rentPrice,
                                 p.floor, p.rooms, p.date);
        if (p.type == "Commercial")
            return new Commercial(p.name, p.address, p.area, p.price, p.rentPrice,
                                  p.businessType);
        if (p.type == "OwnHouse")
            return new OwnHouse(p.name, p.address, p.area, p.price, p.rentPrice,
                                p.landArea, p.hasGarage);

        throw invalid_argument("PropertyFactory: невідомий тип \"" + p.type + "\"");
    }
};
