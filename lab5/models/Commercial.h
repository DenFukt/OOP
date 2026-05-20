#pragma once
#include "Property.h"

class Commercial : public Property {
public:
    string address, businessType;
    double area, rentPrice;

    Commercial(string n, string addr, double a, double p, double rp, string bt)
        : Property(n, p), address(addr), area(a), rentPrice(rp), businessType(bt) {}

    string getType() const override { return "Commercial"; }

    void show() override {
        cout << "[Комерція] " << name << " | " << address
             << " | " << area << "м² | " << businessType
             << " | " << price << " грн | Оренда:" << (isRented?"Так":"Ні") << "\n";
    }

    string toJson() const override {
        return "{\"type\":\"Commercial\","
               "\"name\":\""        + name         + "\","
               "\"address\":\""     + address      + "\","
               "\"area\":"          + to_string(area)      + ","
               "\"price\":"         + to_string(price)     + ","
               "\"rentPrice\":"     + to_string(rentPrice) + ","
               "\"businessType\":\"" + businessType + "\","
               "\"isRented\":"      + (isRented ? "true":"false") + ","
               "\"isBought\":"      + (isBought ? "true":"false") + "}";
    }

    string getInsertSQL() override {
        return "INSERT INTO Properties"
               "(TYPE,NAME,ADDRESS,AREA,PRICE,RENT_PRICE,STATUS,BOUGHT,BUSINESS_TYPE) VALUES"
               "('Commercial','" + name + "','" + address + "'," +
               to_string(area) + "," + to_string(price) + "," + to_string(rentPrice) +
               ",0,0,'" + businessType + "');";
    }
};
