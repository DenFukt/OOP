#pragma once
#include <string>
#include <map>
#include <vector>
#include <sstream>
using namespace std;

// ─── Мінімальний JSON-хелпер ─────────────────────────────────────────────────
namespace Json {

// Екранування рядка
inline string str(const string& s) {
    string r = "\"";
    for (char c : s) {
        if      (c == '"')  r += "\\\"";
        else if (c == '\\') r += "\\\\";
        else if (c == '\n') r += "\\n";
        else if (c == '\r') r += "\\r";
        else                r += c;
    }
    return r + "\"";
}

inline string num(double n) {
    ostringstream oss;
    oss << n;
    return oss.str();
}

inline string num(int n)  { return to_string(n); }
inline string boolean(bool b) { return b ? "true" : "false"; }

// Будує об'єкт з пар key→value (значення вже мають бути JSON-рядками)
inline string obj(vector<pair<string,string>> fields) {
    string r = "{";
    for (size_t i = 0; i < fields.size(); ++i) {
        r += "\"" + fields[i].first + "\":" + fields[i].second;
        if (i + 1 < fields.size()) r += ",";
    }
    return r + "}";
}

// Будує масив з готових JSON-рядків
inline string arr(const vector<string>& items) {
    string r = "[";
    for (size_t i = 0; i < items.size(); ++i) {
        r += items[i];
        if (i + 1 < items.size()) r += ",";
    }
    return r + "]";
}

// ─── Мінімальний парсер ───────────────────────────────────────────────────────
// Повертає map<key, value> де value — сирий рядок (без зовнішніх лапок для рядків)
inline map<string,string> parse(const string& body) {
    map<string,string> result;
    size_t i = 0;
    auto skip = [&]{ while (i < body.size() && (body[i]==' '||body[i]=='\n'||body[i]=='\r'||body[i]=='\t')) i++; };

    skip();
    if (i >= body.size() || body[i] != '{') return result;
    i++; // skip {

    while (i < body.size() && body[i] != '}') {
        skip();
        if (body[i] != '"') break;
        i++; // skip opening "
        string key;
        while (i < body.size() && body[i] != '"') key += body[i++];
        i++; // skip closing "
        skip();
        if (body[i] != ':') break;
        i++; // skip :
        skip();

        string val;
        if (body[i] == '"') {
            i++;
            while (i < body.size() && body[i] != '"') {
                if (body[i] == '\\' && i+1 < body.size()) { i++; val += body[i]; }
                else val += body[i];
                i++;
            }
            i++; // skip closing "
        } else {
            // number / boolean / null
            while (i < body.size() && body[i]!=',' && body[i]!='}') val += body[i++];
            // trim
            while (!val.empty() && (val.back()==' '||val.back()=='\n')) val.pop_back();
        }
        result[key] = val;
        skip();
        if (i < body.size() && body[i] == ',') i++;
        skip();
    }
    return result;
}

} // namespace Json
