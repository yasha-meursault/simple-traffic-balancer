#ifndef LISTENER_HPP
#define LISTENER_HPP

#include <string>

void run_listener();

class Listener {
public:
    Listener(const std::string& address = "127.0.0.1");
};

#endif // LISTENER_HPP