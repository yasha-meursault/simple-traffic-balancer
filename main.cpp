#include "registry.hpp"
#include "listener.hpp"
#include <iostream>

int main() {
    try {
        loadConfig("services.json");
        startHealthCheckThread();
        Listener();  // default address
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}