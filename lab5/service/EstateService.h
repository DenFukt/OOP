#pragma once
#include "IEstateService.h"
#include "../repository/IRepository.h"
#include "../cloud/ICloudSync.h"
#include "../observer/EventLogger.h"
#include "../utils/DoublyLinkedList.h"
#include <memory>
#include <iostream>
using namespace std;

class EstateService : public IEstateService, public Observable {
private:
    shared_ptr<IRepository> _repo;
    shared_ptr<ICloudSync>  _cloud;
    DoublyLinkedList<Property*> _list;

    Property* find(const string& name) {
        for (auto p : _list)
            if (p->getName() == name) return p;
        return nullptr;
    }

public:
    EstateService(shared_ptr<IRepository> repo,
                  shared_ptr<ICloudSync>  cloud = nullptr)
        : _repo(repo), _cloud(cloud) {}

    void subscribe(IObserver* obs) override { Observable::subscribe(obs); }

    void loadData() override { _repo->loadAll(_list); }

    void addProperty(Property* p) override {
        _list.push_back(p);
        _repo->save(p);
        if (_cloud) _cloud->syncSave(p->getName(), p->toJson());
        notify("PROPERTY_ADDED", p->getName());
    }

    void showAll() override {
        if (_list.size() == 0) { cout << "Список порожній.\n"; return; }
        for (auto p : _list) p->show();
    }

    void deleteProperty(string name) override {
        Property* p = find(name);
        if (!p) { cout << "Не знайдено: " << name << "\n"; return; }
        _list.remove(p);
        _repo->remove(name);
        if (_cloud) _cloud->syncDelete(name);
        notify("PROPERTY_DELETED", name);
        delete p;
    }

    void updatePrice(string name, double price) override {
        Property* p = find(name);
        if (!p) { cout << "Не знайдено: " << name << "\n"; return; }
        p->setPrice(price);
        _repo->update(p);
        if (_cloud) _cloud->syncUpdate(name, p->toJson());
        notify("PRICE_UPDATED", name);
    }

    void toggleRent(string name) override {
        Property* p = find(name);
        if (!p) { cout << "Не знайдено: " << name << "\n"; return; }
        p->setRented(!p->getStatus());
        _repo->update(p);
        if (_cloud) _cloud->syncUpdate(name, p->toJson());
        notify(p->getStatus() ? "RENTED" : "RENT_CANCELLED", name);
    }

    void buyProperty(string name) override {
        Property* p = find(name);
        if (!p) { cout << "Не знайдено: " << name << "\n"; return; }
        p->setBought(true);
        _repo->update(p);
        if (_cloud) _cloud->syncUpdate(name, p->toJson());
        notify("PROPERTY_BOUGHT", name);
    }

    void sellProperty(string name) override {
        Property* p = find(name);
        if (!p) { cout << "Не знайдено: " << name << "\n"; return; }
        p->setBought(false);
        _repo->update(p);
        if (_cloud) _cloud->syncUpdate(name, p->toJson());
        notify("PROPERTY_SOLD", name);
    }

    void repairProperty(string name) override {
        Property* p = find(name);
        if (!p) { cout << "Не знайдено: " << name << "\n"; return; }
        notify("REPAIR_STARTED", name);
    }

    double calculateTotalIncome() override {
        double total = 0;
        for (auto p : _list)
            if (p->getStatus()) total += p->getPrice() * 0.05;
        return total;
    }

    string getAllJson() override {
        string result = "[";
        bool first = true;
        for (auto p : _list) {
            if (!first) result += ",";
            first = false;
            result += p->toJson();
        }
        return result + "]";
    }

    Property* findByName(const string& name) { return find(name); }
};
