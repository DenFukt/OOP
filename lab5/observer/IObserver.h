#pragma once
#include <string>
using namespace std;

// ═══════════════════════════════════════════════════════════════════
//  ПАТЕРН: Observer (Behavioral)
//  Дозволяє сервісу сповіщати підписників про події (додавання,
//  оренда, покупка, видалення) без жорсткої прив'язки до них.
// ═══════════════════════════════════════════════════════════════════
class IObserver {
public:
    virtual void onEvent(const string& eventType, const string& propertyName) = 0;
    virtual ~IObserver() {}
};
