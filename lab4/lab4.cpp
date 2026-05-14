#include <iostream>
#include <string>
#include <ios>
#include <limits>
#include <iomanip>
#include <memory>

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

#include <sqlite3.h>
#include "Hypodermic/Hypodermic.h"

using namespace std;

template <typename T>
class DoublyLinkedList{
private:
    struct Node{
        T data;
        Node *next, *prev;
        Node(T val) : data(val), next(nullptr), prev(nullptr) {}
    };
    Node *head, *tail;
    int count;
public:
    DoublyLinkedList() : head(nullptr), tail(nullptr), count(0) {}
    ~DoublyLinkedList() { clear(); }

    void push_back(T val){
        Node* newNode = new Node(val);
        if(!head){
            head = tail = newNode;
        }
        else{ 
            tail->next = newNode; 
            newNode->prev = tail;
            tail = newNode;
        }
        count++;
    }

    void remove(T val){
        Node* curr = head;
        while(curr){
            if(curr->data == val){
                if(curr->prev){
                    curr->prev->next = curr->next;
                }
                if(curr->next){
                    curr->next->prev = curr->prev;
                }
                if(curr == head){
                    head = curr->next;
                }
                if(curr == tail){
                    tail = curr->prev;
                }
                delete curr;
                count--;
                return;
            }
            curr = curr->next;
        }
    }

    void clear(){
        Node* curr = head;
        while(curr){
            Node* next = curr->next;
            delete curr; curr = next;
        }
        head = tail = nullptr; 
        count = 0;
    }

    int size() const { return count; }

    struct Iterator{
        Node* current;
        bool operator!=(const Iterator& other) { return current != other.current; }
        void operator++() { if(current) current = current->next; }
        T operator*() { return current->data; }
    };
    Iterator begin() { return { head }; }
    Iterator end() { return { nullptr }; }
};

struct Passport { string registrationDate; };

class Property{
protected:
    string name;
    double price;
    bool isRented, isBought;
public:
    static int totalObjects;
    Property(string n, double p) : name(n), price(p), isRented(false), isBought(false) { totalObjects++; }
    virtual ~Property() { totalObjects--; }

    string getName() const { return name; }
    double getPrice() const { return price; }
    bool getStatus() const { return isRented; }
    bool getBoughtness() const { return isBought; }

    void setPrice(double p) { price = p; }
    void setRented(bool s) { isRented = s; }
    void setBought(bool b) { isBought = b; }

    virtual void show() = 0;
    virtual string getInsertSQL() = 0;
    virtual string getUpdateSQL(){
        return "UPDATE Properties SET PRICE = " + to_string(price) + 
               ", STATUS = " + (isRented ? "1" : "0") + 
               ", BOUGHT = " + (isBought ? "1" : "0") + " WHERE NAME = '" + name + "';";
    }
};
int Property::totalObjects = 0;

class Apartment : public Property {
public:
    string address; double area; double rentPrice; int floor, rooms; Passport passport;
    Apartment(string n, string addr, double a, double p, double rp, int f, int r, string date)
        : Property(n, p), address(addr), area(a), rentPrice(rp), floor(f), rooms(r) { passport.registrationDate = date; }

    void show() override{ 
        cout << "[Квартира] " << name << " | Адреса: " << address << " | Площа: " << area << " | Поверх: " << floor 
             << " | Кімнат: " << rooms << " | Ціна: " << price << " | Оренда: " << rentPrice 
             << " | Статус: " << (isRented ? "Орендовано" : "Вільно") << " | Покупка: " << (isBought ? "Куплено" : "На продажі") << endl; 
    }
    string getInsertSQL() override{
        return "INSERT INTO Properties (TYPE, NAME, ADDRESS, AREA, PRICE, RENT_PRICE, REG_DATE, STATUS, BOUGHT, FLOOR, ROOMS) VALUES "
               "('Apartment', '" + name + "', '" + address + "', " + to_string(area) + ", " + to_string(price) + ", " + to_string(rentPrice) + ", '" + passport.registrationDate + "', 0, 0, " + to_string(floor) + ", " + to_string(rooms) + ");";
    }
};

class Commercial : public Property {
public:
    string address; double area; double rentPrice; string businessType;
    Commercial(string n, string addr, double a, double p, double rp, string bType)
        : Property(n, p), address(addr), area(a), rentPrice(rp), businessType(bType) {}

