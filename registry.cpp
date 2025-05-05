// registry.cpp
#include "registry.h"
#include <fstream>
#include <sstream>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <string>
#include <ctime>
#include "json.hpp"

using json = nlohmann::json;

std::unordered_map<std::string, std::vector<Endpoint>> serviceMap;
std::mutex registryMutex;

void loadConfig(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) throw std::runtime_error("Could not open config file");

    json j;
    file >> j;

    std::lock_guard<std::mutex> lock(registryMutex);
    for (auto& [service, endpoints] : j.items()) {
        for (const auto& ep : endpoints) {
            std::string addr = ep;
            size_t colon = addr.find(":");
            if (colon == std::string::npos) continue;
            Endpoint endpoint;
            endpoint.host = addr.substr(0, colon);
            endpoint.port = static_cast<uint16_t>(std::stoi(addr.substr(colon + 1)));
            endpoint.healthy = true;
            endpoint.deadUntil = 0;
            serviceMap[service].push_back(endpoint);
        }
    }
}

std::vector<Endpoint> getHealthy(const std::string& serviceName) {
    std::vector<Endpoint> result;
    time_t now = std::time(nullptr);
    std::lock_guard<std::mutex> lock(registryMutex);
    for (const auto& ep : serviceMap[serviceName]) {
        if (ep.healthy && now >= ep.deadUntil) {
            result.push_back(ep);
        }
    }
    return result;
}

std::unordered_map<std::string, std::vector<Endpoint>>& getRegistry() {
    return serviceMap;
}

std::mutex& getRegistryMutex() {
    return registryMutex;
}
