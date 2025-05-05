CXX = g++
CXXFLAGS = -std=c++17 -O2 -pthread

SRCS = main.cpp listener.cpp balancer.cpp registry.cpp health.cpp
OBJS = $(SRCS:.cpp=.o)

balancer: $(OBJS)
	$(CXX) $^ -o $@ $(CXXFLAGS)

servers/srv%: servers/srv%.cpp
	$(CXX) $< -o $@ $(CXXFLAGS)

.PHONY: all clean
all: balancer servers/srv1 servers/srv2 servers/srv3 servers/srv4 servers/srv5

clean:
	rm -f *.o balancer servers/srv*