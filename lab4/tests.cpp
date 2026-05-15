#define CATCH_CONFIG_MAIN
#define TEST_MODE

#include "catch.hpp"

#include "models/Property.h"
#include "models/Apartment.h"
#include "models/Commercial.h"
#include "models/OwnHouse.h"
#include "repository/IRepository.h"
#include "service/EstateService.h"
#include "utils/DoublyLinkedList.h"

using namespace std;

class MockRepository : public IRepository {
public:
    void save(Property*)                       override {}
    void update(Property*)                     override {}
    void remove(string)                        override {}
    void loadAll(DoublyLinkedList<Property*>&) override {}
};

TEST_CASE("DoublyLinkedList — базові операції", "[list]") {
    DoublyLinkedList<int> list;

    REQUIRE(list.size() == 0);

    list.push_back(10);
    list.push_back(20);
    REQUIRE(list.size() == 2);

    list.remove(10);
    REQUIRE(list.size() == 1);

    list.clear();
    REQUIRE(list.size() == 0);
}

TEST_CASE("EstateService — додавання та видалення (позитивний)", "[service]") {
    auto mockRepo = make_shared<MockRepository>();
    EstateService service(mockRepo);

    Property* apt = new Apartment("TestApt", "Вул. Тестова", 50, 1000, 100, 1, 2, "01.01.2025");
    service.addProperty(apt);

    REQUIRE(service.findByName("TestApt") != nullptr);

    service.deleteProperty("TestApt");
    REQUIRE(service.findByName("TestApt") == nullptr);
}

TEST_CASE("EstateService — пошук неіснуючого об'єкта (негативний)", "[service]") {
    auto mockRepo = make_shared<MockRepository>();
    EstateService service(mockRepo);

    REQUIRE(service.findByName("НеіснуючийОб'єкт") == nullptr);
}

TEST_CASE("Property::totalObjects — коректний підрахунок", "[model]") {
    int before = Property::totalObjects;

    Property* a = new Apartment("A1", "Адреса 1", 40, 500, 50, 2, 1, "01.01");
    Property* c = new Commercial("C1", "Адреса 2", 80, 2000, 200, "IT");
    REQUIRE(Property::totalObjects == before + 2);

    delete a;
    REQUIRE(Property::totalObjects == before + 1);

    delete c;
    REQUIRE(Property::totalObjects == before);
}

TEST_CASE("EstateService — оренда та купівля (позитивний)", "[service]") {
    auto mockRepo = make_shared<MockRepository>();
    EstateService service(mockRepo);

    Property* h = new OwnHouse("House1", "Вул. Лісова", 120, 3000, 300, 10, true);
    service.addProperty(h);

    REQUIRE(service.findByName("House1")->getStatus()    == false);
    REQUIRE(service.findByName("House1")->getBoughtness()== false);

    service.toggleRent("House1");
    REQUIRE(service.findByName("House1")->getStatus() == true);

    service.buyProperty("House1");
    REQUIRE(service.findByName("House1")->getBoughtness() == true);

    service.sellProperty("House1");
    REQUIRE(service.findByName("House1")->getBoughtness() == false);

    service.deleteProperty("House1");
}

TEST_CASE("EstateService — зміна ціни (позитивний)", "[service]") {
    auto mockRepo = make_shared<MockRepository>();
    EstateService service(mockRepo);

    service.addProperty(new Commercial("Office1", "Центр", 60, 1500, 150, "Магазин"));

    service.updatePrice("Office1", 2500);
    REQUIRE(service.findByName("Office1")->getPrice() == 2500);

    service.deleteProperty("Office1");
}

TEST_CASE("EstateService — видалення неіснуючого (негативний)", "[service]") {
    auto mockRepo = make_shared<MockRepository>();
    EstateService service(mockRepo);

    REQUIRE_NOTHROW(service.deleteProperty("Примара"));
}
