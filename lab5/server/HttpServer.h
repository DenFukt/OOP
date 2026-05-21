#pragma once

#include <string>
#include <map>
#include <functional>
#include <vector>
#include <thread>
#include <sstream>
#include <algorithm>
#include <fstream>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

#include "JsonHelper.h"
using namespace std;

struct HttpRequest {
    string method, path, body;
    map<string, string> headers;
    map<string, string> pathParams;
};

struct HttpResponse {
    int status = 200;
    string contentType = "application/json";
    string body;

    static HttpResponse ok(const string& json) {
        return {200, "application/json", json};
    }
    static HttpResponse notFound(const string& msg = "Not found") {
        return {404, "application/json", Json::obj({{"error", Json::str(msg)}})};
    }
    static HttpResponse badRequest(const string& msg = "Bad request") {
        return {400, "application/json", Json::obj({{"error", Json::str(msg)}})};
    }
    static HttpResponse html(const string& content) {
        return {200, "text/html; charset=utf-8", content};
    }
};

using Handler = function<HttpResponse(const HttpRequest&)>;

struct Route {
    string method;
    vector<string> parts;
    Handler handler;

    bool match(const string& m, const string& url, map<string,string>& params) const {
        if (m != method) return false;
        vector<string> urlParts;
        stringstream ss(url);
        string seg;
        while (getline(ss, seg, '/'))
            if (!seg.empty()) urlParts.push_back(seg);

        if (urlParts.size() != parts.size()) return false;
        for (size_t i = 0; i < parts.size(); i++) {
            if (parts[i][0] == ':') {
                params[parts[i].substr(1)] = urlParts[i];
            } else if (parts[i] != urlParts[i]) {
                return false;
            }
        }
        return true;
    }
};

class HttpServer {
private:
    int _port;
    vector<Route> _routes;
    bool _running = false;

    vector<string> splitPath(const string& path) {
        vector<string> parts;
        stringstream ss(path);
        string seg;
        while (getline(ss, seg, '/'))
            if (!seg.empty()) parts.push_back(seg);
        return parts;
    }

    void addRoute(const string& method, const string& path, Handler h) {
        Route r;
        r.method  = method;
        r.parts   = splitPath(path);
        r.handler = h;
        _routes.push_back(r);
    }

    HttpRequest parseRequest(const string& raw) {
        HttpRequest req;
        istringstream stream(raw);
        string line;

        getline(stream, line);
        if (!line.empty() && line.back() == '\r') line.pop_back();
        istringstream fl(line);
        string version;
        fl >> req.method >> req.path >> version;

        auto q = req.path.find('?');
        if (q != string::npos) req.path = req.path.substr(0, q);

        size_t contentLength = 0;
        while (getline(stream, line)) {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            if (line.empty()) break;
            auto colon = line.find(':');
            if (colon != string::npos) {
                string key = line.substr(0, colon);
                string val = line.substr(colon + 2);
                transform(key.begin(), key.end(), key.begin(), ::tolower);
                req.headers[key] = val;
                if (key == "content-length")
                    contentLength = stoul(val);
            }
        }

        if (contentLength > 0) {
            req.body.resize(contentLength);
            stream.read(&req.body[0], contentLength);
        }
        return req;
    }

    string buildResponse(const HttpResponse& res) {
        static map<int,string> statusText = {
            {200,"OK"},{201,"Created"},{204,"No Content"},
            {400,"Bad Request"},{404,"Not Found"},{500,"Internal Server Error"}
        };
        string status = to_string(res.status) + " " + statusText[res.status];
        string r  = "HTTP/1.1 " + status + "\r\n";
        r += "Content-Type: " + res.contentType + "\r\n";
        r += "Content-Length: " + to_string(res.body.size()) + "\r\n";
        r += "Access-Control-Allow-Origin: *\r\n";
        r += "Access-Control-Allow-Methods: GET, POST, PATCH, DELETE, OPTIONS\r\n";
        r += "Access-Control-Allow-Headers: Content-Type\r\n";
        r += "Connection: close\r\n\r\n";
        r += res.body;
        return r;
    }

    void handleClient(int clientFd) {
        char buf[65536] = {};
        recv(clientFd, buf, sizeof(buf) - 1, 0);
        string raw(buf);

        HttpRequest req = parseRequest(raw);

        if (req.method == "OPTIONS") {
            HttpResponse res;
            res.status = 204;
            res.body   = "";
            string r = buildResponse(res);
            send(clientFd, r.c_str(), r.size(), 0);
            close(clientFd);
            return;
        }

        HttpResponse response = HttpResponse::notFound("Route not found: " + req.path);
        for (auto& route : _routes) {
            map<string,string> params;
            if (route.match(req.method, req.path, params)) {
                req.pathParams = params;
                response = route.handler(req);
                break;
            }
        }

        string r = buildResponse(response);
        send(clientFd, r.c_str(), r.size(), 0);
        close(clientFd);
    }

public:
    explicit HttpServer(int port) : _port(port) {}

    void get(const string& path, Handler h)    { addRoute("GET",    path, h); }
    void post(const string& path, Handler h)   { addRoute("POST",   path, h); }
    void patch(const string& path, Handler h)  { addRoute("PATCH",  path, h); }
    void del(const string& path, Handler h)    { addRoute("DELETE", path, h); }

    void listen() {
        int serverFd = socket(AF_INET, SOCK_STREAM, 0);
        int opt = 1;
        setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        sockaddr_in addr{};
        addr.sin_family      = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port        = htons(_port);

        if (bind(serverFd, (sockaddr*)&addr, sizeof(addr)) < 0) {
            cout << "[Server] Помилка bind на порт " << _port << endl;
            return;
        }
        ::listen(serverFd, 10);
        _running = true;
        cout << "[Server] Запущено на http://localhost:" << _port << endl;

        while (_running) {
            sockaddr_in clientAddr{};
            socklen_t addrLen = sizeof(clientAddr);
            int clientFd = accept(serverFd, (sockaddr*)&clientAddr, &addrLen);
            if (clientFd < 0) continue;

            thread([this, clientFd]() {
                handleClient(clientFd);
            }).detach();
        }
        close(serverFd);
    }

    void stop() { _running = false; }
};
