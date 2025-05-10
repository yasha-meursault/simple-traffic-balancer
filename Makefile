CXX = g++
CXXFLAGS = -std=c++17 -Wall -pthread
LDFLAGS =

SRC = main.cpp registry.cpp balancer.cpp listener.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = balancer

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f $(OBJ) $(TARGET)