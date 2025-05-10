#include "balancer.hpp"
#include "registry.hpp"
#include <chrono>

Balancer::Balancer(Algo algorithm)
    : algo_(algorithm) {}

Endpoint Balancer::selectEndpoint(const std::string& service) {
    auto healthyEndpoints = getHealthy(service);
    if (healthyEndpoints.empty()) {
        ++failedSelections_;
        throw NoHealthyEndpoints(service);
    }

    Endpoint selectedEndpoint;
    if (algo_ == Algo::RoundRobin) {
        std::lock_guard<std::mutex> lock(rrIdxMutex_);
        auto& idx = rrIdx_[service];
        selectedEndpoint = healthyEndpoints[idx++ % healthyEndpoints.size()];
    } else if (algo_ == Algo::Random) {
        std::lock_guard<std::mutex> lock(rngMutex_);
        std::uniform_int_distribution<size_t> dist(0, healthyEndpoints.size() - 1);
        selectedEndpoint = healthyEndpoints[dist(rng_)];
    }

    ++successfulSelections_;
    return selectedEndpoint;
}

size_t Balancer::getSuccessfulSelections() const noexcept {
    return successfulSelections_;
}

size_t Balancer::getFailedSelections() const noexcept {
    return failedSelections_;
}