#include "httplib.h"
#include "listener.hpp"
#include "balancer.hpp"
#include "registry.hpp"
#include <iostream>
#include <memory>

Balancer globalBalancer(Balancer::Algo::RoundRobin); // Global balancer

void run_listener() {
    httplib::Server server;

    auto handler = [](const httplib::Request &req, httplib::Response &res) {
        std::cout << req.method << " " << req.path << std::endl;

        auto pos = req.path.find('/', 1);
        if (pos == std::string::npos) {
            res.status = 400;
            res.set_content("Bad request", "text/plain");
            return;
        }

        std::string service = req.path.substr(1, pos - 1);
        std::string path = req.path.substr(pos);

        Endpoint ep;
        try {
            ep = globalBalancer.selectEndpoint(service);
            std::cout << "Selected backend: " << ep.host << ":" << ep.port << std::endl;
        } catch (const std::exception& e) {
            res.status = 503;
            res.set_content("No healthy backends available", "text/plain");
            return;
        }

        httplib::Client client(ep.host, ep.port);
        httplib::Result response;

        if (req.method == "GET")
            response = client.Get(path.c_str());
        else if (req.method == "POST")
            response = client.Post(path.c_str(), req.body, "text/plain");
        else if (req.method == "PUT")
            response = client.Put(path.c_str(), req.body, "text/plain");
        else if (req.method == "PATCH")
            response = client.Patch(path.c_str(), req.body, "text/plain");
        else if (req.method == "DELETE")
            response = client.Delete(path.c_str());
        else {
            res.status = 405;
            res.set_content("Method Not Allowed", "text/plain");
            return;
        }

        if (response) {
            res.status = response->status;
            res.set_content(response->body, "text/plain");
        } else {
            res.status = 502;
            res.set_content("Failed to connect to backend", "text/plain");
        }
    };

    server.Get(R"(/.*)", handler);
    server.Post(R"(/.*)", handler);
    server.Put(R"(/.*)", handler);
    server.Patch(R"(/.*)", handler);
    server.Delete(R"(/.*)", handler);

    std::cout << "Listener started on http://127.0.0.1:8080" << std::endl;
    server.listen("127.0.0.1", 8080);
}

Listener::Listener(const std::string &address) {
    run_listener();
}