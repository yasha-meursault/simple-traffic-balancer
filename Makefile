# Компилятор и флаги
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

# Главный исходный файл
SRC = main.cpp
OBJ = $(SRC:.cpp=.o)

# Цель по умолчанию
all: balancer

# Правила сборки
balancer: $(OBJ)
	$(CXX) $(CXXFLAGS) -o balancer $(OBJ)

# Очистка
clean:
	rm -f *.o balancer
