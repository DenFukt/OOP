#pragma once

#include "Property.h"
#include <string>
#include <iostream>

using namespace std;

class Commercial : public Property {
public:
    string address;
    double area, rentPrice;
    string businessType;

    Commercial(string n, string addr, double a, double p, double rp, string bType)
        : Property(n, p), address(addr), area(a), rentPrice(rp), businessType(bType)
    {}

    void show() override {
        cout << "[Офіс/Комерція] " << name
             << " | Адреса: "      << address
             << " | Площа: "       << area
             << " | Тип бізнесу: " << businessType
             << " | Ціна: "        << price
             << " | Оренда: "      << rentPrice
             << " | Статус: "      << (isRented ? "Орендовано" : "Вільно")
             << endl;
    }

    string getInsertSQL() override {
        return "INSERT INTO Properties "
               "(TYPE, NAME, ADDRESS, AREA, PRICE, RENT_PRICE, STATUS, BOUGHT, BUSINESS_TYPE) VALUES "
               "('Commercial', '" + name + "', '" + address + "', " +
               to_string(area) + ", " + to_string(price) + ", " + to_string(rentPrice) +
               ", 0, 0, '" + businessType + "');";
    }
};
