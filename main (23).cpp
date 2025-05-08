#include <iostream>
#include <string>
#include <cstdlib>

#include "ServiceRegistry.hpp"
#include "Balancer.hpp"
#include "HttpListener.hpp"

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Использование: " << argv[0] << " <services.json> <порт> <режим: rr|rnd>\n";
        return 1;
    }

    std::string configPath = argv[1];       
    int port = std::atoi(argv[2]);          
    std::string mode = argv[3];             

    if (mode != "rr" && mode != "rnd") {
        std::cerr << "Ошибка: режим должен быть rr (round robin) или rnd (random)\n";
        return 1;
    }

    try {
        ServiceRegistry registry(configPath);   
        Balancer balancer(registry, mode);      
        HttpListener listener(port);            

        std::cout << "Балансировщик запущен на порту " << port << " в режиме " << mode << "\n";

        listener.run();                         
    } catch (const std::exception& ex) {
        std::cerr << "Ошибка: " << ex.what() << "\n";
        return 1;
    }

    return 0;
}
