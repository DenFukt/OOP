#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "models/Property.h"
#include "models/Apartment.h"
#include "models/Commercial.h"
#include "models/OwnHouse.h"
#include "utils/DoublyLinkedList.h"
#include "utils/JsonParser.h"
#include "factory/PropertyFactory.h"
#include "repository/IRepository.h"
#include "observer/IObserver.h"
#include "observer/EventLogger.h"
#include "service/EstateService.h"

#include <stdexcept>
#include <vector>
#include <string>
using namespace std;

// ── Mock-репозиторій ──────────────────────────────────────────────────────────
class MockRepo : public IRepository {
public:
    void save   (Property*)                       override {}
    void update (Property*)                       override {}
    void remove (string)                          override {}
    void loadAll(DoublyLinkedList<Property*>&)    override {}
};

// ── Mock-спостерігач ──────────────────────────────────────────────────────────
class MockObserver : public IObserver {
public:
    vector<pair<string,string>> events;
    void onEvent(const string& ev, const string& name) override {
        events.push_back({ev, name});
    }
};

// ── Mock-хмара ────────────────────────────────────────────────────────────────
class MockCloud : public ICloudSync {
public:
    int saveCount=0, updateCount=0, deleteCount=0;
    bool isEnabled() const override { return true; }
    void syncSave  (const string&, const string&) override { saveCount++;   }
    void syncUpdate(const string&, const string&) override { updateCount++; }
    void syncDelete(const string&)                override { deleteCount++; }
};

// ════════════════════════════════════════════════════════════════════
// 1. PropertyFactory (Creational pattern)
// ════════════════════════════════════════════════════════════════════
TEST_CASE("PropertyFactory — Apartment (позитивний)", "[factory]") {
    PropertyParams p;
    p.type="Apartment"; p.name="Test"; p.address="вул.1";
    p.area=50; p.price=1000; p.rentPrice=100; p.floor=2; p.rooms=2; p.date="01.01";
    auto* obj = PropertyFactory::create(p);
    REQUIRE(obj != nullptr);
    REQUIRE(obj->getType() == "Apartment");
    REQUIRE(obj->getName() == "Test");
    delete obj;
}

TEST_CASE("PropertyFactory — Commercial (позитивний)", "[factory]") {
    PropertyParams p;
    p.type="Commercial"; p.name="Office"; p.address="вул.2";
    p.area=80; p.price=2000; p.rentPrice=200; p.businessType="IT";
    auto* obj = PropertyFactory::create(p);
    REQUIRE(obj != nullptr);
    REQUIRE(obj->getType() == "Commercial");
    delete obj;
}

TEST_CASE("PropertyFactory — OwnHouse (позитивний)", "[factory]") {
    PropertyParams p;
    p.type="OwnHouse"; p.name="House"; p.address="вул.3";
    p.area=120; p.price=3000; p.rentPrice=300; p.landArea=10; p.hasGarage=true;
    auto* obj = PropertyFactory::create(p);
    REQUIRE(obj != nullptr);
    REQUIRE(obj->getType() == "OwnHouse");
    delete obj;
}

TEST_CASE("PropertyFactory — невідомий тип кидає виняток (негативний)", "[factory]") {
    PropertyParams p; p.type = "UnknownType"; p.name = "X";
    REQUIRE_THROWS_AS(PropertyFactory::create(p), invalid_argument);
}

// ════════════════════════════════════════════════════════════════════
// 2. DoublyLinkedList
// ════════════════════════════════════════════════════════════════════
TEST_CASE("DoublyLinkedList — push_back і size", "[list]") {
    DoublyLinkedList<int> lst;
    REQUIRE(lst.size() == 0);
    lst.push_back(1); lst.push_back(2); lst.push_back(3);
    REQUIRE(lst.size() == 3);
}

TEST_CASE("DoublyLinkedList — remove існуючого (позитивний)", "[list]") {
    DoublyLinkedList<int> lst;
    lst.push_back(10); lst.push_back(20); lst.push_back(30);
    lst.remove(20);
    REQUIRE(lst.size() == 2);
}

TEST_CASE("DoublyLinkedList — remove неіснуючого не падає (негативний)", "[list]") {
    DoublyLinkedList<int> lst;
    lst.push_back(1);
    REQUIRE_NOTHROW(lst.remove(999));
    REQUIRE(lst.size() == 1);
}

TEST_CASE("DoublyLinkedList — clear очищає список", "[list]") {
    DoublyLinkedList<int> lst;
    lst.push_back(1); lst.push_back(2);
    lst.clear();
    REQUIRE(lst.size() == 0);
}

