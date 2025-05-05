// health.cpp
#include "registry.h"
#include <thread>
#include <chrono>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <ctime>

bool isAlive(const std::string& host, uint16_t port, int timeout_ms = 500) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return false;

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, host.c_str(), &addr.sin_addr);

    timeval tv;
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));

    int res = connect(sock, (sockaddr*)&addr, sizeof(addr));
    close(sock);
    return res == 0;
}

void startHealthCheckThread(int intervalSeconds = 10) {
    std::thread([intervalSeconds]() {
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(intervalSeconds));
            time_t now = std::time(nullptr);

            auto& registry = getRegistry();
            auto& mutex = getRegistryMutex();
            std::lock_guard<std::mutex> lock(mutex);

            for (auto& [service, endpoints] : registry) {
                for (auto& ep : endpoints) {
                    if (!ep.healthy && now < ep.deadUntil) continue;
                    if (isAlive(ep.host, ep.port)) {
                        ep.healthy = true;
                        ep.deadUntil = 0;
                    } else {
                        ep.healthy = false;
                        ep.deadUntil = now + 30;
                    }
                }
            }
        }
    }).detach();
}
