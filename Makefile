CXX := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -I src
TARGET := build/bigint_demo
SRCS := src/main.cpp src/BigInt.cpp

all: $(TARGET)

$(TARGET): $(SRCS)
	mkdir -p build
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf build

.PHONY: all run clean