    void show() override{ 
        cout << "[Офіс/Комерція] " << name << " | Адреса: " << address << " | Площа: " << area << " | Тип бізнесу: " << businessType 
             << " | Ціна: " << price << " | Оренда: " << rentPrice 
             << " | Статус: " << (isRented ? "Орендовано" : "Вільно") << endl; 
    }
    string getInsertSQL() override{
        return "INSERT INTO Properties (TYPE, NAME, ADDRESS, AREA, PRICE, RENT_PRICE, STATUS, BOUGHT, BUSINESS_TYPE) VALUES "
               "('Commercial', '" + name + "', '" + address + "', " + to_string(area) + ", " + to_string(price) + ", " + to_string(rentPrice) + ", 0, 0, '" + businessType + "');";
    }
};

class ownHouse : public Property{
public:
    string address; double area; double rentPrice; double landArea; bool hasGarage;
    ownHouse(string n, string addr, double a, double p, double rp, double land, bool garage)
        : Property(n, p), address(addr), area(a), rentPrice(rp), landArea(land), hasGarage(garage) {}

    void show() override{ 
        cout << "[Приватний будинок] " << name << " | Адреса: " << address << " | Площа: " << area << " | Ділянка: " << landArea << " сот."
             << " | Гараж: " << (hasGarage ? "Є" : "Немає") << " | Ціна: " << price << " | Оренда: " << rentPrice 
             << " | Статус: " << (isRented ? "Орендовано" : "Вільно") << endl; 
    }
    string getInsertSQL() override{
        return "INSERT INTO Properties (TYPE, NAME, ADDRESS, AREA, PRICE, RENT_PRICE, STATUS, BOUGHT, LAND_AREA, HAS_GARAGE) VALUES "
               "('ownHouse', '" + name + "', '" + address + "', " + to_string(area) + ", " + to_string(price) + ", " + to_string(rentPrice) + ", 0, 0, " + to_string(landArea) + ", " + (hasGarage ? "1" : "0") + ");";
    }
};

class IRepository{
public:
    virtual void save(Property* p) = 0;
    virtual void update(Property* p) = 0;
    virtual void loadAll(DoublyLinkedList<Property*>& list) = 0;
    virtual void remove(string name) = 0;
    virtual ~IRepository() {}
};

class IEstateService{
public:
    virtual void addProperty(Property* p) = 0;
    virtual void showAll() = 0;
    virtual void deleteProperty(string name) = 0;
    virtual void updatePrice(string name, double newPrice) = 0;
    virtual void toggleRent(string name) = 0;
    virtual void buyProperty(string name) = 0;
    virtual void sellProperty(string name) = 0;
    virtual void repairProperty(string name) = 0;
    virtual void calculateTotalIncome() = 0;
    virtual void filterByPrice(double maxPrice) = 0;
    virtual void loadData() = 0;
    virtual ~IEstateService() {}
};

class SQLiteRepository : public IRepository{
private:
    sqlite3* db;
public:
    SQLiteRepository(){
        sqlite3_open("estate.db", &db);
        const char* sql = "CREATE TABLE IF NOT EXISTS Properties ("
                          "ID INTEGER PRIMARY KEY AUTOINCREMENT, TYPE TEXT, NAME TEXT, ADDRESS TEXT, "
                          "AREA REAL, PRICE REAL, RENT_PRICE REAL, REG_DATE TEXT, STATUS INTEGER, "
                          "BOUGHT INTEGER, FLOOR INTEGER, ROOMS INTEGER, BUSINESS_TYPE TEXT, "
                          "LAND_AREA REAL, HAS_GARAGE INTEGER);";
        sqlite3_exec(db, sql, NULL, 0, NULL);
    }
    void save(Property* p) override {
    string sql = p->getInsertSQL();
    char* errMsg = nullptr;

    cout << "\n[DEBUG SQL] Спроба виконати: " << sql << endl;

    int rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &errMsg);
    
