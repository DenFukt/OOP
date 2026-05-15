#pragma once

#include "../models/Property.h"
#include <string>

using namespace std;

class IEstateService {
public:
    virtual void addProperty(Property* p) = 0;
    virtual void showAll() = 0;
    virtual void deleteProperty(string name) = 0;
    virtual void updatePrice(string name, double price) = 0;
    virtual void toggleRent(string name) = 0;
    virtual void buyProperty(string name) = 0;
    virtual void sellProperty(string name) = 0;
    virtual void repairProperty(string name) = 0;
    virtual void calculateTotalIncome() = 0;
    virtual void filterByPrice(double maxPrice) = 0;
    virtual void loadData() = 0;
    virtual ~IEstateService() {}
};
