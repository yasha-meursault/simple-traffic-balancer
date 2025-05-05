#include "httplib.h" // скопировал с гитхаба :)
#include <iostream>

void run_listener() {
	httplib::Server server;
	
	auto handler = [](const httplib::Request& req, httplib::Response& res) {
		std::cout << req.method << " " << req.path << std::endl; // для отладки 

		auto pos = req.path.find('/', 1);
		if (pos == std::string::npos) {
			res.status = 400;
			res.set_content("Bad request", "text/plain");
			return;
		}

		std::string service = req.path.substr(1, pos - 1);
		std::string path = req.path.substr(pos);

		httplib::Client client("127.0.0.1", 8081);
		//****************************************************
		//*                                                  *
		//* ВАЖНО! ИДЕТ НА localhost:8081, ЕСЛИ ЧТО ПОМЕНЯЕМ *
		//*                                                  *
		//****************************************************

		std::shared_ptr<httplib::Response> response;

		if (req.method == "GET")
			auto response = client.Get(path.c_str());
		else if (req.method == "POST")
			auto response = client.Post(path.c_str(), req.body, "text/plain");
		else if (req.method == "PUT")
			auto response = client.Put(path.c_str(), req.body, "text/plain");
		else if (req.method == "PATCH")
			auto response = client.Patch(path.c_str(), req.body, "text/plain");
		else if (req.method == "DELETE")
			auto response = client.Delete(path.c_str());
		else {
			res.status = 502;
			res.set_content("Failed to connect to backend", "text/plain");
		}
		};

		// обработчик методов
	server.Get(R"(/.*)", handler);
	server.Post(R"(/.*)", handler);
	server.Put(R"(/.*)", handler);
	server.Patch(R"(/.*)", handler);
	server.Delete(R"(/.*)", handler);

	std::cout << "Listener started on http://127.0.0.1:8080" << std::endl;
	server.listen("127.0.0.1", 8080);
}