#ifndef LISTENER_HPP
#define LISTENER_HPP

#include <string>

// Declare the Listener class and its methods
class Listener {
public:
    Listener(const std::string& address);

private:
    std::string address;
};

#endif // LISTENER_HPP
