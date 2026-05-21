#pragma once
#include "Property.h"

class OwnHouse : public Property{
public:
    string address;
    double area, rentPrice, landArea;
    bool   hasGarage;

    OwnHouse(string n, string addr, double a, double p, double rp,
             double land, bool garage)
        : Property(n, p), address(addr), area(a), rentPrice(rp),
          landArea(land), hasGarage(garage) {}

    string getType() const override { return "OwnHouse"; }

    void show() override {
        cout << "[Будинок] " << name << " | " << address
             << " | " << area << "м² | ділянка:" << landArea << " сот."
             << " | гараж:" << (hasGarage?"Є":"Немає")
             << " | " << price << " грн | Оренда:" << (isRented?"Так":"Ні") << "\n";
    }

    string toJson() const override {
        return "{\"type\":\"OwnHouse\","
               "\"name\":\"" + name + "\","
               "\"address\":\""  + address + "\","
               "\"area\":" + to_string(area) + ","
               "\"price\":" + to_string(price) + ","
               "\"rentPrice\":"  + to_string(rentPrice) + ","
               "\"landArea\":" + to_string(landArea) + ","
               "\"hasGarage\":" + (hasGarage  ? "true":"false") + ","
               "\"isRented\":" + (isRented   ? "true":"false") + ","
               "\"isBought\":" + (isBought   ? "true":"false") + "}";
    }

    string getInsertSQL() override {
        return "INSERT INTO Properties"
               "(TYPE,NAME,ADDRESS,AREA,PRICE,RENT_PRICE,STATUS,BOUGHT,LAND_AREA,HAS_GARAGE) VALUES"
               "('OwnHouse','" + name + "','" + address + "'," +
               to_string(area) + "," + to_string(price) + "," + to_string(rentPrice) +
               ",0,0," + to_string(landArea) + "," + (hasGarage?"1":"0") + ");";
    }
};