    if (rc != SQLITE_OK) {
        cout << "[DB ПОМИЛКА] SQLite каже: " << errMsg << endl;
        sqlite3_free(errMsg);
    } else {
        cout << "[DB УСПІХ] Дані фізично записано у файл estate.db!" << endl;
    }
}
    void update(Property* p) override {
    string sql = p->getUpdateSQL();
    char* errMsg = nullptr;

    cout << "\n[DEBUG SQL] Спроба оновлення: " << sql << endl;

    int rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &errMsg);
    
    if (rc != SQLITE_OK) {
        cout << "[DB ПОМИЛКА] SQLite каже: " << errMsg << endl;
        sqlite3_free(errMsg);
    } else {
        cout << "[DB УСПІХ] Дані об'єкта успішно оновлено в базі!" << endl;
    }
}
    void remove(string name) override {
        string sql = "DELETE FROM Properties WHERE NAME = '" + name + "';";
        char* errMsg = nullptr;

        cout << "\n[DEBUG SQL] Спроба видалення: " << sql << endl;

        int rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &errMsg);

        if (rc != SQLITE_OK) {
            cout << "[DB ПОМИЛКА] SQLite каже: " << errMsg << endl;
            sqlite3_free(errMsg);
        } else {
            cout << "[DB УСПІХ] Об'єкт успішно видалено з бази даних!" << endl;
        }
    }
    void loadAll(DoublyLinkedList<Property*>& list) override {
        const char* sql = "SELECT TYPE, NAME, ADDRESS, AREA, PRICE, RENT_PRICE, REG_DATE, STATUS, BOUGHT, FLOOR, ROOMS, BUSINESS_TYPE, LAND_AREA, HAS_GARAGE FROM Properties;";
        sqlite3_stmt* stmt;

        // Підготовлюємо запит
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
            // Читаємо рядок за рядком
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                // Зчитуємо базові поля
                string type = (const char*)sqlite3_column_text(stmt, 0);
                string name = (const char*)sqlite3_column_text(stmt, 1);
                string addr = (const char*)sqlite3_column_text(stmt, 2);
                double area = sqlite3_column_double(stmt, 3);
                double price = sqlite3_column_double(stmt, 4);
                double rentPrice = sqlite3_column_double(stmt, 5);
                string date = sqlite3_column_text(stmt, 6) ? (const char*)sqlite3_column_text(stmt, 6) : "";
                bool status = sqlite3_column_int(stmt, 7);
                bool bought = sqlite3_column_int(stmt, 8);

                Property* p = nullptr;

                // Залежно від типу, створюємо потрібний об'єкт
                if (type == "Apartment") {
                    int floor = sqlite3_column_int(stmt, 9);
                    int rooms = sqlite3_column_int(stmt, 10);
                    p = new Apartment(name, addr, area, price, rentPrice, floor, rooms, date);
                } 
                else if (type == "Commercial") {
                    string bType = sqlite3_column_text(stmt, 11) ? (const char*)sqlite3_column_text(stmt, 11) : "";
                    p = new Commercial(name, addr, area, price, rentPrice, bType);
                } 
                else if (type == "ownHouse") {
                    double land = sqlite3_column_double(stmt, 12);
                    bool garage = sqlite3_column_int(stmt, 13);
                    p = new ownHouse(name, addr, area, price, rentPrice, land, garage);
                }

                // Якщо об'єкт успішно створено, відновлюємо його статус і додаємо в список
                if (p) {
                    p->setRented(status);
                    p->setBought(bought);
                    list.push_back(p);
                }
            }
            sqlite3_finalize(stmt); // Очищаємо пам'ять від запиту
            cout << "[DB] Дані успішно завантажено з бази при старті!" << endl;
        } else {
            cout << "[DB ПОМИЛКА] Не вдалося прочитати базу: " << sqlite3_errmsg(db) << endl;
        }
    }
    ~SQLiteRepository() { sqlite3_close(db); }
};

class EstateService : public IEstateService {
private:
    shared_ptr<IRepository> _repo;
    DoublyLinkedList<Property*> _list;
public:
    EstateService(shared_ptr<IRepository> repo) : _repo(repo) {}

    void addProperty(Property* p) override { _list.push_back(p); _repo->save(p); }

    void loadData() override {
        _repo->loadAll(_list);
    }
    void showAll() override{ 
        if(_list.size() == 0) cout << "База порожня!\n";
        for (auto p : _list) p->show(); 
    }

    Property* findByName(string name){
        for (auto p : _list) if (p->getName() == name) return p;
        return nullptr;
    }

    void deleteProperty(string name) override{
        Property* p = findByName(name);
        if (p) { _list.remove(p); _repo->remove(name); delete p; cout << "Об'єкт успішно видалено!\n"; }
        else cout << "Об'єкт не знайдено.\n";
    }

    void updatePrice(string name, double newPrice) override{
        Property* p = findByName(name);
        if (p) { p->setPrice(newPrice); _repo->update(p); cout << "Ціну змінено!\n"; }
    }

    void toggleRent(string name) override{
        Property* p = findByName(name);
        if (p) { p->setRented(!p->getStatus()); _repo->update(p); cout << "Статус оренди змінено!\n"; }
    }

    void buyProperty(string name) override{
        Property* p = findByName(name);
        if (p) { p->setBought(true); _repo->update(p); cout << "Вітаємо з покупкою!\n"; }
    }

    void sellProperty(string name) override{
        Property* p = findByName(name);
        if (p) { p->setBought(false); _repo->update(p); cout << "Об'єкт знову виставлено на продаж!\n"; }
    }

    void repairProperty(string name) override{
        Property* p = findByName(name);
        if (p) { cout << "На об'єкті '" << name << "' розпочато ремонтні роботи.\n"; }
    }

    void calculateTotalIncome() override{
        double total = 0;
        for (auto p : _list) if (p->getStatus()) total += p->getPrice() * 0.05; // Наприклад, 5% від ціни
        cout << "Загальний прибуток від оренди: " << total << " грн\n";
    }

