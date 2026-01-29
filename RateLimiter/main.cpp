#include <bits/stdc++.h>
using namespace std;

// ---------------- Interface ----------------
class RateLimiter {
public:
    virtual bool allowRequest(const string& userId) = 0;
    virtual ~RateLimiter() = default;
};

// ---------------- Config ----------------
enum class AlgorithmType {
    COUNTER,
    SLIDING_WINDOW,
    TOKEN_BUCKET,
    LEAKY_BUCKET
};

struct RateLimitConfig {
    int maxRequests;
    int timeWindow;
    AlgorithmType algorithm;
};

// ---------------- Counter Limiter ----------------
class CounterRateLimiter : public RateLimiter {
private:
    unordered_map<string, int> counter;
    unordered_map<string, time_t> windowStart;
    int maxRequests;
    int windowSize;

public:
    CounterRateLimiter(int maxReq, int window)
        : maxRequests(maxReq), windowSize(window) {}

    bool allowRequest(const string& userId) override {
        time_t now = time(nullptr);

        if (windowStart[userId] == 0 ||
            difftime(now, windowStart[userId]) >= windowSize) {
            windowStart[userId] = now;
            counter[userId] = 0;
        }

        if (counter[userId] < maxRequests) {
            counter[userId]++;
            return true;
        }
        return false;
    }
};

// ---------------- Factory ----------------
class RateLimiterFactory {
public:
    static unique_ptr<RateLimiter>
    createLimiter(const RateLimitConfig& config) {
        switch (config.algorithm) {
        case AlgorithmType::COUNTER:
            return make_unique<CounterRateLimiter>(
                config.maxRequests, config.timeWindow);
        default:
            throw invalid_argument("Unsupported algorithm");
        }
    }
};

// ---------------- Service ----------------
class RateLimiterService {
private:
    unordered_map<string, RateLimitConfig> configs;
    unordered_map<string, unique_ptr<RateLimiter>> limiters;

public:
    void addConfig(const string& userId, const RateLimitConfig& config) {
        configs[userId] = config;
    }

    bool handleRequest(const string& userId) {
        if (limiters.find(userId) == limiters.end()) {
            limiters[userId] =
                RateLimiterFactory::createLimiter(configs[userId]);
        }
        return limiters[userId]->allowRequest(userId);
    }
};

// ---------------- Main ----------------
int main() {
    RateLimiterService service;

    RateLimitConfig config{5, 10, AlgorithmType::COUNTER};
    service.addConfig("user_1", config);

    cout << "Counter Rate Limiter Test (5 req / 10 sec)\n";

    for (int i = 1; i <= 7; i++) {
        bool allowed = service.handleRequest("user_1");
        cout << "Request " << i << " -> "
             << (allowed ? "ALLOWED" : "REJECTED") << endl;
        this_thread::sleep_for(chrono::seconds(1));
    }

    cout << "\nWaiting for window reset...\n";
    this_thread::sleep_for(chrono::seconds(4));

    cout << "Request after reset -> "
         << (service.handleRequest("user_1") ? "ALLOWED" : "REJECTED")
         << endl;

    return 0;
}
