#pragma once
#include <string>
using namespace std;

class ICloudSync {
public:
    virtual bool isEnabled()  const = 0;
    virtual void syncSave  (const string& name, const string& json) = 0;
    virtual void syncUpdate(const string& name, const string& json) = 0;
    virtual void syncDelete(const string& name) = 0;
    virtual ~ICloudSync() {}
};