    void filterByPrice(double maxPrice) override{
        bool found = false;
        for (auto p : _list) {
            if (p->getPrice() <= maxPrice) { p->show(); found = true; }
        }
        if(!found) cout << "Об'єктів у такому ціновому діапазоні не знайдено.\n";
    }
};

int trueint() {
    int x; 
    while(!(cin >> x)){
        cout << "Помилка! Введіть число: ";
        cin.clear();
        cin.ignore((numeric_limits<streamsize>::max)(), '\n');
    }
    return x;
}

void addObjectUI(shared_ptr<IEstateService> service){
    cout << "Який тип об'єкта додати?\n1. Апартаменти\n2. Комерційна нерухомість\n3. Приватний будинок\nВаш вибір: ";
    int type = trueint();
    
    string name, addr, date, bType;
    double area, price, rent, land;
    int floor, rooms, garageInput;

    cout << "Введіть назву: "; 
    getline(cin >> ws, name);
    cout << "Введіть адресу: ";
    getline(cin >> ws, addr);
    cout << "Введіть площу: ";
    area = (double)trueint();
    cout << "Введіть ціну: ";
    price = (double)trueint();
    cout << "Введіть ціну оренди: ";
    rent = (double)trueint();

    if(type == 1){
        cout << "Введіть поверх: ";
        floor = trueint();
        cout << "Введіть к-сть кімнат: ";
        rooms = trueint();
        cout << "Введіть дату реєстрації паспорта: ";
        cin >> date;
        service->addProperty(new Apartment(name, addr, area, price, rent, floor, rooms, date));
    } 
    else if(type == 2){
        cout << "Введіть тип бізнесу (напр. IT, Магазин): ";
        cin >> bType;
        service->addProperty(new Commercial(name, addr, area, price, rent, bType));
    }
    else if(type == 3){
        cout << "Введіть площу ділянки (соток): ";
        land = (double)trueint();
        cout << "Чи є гараж? (1 - Так, 0 - Ні): ";
        garageInput = trueint();
        service->addProperty(new ownHouse(name, addr, area, price, rent, land, garageInput == 1));
    }
    cout << "Об'єкт успішно додано!\n";
}

//#ifndef TEST_MODE
int main(){
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);

    Hypodermic::ContainerBuilder builder;
    builder.registerType<SQLiteRepository>().as<IRepository>().singleInstance();
    builder.registerType<EstateService>().as<IEstateService>().singleInstance();
    auto container = builder.build();
    auto service = container->resolve<IEstateService>();
    service->loadData();

    int choice = -1;
    while (choice != 0) {
        cout << "\n--- Керування нерухомістю ---\n";
        cout << "1. Показати всі об'єкти\n2. Змінити ціну (за назвою)\n3. Видалити об'єкт\n4. Здійснити ремонт\n";
        cout << "5. Додати новий об'єкт\n6. Здати в оренду/Повернути\n7. Придбати об'єкт\n8. Розрахувати прибуток\n";
        cout << "9. Продати об'єкт\n10. Загальна кількість об'єктів\n11. Фільтр за ціною\n0. Вихід\nВаш вибір: ";
        
        choice = trueint();
        string name;
        switch(choice){
            case 1:
                service->showAll(); 
                break;
            case 2:{
                cout << "Введіть назву: ";
                cin >> name;
                cout << "Нова ціна: ";
                service->updatePrice(name, (double)trueint());
                break;
            }
            case 3:{
                cout << "Введіть назву для видалення: ";
                cin >> name;
                service->deleteProperty(name);
                break;
            }
            case 4:{
                cout << "Введіть назву об'єкта для ремонту: ";
                cin >> name;
                service->repairProperty(name);
                break;
            case 5:
                addObjectUI(service);
                break;
            case 6:{
                cout << "Введіть назву об'єкта: ";
                cin >> name;
                service->toggleRent(name);
                break;
            }
            case 7:{
                cout << "Введіть назву об'єкта для купівлі: ";
                cin >> name;
                service->buyProperty(name);
                break;
            }
            case 8:
                service->calculateTotalIncome();
                break;
            case 9:{
                cout << "Введіть назву об'єкта для продажу: ";
                cin >> name;
                service->sellProperty(name);
                break;
            }
            case 10:{
                cout << "Загальна кількість об'єктів у системі: " << Property::totalObjects << endl;
                break;
            }
            case 11:{
                cout << "Введіть максимальну ціну: ";
                service->filterByPrice((double)trueint());
                break;
            }
            case 0:{
                cout << "До зустрічі!\n";
                break;
            }
            default:{
                cout << "Неправильний вибір!\n";
                break;
            }
        }
    }
}
    return 0;
}
//#endif