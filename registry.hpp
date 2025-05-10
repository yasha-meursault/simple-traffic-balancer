#ifndef REGISTRY_H
#define REGISTRY_H

#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <ctime>

struct Endpoint {
    std::string host;
    uint16_t port;
    bool healthy;
    time_t deadUntil;
};

void loadConfig(const std::string& filename);
std::vector<Endpoint> getHealthy(const std::string& serviceName);
std::unordered_map<std::string, std::vector<Endpoint>>& getRegistry();
std::mutex& getRegistryMutex();
void startHealthCheckThread(int intervalSeconds = 10);

#endif // REGISTRY_H