CC=g++
CF=-std=c++11 -pedantic -W -pthread

BUILD_DIR=build

all: $(BUILD_DIR) Network

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

Network: $(BUILD_DIR)/main.o $(BUILD_DIR)/System.o $(BUILD_DIR)/Switch.o
	$(CC) $(CF) -o Network $(BUILD_DIR)/main.o $(BUILD_DIR)/System.o $(BUILD_DIR)/Switch.o

$(BUILD_DIR)/main.o: main.cpp 
	$(CC) $(CF) -c -o $(BUILD_DIR)/main.o main.cpp

$(BUILD_DIR)/System.o: System.cpp System.hpp 
	$(CC) $(CF) -c -o $(BUILD_DIR)/System.o System.cpp

$(BUILD_DIR)/Switch.o: Switch.cpp Switch.hpp 
	$(CC) $(CF) -c -o $(BUILD_DIR)/Switch.o Switch.cpp

.PHONY: clean
clean:
	rm -rf Network $(BUILD_DIR) &> /dev/null