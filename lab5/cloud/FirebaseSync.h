#pragma once

/*
 *  Firebase Realtime Database sync
 *  ────────────────────────────────────────────────────────────────
 *  Активується коли встановлено змінну середовища FIREBASE_URL:
 *    export FIREBASE_URL="https://<project>-default-rtdb.firebaseio.com"
 *
 *  REST API Firebase:
 *    PUT  /properties/<name>.json  → зберегти/оновити
 *    DELETE /properties/<name>.json → видалити
 *
 *  Використовуємо curl через system() — простий, надійний для docker
 * ────────────────────────────────────────────────────────────────
 */

#include "ICloudSync.h"
#include <cstdlib>
#include <iostream>
using namespace std;

class FirebaseSync : public ICloudSync {
private:
    string baseUrl;
    bool   _enabled;

    // Екранує лапки у JSON для передачі в shell
    string escape(const string& s) {
        string r;
        for (char c : s) {
            if (c == '\'') r += "'\\''";
            else r += c;
        }
        return r;
    }

    void curlPut(const string& endpoint, const string& json) {
        string cmd = "curl -s -X PUT "
                     "-H \"Content-Type: application/json\" "
                     "-d '" + escape(json) + "' "
                     "\"" + baseUrl + endpoint + "\" > /dev/null 2>&1";
        system(cmd.c_str());
    }

    void curlDelete(const string& endpoint) {
        string cmd = "curl -s -X DELETE \"" + baseUrl + endpoint + "\" > /dev/null 2>&1";
        system(cmd.c_str());
    }

public:
    FirebaseSync() {
        const char* url = getenv("FIREBASE_URL");
        _enabled = (url != nullptr && string(url).size() > 5);
        if (_enabled) {
            baseUrl = string(url);
            cout << "[Cloud] Firebase sync увімкнено → " << baseUrl << "\n";
        } else {
            cout << "[Cloud] Firebase sync вимкнено (FIREBASE_URL не задано)\n";
        }
    }

    bool isEnabled() const override { return _enabled; }

    void syncSave(const string& name, const string& json) override {
        if (!_enabled) return;
        curlPut("/properties/" + name + ".json", json);
        cout << "[Cloud] Синхронізовано: " << name << "\n";
    }

    void syncUpdate(const string& name, const string& json) override {
        syncSave(name, json); // PUT ідемпотентний — те саме що й save
    }

    void syncDelete(const string& name) override {
        if (!_enabled) return;
        curlDelete("/properties/" + name + ".json");
        cout << "[Cloud] Видалено з хмари: " << name << "\n";
    }
};
