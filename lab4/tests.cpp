#define CATCH_CONFIG_MAIN 
#define TEST_MODE         

#include "catch.hpp"
#include "lab4.cpp" 

class MockRepository : public IRepository {
public:
    void save(Property* p) override {}
    void update(Property* p) override {}
    void remove(string name) override {}
    void loadAll(DoublyLinkedList<Property*>& list) override {}
};


TEST_CASE("Перевірка роботи DoublyLinkedList", "[list]") {
    DoublyLinkedList<int> list;

    REQUIRE(list.size() == 0);

    list.push_back(10);
    list.push_back(20);

    REQUIRE(list.size() == 2);

    list.clear();
    REQUIRE(list.size() == 0);
}

TEST_CASE("Перевірка пошуку та видалення у EstateService", "[service]") {
    shared_ptr<IRepository> mockRepo = make_shared<MockRepository>();
    EstateService service(mockRepo);
    
    Property* apt = new Apartment("TestApt", "Вул. Тестова", 50, 1000, 100, 1, 1, "01.01");
    service.addProperty(apt);

    REQUIRE(service.findByName("TestApt") != nullptr);

    REQUIRE(service.findByName("SomeHouse") == nullptr);

    service.deleteProperty("TestApt");
    REQUIRE(service.findByName("TestApt") == nullptr);
}