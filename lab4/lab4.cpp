#include <iostream>
#include <string>
#include <ios>
#include <limits>
#include <memory>

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

#include "models/Property.h"
#include "models/Apartment.h"
#include "models/Commercial.h"
#include "models/OwnHouse.h"
#include "repository/IRepository.h"
#include "repository/SQLiteRepository.h"
#include "service/IEstateService.h"
#include "service/EstateService.h"

#include "Hypodermic/Hypodermic.h"

using namespace std;

int trueint() {
    int x;
    while(!(cin >> x)){
        cout << "Помилка! Введіть число: ";
        cin.clear();
        cin.ignore((numeric_limits<streamsize>::max)(), '\n');
    }
    return x;
}

void addObjectUI(shared_ptr<IEstateService> service) {
    cout << "Який тип об'єкта додати?\n"
         << "1. Апартаменти\n"
         << "2. Комерційна нерухомість\n"
         << "3. Приватний будинок\n"
         << "Ваш вибір: ";
    int type = trueint();

    string name, addr, date, bType;
    double area, price, rent, land;
    int floor, rooms, garageInput;

    cout << "Введіть назву: ";
    getline(cin >> ws, name);
    cout << "Введіть адресу: ";
    getline(cin >> ws, addr);
    cout << "Введіть площу: ";
    area  = (double)trueint();
    cout << "Введіть ціну: ";
    price = (double)trueint();
    cout << "Введіть ціну оренди: ";
    rent = (double)trueint();

    if(type == 1){
        cout << "Введіть поверх: ";
        floor = trueint();
        cout << "Введіть к-сть кімнат: ";
        rooms = trueint();
        cout << "Введіть дату реєстрації: ";
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
        service->addProperty(new OwnHouse(name, addr, area, price, rent, land, garageInput == 1));
    }
    else{
        cout << "Невірний тип!\n";
        return;
    }
    cout << "Об'єкт успішно додано!\n";
}

#ifndef TEST_MODE
int main() {
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
        cout << "\n--- Керування нерухомістю ---\n"
             << " 1. Показати всі об'єкти\n"
             << " 2. Змінити ціну (за назвою)\n"
             << " 3. Видалити об'єкт\n"
             << " 4. Здійснити ремонт\n"
             << " 5. Додати новий об'єкт\n"
             << " 6. Здати в оренду / Повернути\n"
             << " 7. Придбати об'єкт\n"
             << " 8. Розрахувати прибуток від оренди\n"
             << " 9. Продати об'єкт\n"
             << "10. Загальна кількість об'єктів\n"
             << "11. Фільтр за ціною\n"
             << " 0. Вихід\n"
             << "Ваш вибір: ";

        choice = trueint();
        string name;

        switch (choice) {
            case 1:
                service->showAll();
                break;
            case 2:
                cout << "Введіть назву: ";
                cin >> name;
                cout << "Нова ціна: ";
                service->updatePrice(name, (double)trueint());
                break;
            case 3:
                cout << "Введіть назву для видалення: ";
                cin >> name;
                service->deleteProperty(name);
                break;
            case 4:
                cout << "Введіть назву об'єкта для ремонту: ";
                cin >> name;
                service->repairProperty(name);
                break;
            case 5:
                addObjectUI(service);
                break;
            case 6:
                cout << "Введіть назву об'єкта: ";
                cin >> name;
                service->toggleRent(name);
                break;
            case 7:
                cout << "Введіть назву для купівлі: ";
                cin >> name;
                service->buyProperty(name);
                break;
            case 8:
                service->calculateTotalIncome();
                break;
            case 9:
                cout << "Введіть назву для продажу: ";
                cin >> name;
                service->sellProperty(name);
                break;
            case 10:
                cout << "Загальна кількість об'єктів: " << Property::totalObjects << endl;
                break;
            case 11:
                cout << "Введіть максимальну ціну: ";
                service->filterByPrice((double)trueint());
                break;
            case 0:
                cout << "До зустрічі!\n";
                break;
            default:
                cout << "Неправильний вибір!\n";
                break;
        }
    }
    return 0;
}
#endif
