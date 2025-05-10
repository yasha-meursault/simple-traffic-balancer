#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <atomic>
#include <mutex>
#include <stdexcept>
#include <random>

#include "registry.hpp"

class NoHealthyEndpoints : public std::runtime_error {
public:
    explicit NoHealthyEndpoints(const std::string& service)
        : std::runtime_error("No healthy endpoints available for service: " + service) {}
};

class Balancer {
public:
    enum class Algo { RoundRobin, Random };

    Balancer(Algo algorithm);
    [[nodiscard]] Endpoint selectEndpoint(const std::string& service);
    [[nodiscard]] size_t getSuccessfulSelections() const noexcept;
    [[nodiscard]] size_t getFailedSelections() const noexcept;

private:
    Algo algo_;
    std::unordered_map<std::string, std::atomic_size_t> rrIdx_;
    std::mutex rrIdxMutex_;
    std::mt19937 rng_{std::random_device{}()};
    std::mutex rngMutex_;
    std::atomic_size_t successfulSelections_{0};
    std::atomic_size_t failedSelections_{0};
};
