#pragma once

#include "Property.h"
#include <string>
#include <iostream>

using namespace std;

class Apartment : public Property {
public:
    string address;
    double area, rentPrice;
    int    floor, rooms;
    Passport passport;

    Apartment(string n, string addr, double a, double p, double rp,
              int f, int r, string date)
        : Property(n, p), address(addr), area(a), rentPrice(rp), floor(f), rooms(r)
    {
        passport.registrationDate = date;
    }

    void show() override {
        cout << "[Квартира] " << name
             << " | Адреса: "  << address
             << " | Площа: "   << area
             << " | Поверх: "  << floor
             << " | Кімнат: "  << rooms
             << " | Ціна: "    << price
             << " | Оренда: "  << rentPrice
             << " | Статус: "  << (isRented ? "Орендовано" : "Вільно")
             << " | Покупка: " << (isBought  ? "Куплено"    : "На продажі")
             << endl;
    }

    string getInsertSQL() override {
        return "INSERT INTO Properties "
               "(TYPE, NAME, ADDRESS, AREA, PRICE, RENT_PRICE, REG_DATE, STATUS, BOUGHT, FLOOR, ROOMS) VALUES "
               "('Apartment', '" + name + "', '" + address + "', " +
               to_string(area) + ", " + to_string(price) + ", " + to_string(rentPrice) +
               ", '" + passport.registrationDate + "', 0, 0, " +
               to_string(floor) + ", " + to_string(rooms) + ");";
    }
};
