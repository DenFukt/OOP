#pragma once
#include <string>
#include <unordered_map>
#include <atomic>
#include <chrono>
#include <sstream>
#include <iomanip>
using namespace std;
using namespace std::chrono;

struct EndpointStat {
    atomic<long>     hits{0};
    atomic<long long> totalMs{0};

    // atomic не копіюється — потрібен move
    EndpointStat() = default;
    EndpointStat(EndpointStat&&) noexcept {}
};

// ─── ApiInsights ─────────────────────────────────────────────────────────────
// Збирає статистику по кожному маршруту: кількість запитів, середній час.
// GET /api/insights повертає JSON.
// Дані живуть у пам'яті — скидаються при рестарті контейнера.

class ApiInsights {
private:
    unordered_map<string, EndpointStat> stats;
    unordered_map<string, long>         hitsSnapshot; // для JSON-виводу
    unordered_map<string, double>       avgMsSnapshot;
    atomic<long>     totalRequests{0};
    steady_clock::time_point startTime;

public:
    ApiInsights() : startTime(steady_clock::now()) {}

    // Викликається ПЕРЕД обробкою запиту
    steady_clock::time_point begin(const string& route) {
        totalRequests++;
        stats[route].hits++;
        return steady_clock::now();
    }

    // Викликається ПІСЛЯ обробки запиту
    void end(const string& route, const steady_clock::time_point& t0) {
        auto ms = duration_cast<milliseconds>(steady_clock::now() - t0).count();
        stats[route].totalMs += ms;
    }

    string toJson() {
        long upSec = duration_cast<seconds>(
                         steady_clock::now() - startTime).count();

        ostringstream oss;
        oss << fixed << setprecision(2);

        oss << "{"
            << "\"totalRequests\":" << totalRequests.load() << ","
            << "\"uptimeSeconds\":"  << upSec << ","
            << "\"endpoints\":[";

        bool first = true;
        for (auto& [route, stat] : stats) {
            long   hits = stat.hits.load();
            long long ms = stat.totalMs.load();
            double avg  = hits > 0 ? (double)ms / hits : 0.0;

            if (!first) oss << ",";
            first = false;
            oss << "{\"route\":\""  << route << "\","
                << "\"hits\":"       << hits  << ","
                << "\"avgMs\":"      << avg   << "}";
        }

        oss << "]}";
        return oss.str();
    }
};
