#pragma once

#include "IEstateService.h"
#include "../repository/IRepository.h"
#include "../utils/DoublyLinkedList.h"
#include <memory>
#include <iostream>
#include <string>

using namespace std;

class EstateService : public IEstateService {
private:
    shared_ptr<IRepository> _repo;
    DoublyLinkedList<Property*> _list;

public:
    EstateService(shared_ptr<IRepository> repo) : _repo(repo) {}

    Property* findByName(const string& name){
        for(auto p : _list)
            if (p->getName() == name) return p;
        return nullptr;
    }

    void addProperty(Property* p) override {
        _list.push_back(p);
        _repo->save(p);
    }

    void loadData() override {
        _repo->loadAll(_list);
    }

    void showAll() override {
        if(_list.size() == 0){
            cout << "База порожня!\n";
            return;
        }
        for (auto p : _list) p->show();
    }

    void deleteProperty(string name) override {
        Property* p = findByName(name);
        if(p){
            _list.remove(p);
            _repo->remove(name);
            delete p;
            cout << "Об'єкт успішно видалено!\n";
        }
        else{
            cout << "Об'єкт не знайдено.\n";
        }
    }

    void updatePrice(string name, double newPrice) override {
        Property* p = findByName(name);
        if(p){
            p->setPrice(newPrice);
            _repo->update(p);
            cout << "Ціну змінено!\n";
        }
        else{
            cout << "Об'єкт не знайдено.\n";
        }
    }

    void toggleRent(string name) override {
        Property* p = findByName(name);
        if(p){
            p->setRented(!p->getStatus());
            _repo->update(p);
            cout << "Статус оренди змінено!\n";
        }
        else{
            cout << "Об'єкт не знайдено.\n";
        }
    }

    void buyProperty(string name) override {
        Property* p = findByName(name);
        if(p){
            p->setBought(true);
            _repo->update(p);
            cout << "Вітаємо з покупкою!\n";
        }
        else{
            cout << "Об'єкт не знайдено.\n";
        }
    }

    void sellProperty(string name) override {
        Property* p = findByName(name);
        if(p){
            p->setBought(false);
            _repo->update(p);
            cout << "Об'єкт знову виставлено на продаж!\n";
        }
        else{
            cout << "Об'єкт не знайдено.\n";
        }
    }

    void repairProperty(string name) override {
        Property* p = findByName(name);
        if(p){
            cout << "На об'єкті '" << name << "' розпочато ремонтні роботи.\n";
        }
        else{
            cout << "Об'єкт не знайдено.\n";
        }
    }

    void calculateTotalIncome() override {
        double total = 0;
        for(auto p : _list)
            if (p->getStatus()) total += p->getPrice() * 0.05;
        cout << "Загальний прибуток від оренди: " << total << " грн\n";
    }

    void filterByPrice(double maxPrice) override {
        bool found = false;
        for(auto p : _list){
            if(p->getPrice() <= maxPrice){
                p->show();
                found = true;
            }
        }
        if(!found){ 
            cout << "Об'єктів у такому ціновому діапазоні не знайдено.\n";
        }
    }
};
