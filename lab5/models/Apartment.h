#pragma once
#include "Property.h"

class Apartment : public Property {
public:
    string address;
    double area, rentPrice;
    int    floor, rooms;
    Passport passport;

    Apartment(string n, string addr, double a, double p, double rp,
              int f, int r, string date)
        : Property(n, p), address(addr), area(a), rentPrice(rp), floor(f), rooms(r)
    { passport.registrationDate = date; }

    string getType() const override { return "Apartment"; }

    void show() override {
        cout << "[Квартира] " << name << " | " << address
             << " | " << area << "м² | " << floor << " пов. | " << rooms << " кімн."
             << " | " << price << " грн | Оренда:" << (isRented?"Так":"Ні")
             << " | Куплено:" << (isBought?"Так":"Ні") << "\n";
    }

    string toJson() const override {
        return "{\"type\":\"Apartment\","
               "\"name\":\""       + name    + "\","
               "\"address\":\""    + address + "\","
               "\"area\":"         + to_string(area)      + ","
               "\"price\":"        + to_string(price)     + ","
               "\"rentPrice\":"    + to_string(rentPrice) + ","
               "\"floor\":"        + to_string(floor)     + ","
               "\"rooms\":"        + to_string(rooms)     + ","
               "\"date\":\""       + passport.registrationDate + "\","
               "\"isRented\":"     + (isRented ? "true":"false") + ","
               "\"isBought\":"     + (isBought ? "true":"false") + "}";
    }

    string getInsertSQL() override {
        return "INSERT INTO Properties"
               "(TYPE,NAME,ADDRESS,AREA,PRICE,RENT_PRICE,REG_DATE,STATUS,BOUGHT,FLOOR,ROOMS) VALUES"
               "('Apartment','" + name + "','" + address + "'," +
               to_string(area) + "," + to_string(price) + "," + to_string(rentPrice) +
               ",'" + passport.registrationDate + "',0,0," +
               to_string(floor) + "," + to_string(rooms) + ");";
    }
};
