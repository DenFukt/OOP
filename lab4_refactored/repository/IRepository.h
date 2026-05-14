#pragma once

#include "../models/Property.h"
#include "../utils/DoublyLinkedList.h"
#include <string>

using namespace std;

class IRepository {
public:
    virtual void save(Property* p)                       = 0;
    virtual void update(Property* p)                     = 0;
    virtual void loadAll(DoublyLinkedList<Property*>& list) = 0;
    virtual void remove(string name)                     = 0;
    virtual ~IRepository() {}
};
