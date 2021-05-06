CC=g++
CF=-std=c++11 -pedantic -W -pthread

BUILD_DIR=build

all: $(BUILD_DIR) Network $(BUILD_DIR)/System $(BUILD_DIR)/Switch

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

Network: $(BUILD_DIR)/main.cpp
	$(CC) $(CF) -o Network $(BUILD_DIR)/main.cpp

$(BUILD_DIR)/System: System.cpp System.hpp 
	$(CC) $(CF) -o $(BUILD_DIR)/System System.cpp

$(BUILD_DIR)/Switch: Switch.cpp Switch.hpp 
	$(CC) $(CF) -o $(BUILD_DIR)/Switch Switch.cpp

.PHONY: clean
clean:
	rm -rf Network $(BUILD_DIR) &> /dev/null