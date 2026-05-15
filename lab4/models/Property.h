#pragma once

#include <string>
#include <iostream>

using namespace std;

struct Passport {
    string registrationDate;
};

class Property {
protected:
    string name;
    double price;
    bool isRented, isBought;

public:
    static int totalObjects;

    Property(string n, double p)
        : name(n), price(p), isRented(false), isBought(false) {
        totalObjects++;
    }
    virtual ~Property() { totalObjects--; }

    string getName() const { return name; }
    double getPrice() const { return price; }
    bool   getStatus() const { return isRented; }
    bool   getBoughtness() const { return isBought; }

    void setPrice(double p) { price = p; }
    void setRented(bool s) { isRented = s; }
    void setBought(bool b){ isBought = b; }

    virtual void show() = 0;
    virtual string getInsertSQL() = 0;

    virtual string getUpdateSQL() {
        return "UPDATE Properties SET PRICE = " + to_string(price) +
               ", STATUS = " + (isRented ? "1" : "0") +
               ", BOUGHT = " + (isBought  ? "1" : "0") +
               " WHERE NAME = '" + name + "';";
    }
};

inline int Property::totalObjects = 0;
