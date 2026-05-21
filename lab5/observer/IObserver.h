#pragma once
#include <string>
using namespace std;

class IObserver{
public:
    virtual void onEvent(const string& eventType, const string& propertyName) = 0;
    virtual ~IObserver() {}
};
