#include <bits/stdc++.h>
using namespace std;

// ---------------- Interface ----------------
class RateLimiter
{
public:
    virtual bool allowRequest(const string &userId) = 0;
    virtual ~RateLimiter() = default;
};

// ---------------- Config ----------------
enum class AlgorithmType
{
    COUNTER,
    SLIDING_WINDOW,
    TOKEN_BUCKET,
    LEAKY_BUCKET
};

enum class UserType {
    FREE,
    PREMIUM_1,
    PREMIUM_2,
    PREMIUM_3
};



struct RateLimitConfig
{
    int maxRequests;
    int timeWindow;
    AlgorithmType algorithm;
};

struct User {
    string userId;
    UserType type;
};




// ---------------- Counter Limiter ----------------
class CounterRateLimiter : public RateLimiter
{
private:
    unordered_map<string, int> counter;
    unordered_map<string, time_t> windowStart;
    int maxRequests;
    int windowSize;

public:
    CounterRateLimiter(int maxReq, int window)
        : maxRequests(maxReq), windowSize(window) {}

    bool allowRequest(const string &userId) override
    {
        time_t now = time(nullptr);

        if (windowStart[userId] == 0 ||
            difftime(now, windowStart[userId]) >= windowSize)
        {
            windowStart[userId] = now;
            counter[userId] = 0;
        }

        if (counter[userId] < maxRequests)
        {
            counter[userId]++;
            return true;
        }
        return false;
    }
};

//--------------Slinding Window Limiter ------------------

class SlidingWindowRateLimiter : public RateLimiter
{
private:
    unordered_map<string, queue<time_t>> requests;
    int maxRequests;
    int windowSize; 

public:
    SlidingWindowRateLimiter(int maxReq, int window)
        : maxRequests(maxReq), windowSize(window) {}

    bool allowRequest(const string &userId) override
    {
        time_t now = time(nullptr);

        while (!requests[userId].empty() &&
               difftime(now, requests[userId].front()) >= windowSize)
        {
            requests[userId].pop();
        }

        if (requests[userId].size() >= maxRequests)
            return false;

        requests[userId].push(now);
        return true;
    }

    ~SlidingWindowRateLimiter() = default;
};

//-------------------token-bucket-----------------------------
class TokenBucketRateLimiter : public RateLimiter {
private:
    unordered_map<string, int> tokens;
    unordered_map<string, time_t> lastRefill;

    int maxTokens;
    int windowSize; 

public:
    TokenBucketRateLimiter(int maxReq, int window)
        : maxTokens(maxReq), windowSize(window) {}

    bool allowRequest(const string& userId) override {
        time_t now = time(nullptr);
        if (lastRefill.find(userId) == lastRefill.end()) {
            tokens[userId] = maxTokens;
            lastRefill[userId] = now;
        }
        double elapsed = difftime(now, lastRefill[userId]);
        double refillRate = (double)maxTokens / windowSize;

        int newTokens = (int)(elapsed * refillRate);

        if (newTokens > 0) {
            tokens[userId] = min(maxTokens, tokens[userId] + newTokens);
            lastRefill[userId] = now;
        }
        if (tokens[userId] > 0) {
            tokens[userId]--;
            return true;
        }

        return false;
    }

    ~TokenBucketRateLimiter() = default;
};
//---------------------Rate limit policy class ----------------------
class RateLimitPolicy {
public:
    static RateLimitConfig getConfig(UserType type) {
        switch (type) {

        case UserType::FREE:
            return {5, 10, AlgorithmType::COUNTER};

        case UserType::PREMIUM_1:
            return {20, 30, AlgorithmType::SLIDING_WINDOW};

        case UserType::PREMIUM_2:
            return {50, 60, AlgorithmType::TOKEN_BUCKET};

        case UserType::PREMIUM_3:
            return {100, 60, AlgorithmType::TOKEN_BUCKET};

        default:
            throw invalid_argument("Unknown user type");
        }
    }
};




// ---------------- Factory ----------------
class RateLimiterFactory
{
public:
    static unique_ptr<RateLimiter>
    createLimiter(const RateLimitConfig &config)
    {
        switch (config.algorithm)
        {
        case AlgorithmType::COUNTER:
            return make_unique<CounterRateLimiter>(
                config.maxRequests, config.timeWindow);
        case AlgorithmType::SLIDING_WINDOW:
            return make_unique<SlidingWindowRateLimiter>(
                config.maxRequests,
                config.timeWindow);
        case AlgorithmType::TOKEN_BUCKET:
            return make_unique<TokenBucketRateLimiter>(
                config.maxRequests,
                config.timeWindow);
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
    bool handleRequest(const User& user) {
        if (configs.find(user.userId) == configs.end()) {
            configs[user.userId] =
                RateLimitPolicy::getConfig(user.type);
        }

        if (limiters.find(user.userId) == limiters.end()) {
            limiters[user.userId] =
                RateLimiterFactory::createLimiter(configs[user.userId]);
        }

        return limiters[user.userId]->allowRequest(user.userId);
    }

    void resetLimiter(const string& userId) {
        configs.erase(userId);
        limiters.erase(userId);
    }   
};



// --------------------------Tier upgrade Service ------------------
class TierUpgradeService {
public:
    static void upgradeUser(
        User& user,
        UserType newTier,
        RateLimiterService& rateLimiterService
    ) {
        user.type = newTier;
        rateLimiterService.resetLimiter(user.userId);

        cout << "User " << user.userId
             << " upgraded to new tier\n";
    }
};









// ---------------- Main ----------------
int main() {
    RateLimiterService rateLimiterService;

    User user{"user_1", UserType::FREE};

    for (int i = 0; i < 7; i++) {
        cout << "Request " << i << ":"<<(rateLimiterService.handleRequest(user) ? "Accepted" : "rejected") << endl;
    }

    cout << "---- UPGRADING USER ----\n";

    TierUpgradeService::upgradeUser(
        user,
        UserType::PREMIUM_2,
        rateLimiterService
    );
    for (int i = 0; i < 100; i++) {
        cout << "Request " << i << ":"<<(rateLimiterService.handleRequest(user) ? "Accepted" : "rejected") << endl;
    }
}

