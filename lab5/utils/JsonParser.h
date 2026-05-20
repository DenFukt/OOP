#pragma once
#include <string>
using namespace std;

// Простий парсер JSON-полів без зовнішніх бібліотек
inline string jsonField(const string& json, const string& key) {
    string search = "\"" + key + "\":";
    size_t pos = json.find(search);
    if (pos == string::npos) return "";
    pos += search.size();
    while (pos < json.size() && json[pos] == ' ') pos++;
    if (pos >= json.size()) return "";

    if (json[pos] == '"') {
        size_t end = json.find('"', pos + 1);
        return (end == string::npos) ? "" : json.substr(pos + 1, end - pos - 1);
    }
    size_t end = json.find_first_of(",}", pos);
    return (end == string::npos) ? json.substr(pos) : json.substr(pos, end - pos);
}

inline double jsonDouble(const string& json, const string& key) {
    string v = jsonField(json, key);
    try { return v.empty() ? 0.0 : stod(v); } catch (...) { return 0.0; }
}

inline int jsonInt(const string& json, const string& key) {
    string v = jsonField(json, key);
    try { return v.empty() ? 0 : stoi(v); } catch (...) { return 0; }
}

inline bool jsonBool(const string& json, const string& key) {
    return jsonField(json, key) == "true";
}
