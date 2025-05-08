#pragma once
#include "ServiceRegistry.hpp"
#include <string>

class Balancer {
public:
    Balancer(ServiceRegistry& registry, const std::string& mode) {
        
        (void)registry;  
        (void)mode;      
    }
};
