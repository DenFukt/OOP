#pragma once
#include "IObserver.h"
#include <iostream>
#include <fstream>
#include <ctime>
#include <vector>
#include <string>
using namespace std;

class EventLogger : public IObserver{
private:
    ofstream logFile;

    string timestamp(){
        time_t now = time(nullptr);
        char buf[20];
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
        return string(buf);
    }

public:
    explicit EventLogger(const string& filename = "events.log"){
        logFile.open(filename, ios::app);
    }

    void onEvent(const string& eventType, const string& propertyName) override {
        string msg = "[" + timestamp() + "] EVENT=" + eventType
                   + " OBJECT=\"" + propertyName + "\"";
        cout << msg << "\n";
        if (logFile.is_open()) logFile << msg << "\n";
    }

    ~EventLogger() { if (logFile.is_open()) logFile.close(); }
};

class Observable {
protected:
    vector<IObserver*> observers;

public:
    void subscribe(IObserver* obs) { observers.push_back(obs); }
    void unsubscribe(IObserver* obs){
        observers.erase(remove(observers.begin(), observers.end(), obs), observers.end());
    }
    void notify(const string& ev, const string& name){
        for (auto* o : observers) o->onEvent(ev, name);
    }
};