// ════════════════════════════════════════════════════════════════════
// 3. Observer pattern
// ════════════════════════════════════════════════════════════════════
TEST_CASE("Observer — підписник отримує подію (позитивний)", "[observer]") {
    auto repo = make_shared<MockRepo>();
    EstateService svc(repo);
    MockObserver obs;
    svc.subscribe(&obs);

    PropertyParams p; p.type="Apartment"; p.name="OA"; p.address="A";
    p.area=40; p.price=500; p.rentPrice=50; p.floor=1; p.rooms=1; p.date="";
    svc.addProperty(PropertyFactory::create(p));

    REQUIRE(obs.events.size() == 1);
    REQUIRE(obs.events[0].first  == "PROPERTY_ADDED");
    REQUIRE(obs.events[0].second == "OA");
    svc.deleteProperty("OA");
}

TEST_CASE("Observer — кілька підписників отримують подію", "[observer]") {
    auto repo = make_shared<MockRepo>();
    EstateService svc(repo);
    MockObserver obs1, obs2;
    svc.subscribe(&obs1);
    svc.subscribe(&obs2);

    PropertyParams p; p.type="Commercial"; p.name="CO"; p.address="B";
    p.area=60; p.price=800; p.rentPrice=80; p.businessType="Shop";
    svc.addProperty(PropertyFactory::create(p));

    REQUIRE(obs1.events.size() == 1);
    REQUIRE(obs2.events.size() == 1);
    svc.deleteProperty("CO");
}

TEST_CASE("Observer — без підписників не падає (негативний)", "[observer]") {
    auto repo = make_shared<MockRepo>();
    EstateService svc(repo);
    PropertyParams p; p.type="OwnHouse"; p.name="HX"; p.address="C";
    p.area=80; p.price=600; p.rentPrice=60; p.landArea=5; p.hasGarage=false;
    REQUIRE_NOTHROW(svc.addProperty(PropertyFactory::create(p)));
    svc.deleteProperty("HX");
}

// ════════════════════════════════════════════════════════════════════
// 4. EstateService — CRUD
// ════════════════════════════════════════════════════════════════════
TEST_CASE("EstateService — додавання та пошук (позитивний)", "[service]") {
    auto repo = make_shared<MockRepo>();
    EstateService svc(repo);
    PropertyParams p; p.type="Apartment"; p.name="SearchMe"; p.address="D";
    p.area=45; p.price=900; p.rentPrice=90; p.floor=3; p.rooms=2; p.date="";
    svc.addProperty(PropertyFactory::create(p));
    REQUIRE(svc.findByName("SearchMe") != nullptr);
    svc.deleteProperty("SearchMe");
}

TEST_CASE("EstateService — пошук неіснуючого (негативний)", "[service]") {
    auto repo = make_shared<MockRepo>();
    EstateService svc(repo);
    REQUIRE(svc.findByName("Привид") == nullptr);
}

TEST_CASE("EstateService — видалення (позитивний)", "[service]") {
    auto repo = make_shared<MockRepo>();
    EstateService svc(repo);
    PropertyParams p; p.type="Commercial"; p.name="DelMe"; p.address="E";
    p.area=50; p.price=700; p.rentPrice=70; p.businessType="Bar";
    svc.addProperty(PropertyFactory::create(p));
    svc.deleteProperty("DelMe");
    REQUIRE(svc.findByName("DelMe") == nullptr);
}

TEST_CASE("EstateService — видалення неіснуючого не падає (негативний)", "[service]") {
    auto repo = make_shared<MockRepo>();
    EstateService svc(repo);
    REQUIRE_NOTHROW(svc.deleteProperty("Привид"));
}

TEST_CASE("EstateService — toggleRent (позитивний)", "[service]") {
    auto repo = make_shared<MockRepo>();
    EstateService svc(repo);
    PropertyParams p; p.type="Apartment"; p.name="RentMe"; p.address="F";
    p.area=55; p.price=1100; p.rentPrice=110; p.floor=2; p.rooms=3; p.date="";
    svc.addProperty(PropertyFactory::create(p));
    REQUIRE(svc.findByName("RentMe")->getStatus() == false);
    svc.toggleRent("RentMe");
    REQUIRE(svc.findByName("RentMe")->getStatus() == true);
    svc.toggleRent("RentMe");
    REQUIRE(svc.findByName("RentMe")->getStatus() == false);
    svc.deleteProperty("RentMe");
}

