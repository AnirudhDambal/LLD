# Rate Limiter – Low Level Design (LLD)

## 📌 Problem Statement

Design a **Rate-Limiting System** that restricts the number of API requests a user can make within a given time window. The system must support **multiple rate-limiting algorithms**, be **configurable per user**, and be **easily extensible**.

This design focuses on **clean architecture**, **thread safety**, and **performance**, making it suitable for real-world backend systems.

---

## 🎯 Goals of the System

* Prevent API abuse and ensure fair usage
* Support different user tiers (Free, Premium, etc.)
* Allow multiple rate-limiting algorithms
* Enable easy extension without changing core logic
* Handle concurrent requests safely

---

## ✅ Functional Requirements

### 1. Configurable Rate Limits

Each user can have a custom rate limit defined by:

* **User ID** – Unique identifier
* **Max Requests** – Allowed requests
* **Time Window** – Duration in seconds
* **Rate Limiting Algorithm** – Counter, Sliding Window, Token Bucket, Leaky Bucket

Different users may use different algorithms depending on their needs.

---

### 2. Request Handling

When a request arrives:

* ✔️ If request is within the limit → **Allow**
* ❌ If request exceeds the limit → **Reject with error**

---

### 3. Supported Rate Limiting Algorithms

#### 🔹 Counter-Based (Fixed Window)

* Maintains a request counter per time window
* Counter resets when the window expires
* Simple and fast, but allows bursts at window boundaries

**Example:**

* Limit: 5 requests / 10 seconds
* 6th request within same window → ❌ Rejected

---

#### 🔹 Sliding Window

* Maintains timestamps of recent requests
* Counts only requests in the last X seconds
* More accurate than fixed window

**Example:**

* Limit: 3 requests / 5 seconds
* Old timestamps are removed as window slides

---

#### 🔹 Token Bucket

* Bucket has tokens that refill at a fixed rate
* Each request consumes one token
* Allows controlled bursts

**Best for:** APIs allowing short spikes

---

#### 🔹 Leaky Bucket

* Requests enter a queue
* Processed at a fixed rate
* Smoothens traffic and avoids bursts

**Best for:** Traffic shaping

---

## 🚀 Non-Functional Requirements

### 1. Extensibility

* New algorithms can be added without modifying existing code
* Uses **Strategy Design Pattern**

### 2. Performance

* Constant or amortized constant time checks
* Efficient data structures: Maps, Queues, Deques

### 3. Concurrency

* Thread-safe request handling
* Atomic updates for counters and tokens
* Supports high concurrent traffic

---

## 🧩 High-Level Architecture

```
Client Request
      ↓
RateLimiterService
      ↓
RateLimiter (Interface)
      ↓
Concrete Algorithm (Counter / Sliding / Token / Leaky)
```

---

## 🏗️ Key Components & Classes

### 1. RateLimiter (Interface)

Defines the contract for all rate-limiting algorithms.

**Responsibilities:**

* Check whether a request is allowed
* Algorithm-agnostic interface

```java
public interface RateLimiter {
    boolean allowRequest(String userId);
}
```

---

### 2. CounterRateLimiter

* Uses a counter per user
* Resets counter after time window expires

---

### 3. SlidingWindowRateLimiter

* Stores timestamps of requests per user
* Removes outdated timestamps

---

### 4. TokenBucketRateLimiter

* Maintains token count per user
* Refills tokens based on elapsed time

---

### 5. LeakyBucketRateLimiter

* Uses a queue per user
* Processes requests at a constant rate

---

### 6. RateLimitConfig

Stores configuration per user.

```java
class RateLimitConfig {
    int maxRequests;
    int timeWindowInSeconds;
    RateLimiterAlgorithm algorithm;
}
```

---

### 7. RateLimiterFactory

* Creates rate limiter instances based on configuration
* Decouples creation logic from business logic

---

## 🧠 Design Patterns Used

| Pattern                 | Purpose                                |
| ----------------------- | -------------------------------------- |
| Strategy                | Plug-and-play rate-limiting algorithms |
| Factory                 | Instantiate correct limiter            |
| Singleton (Optional)    | Central rate-limiter service           |
| Thread-Safe Collections | Handle concurrency                     |

---

## 🧪 Example Flow

1. User sends API request
2. RateLimiterService fetches user's config
3. Factory provides correct RateLimiter
4. `allowRequest()` is called
5. Request is allowed or rejected

---

## ⚙️ Future Enhancements

* Distributed rate limiting (Redis-based)
* Per-endpoint rate limits
* Dynamic config updates
* Monitoring & metrics integration

---

## 🏁 Conclusion

This Rate Limiter LLD demonstrates a **scalable**, **extensible**, and **production-ready** design. It follows SOLID principles and supports multiple algorithms without tight coupling.

Perfect for **machine coding rounds**, **system design interviews**, and **real-world backend systems**.
