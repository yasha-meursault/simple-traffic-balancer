#include "registry.hpp"
#include "listener.hpp"
#include "health.cpp"  // we include .cpp directly here for simplicity
#include <iostream>

int main() {
    try {
        loadConfig("services.json");
        startHealthCheckThread();
        run_listener();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}