TEST_CASE("EstateService — buy/sell (позитивний)", "[service]") {
    auto repo = make_shared<MockRepo>();
    EstateService svc(repo);
    PropertyParams p; p.type="OwnHouse"; p.name="BuyMe"; p.address="G";
    p.area=100; p.price=2500; p.rentPrice=250; p.landArea=8; p.hasGarage=true;
    svc.addProperty(PropertyFactory::create(p));
    svc.buyProperty("BuyMe");
    REQUIRE(svc.findByName("BuyMe")->getBoughtness() == true);
    svc.sellProperty("BuyMe");
    REQUIRE(svc.findByName("BuyMe")->getBoughtness() == false);
    svc.deleteProperty("BuyMe");
}

TEST_CASE("EstateService — updatePrice (позитивний)", "[service]") {
    auto repo = make_shared<MockRepo>();
    EstateService svc(repo);
    PropertyParams p; p.type="Commercial"; p.name="PriceMe"; p.address="H";
    p.area=70; p.price=1500; p.rentPrice=150; p.businessType="Gym";
    svc.addProperty(PropertyFactory::create(p));
    svc.updatePrice("PriceMe", 3000);
    REQUIRE(svc.findByName("PriceMe")->getPrice() == 3000);
    svc.deleteProperty("PriceMe");
}

TEST_CASE("EstateService — дохід від оренди (позитивний)", "[service]") {
    auto repo = make_shared<MockRepo>();
    EstateService svc(repo);
    PropertyParams p; p.type="Apartment"; p.name="IncMe"; p.address="I";
    p.area=50; p.price=2000; p.rentPrice=200; p.floor=1; p.rooms=1; p.date="";
    svc.addProperty(PropertyFactory::create(p));
    svc.toggleRent("IncMe");
    double income = svc.calculateTotalIncome();
    REQUIRE(income == Approx(100.0)); // 5% від 2000
    svc.deleteProperty("IncMe");
}

TEST_CASE("EstateService — дохід без оренди = 0 (негативний)", "[service]") {
    auto repo = make_shared<MockRepo>();
    EstateService svc(repo);
    REQUIRE(svc.calculateTotalIncome() == Approx(0.0));
}

// ════════════════════════════════════════════════════════════════════
// 5. Cloud sync
// ════════════════════════════════════════════════════════════════════
TEST_CASE("EstateService — хмарна синхронізація при додаванні", "[cloud]") {
    auto repo  = make_shared<MockRepo>();
    auto cloud = make_shared<MockCloud>();
    EstateService svc(repo, cloud);

    PropertyParams p; p.type="Apartment"; p.name="CloudTest"; p.address="J";
    p.area=45; p.price=800; p.rentPrice=80; p.floor=1; p.rooms=1; p.date="";
    svc.addProperty(PropertyFactory::create(p));
    REQUIRE(cloud->saveCount == 1);
    svc.deleteProperty("CloudTest");
    REQUIRE(cloud->deleteCount == 1);
}

// ════════════════════════════════════════════════════════════════════
// 6. Property::totalObjects
// ════════════════════════════════════════════════════════════════════
TEST_CASE("Property::totalObjects — коректний лічильник", "[model]") {
    int before = Property::totalObjects;
    auto* a = new Apartment("X1","A",40,500,50,1,1,"");
    auto* c = new Commercial("X2","B",80,2000,200,"IT");
    REQUIRE(Property::totalObjects == before + 2);
    delete a;
    REQUIRE(Property::totalObjects == before + 1);
    delete c;
    REQUIRE(Property::totalObjects == before);
}

// ════════════════════════════════════════════════════════════════════
// 7. toJson() та JsonParser
// ════════════════════════════════════════════════════════════════════
TEST_CASE("Apartment::toJson() містить потрібні поля", "[json]") {
    Apartment a("AptJson","вул.1",55,1200,120,3,2,"01.01");
    string j = a.toJson();
    REQUIRE(j.find("\"name\":\"AptJson\"") != string::npos);
    REQUIRE(j.find("\"type\":\"Apartment\"") != string::npos);
    REQUIRE(j.find("\"floor\":3") != string::npos);
}

TEST_CASE("JsonParser — витягує поля коректно (позитивний)", "[json]") {
    string json = "{\"type\":\"Apartment\",\"name\":\"Test\",\"price\":1500.5,\"floor\":3}";
    REQUIRE(jsonField (json, "type")  == "Apartment");
    REQUIRE(jsonField (json, "name")  == "Test");
    REQUIRE(jsonDouble(json, "price") == Approx(1500.5));
    REQUIRE(jsonInt   (json, "floor") == 3);
}

TEST_CASE("JsonParser — відсутнє поле повертає порожньо (негативний)", "[json]") {
    string json = "{\"name\":\"Test\"}";
    REQUIRE(jsonField(json, "address") == "");
    REQUIRE(jsonDouble(json, "price")  == Approx(0.0));
}
