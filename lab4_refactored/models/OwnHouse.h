#pragma once

#include "Property.h"
#include <string>
#include <iostream>

using namespace std;

class OwnHouse : public Property {
public:
    string address;
    double area, rentPrice, landArea;
    bool   hasGarage;

    OwnHouse(string n, string addr, double a, double p, double rp,
             double land, bool garage)
        : Property(n, p), address(addr), area(a), rentPrice(rp),
          landArea(land), hasGarage(garage)
    {}

    void show() override {
        cout << "[Приватний будинок] " << name
             << " | Адреса: "  << address
             << " | Площа: "   << area
             << " | Ділянка: " << landArea << " сот."
             << " | Гараж: "   << (hasGarage ? "Є" : "Немає")
             << " | Ціна: "    << price
             << " | Оренда: "  << rentPrice
             << " | Статус: "  << (isRented ? "Орендовано" : "Вільно")
             << endl;
    }

    string getInsertSQL() override {
        return "INSERT INTO Properties "
               "(TYPE, NAME, ADDRESS, AREA, PRICE, RENT_PRICE, STATUS, BOUGHT, LAND_AREA, HAS_GARAGE) VALUES "
               "('OwnHouse', '" + name + "', '" + address + "', " +
               to_string(area) + ", " + to_string(price) + ", " + to_string(rentPrice) +
               ", 0, 0, " + to_string(landArea) + ", " + (hasGarage ? "1" : "0") + ");";
    }
};
