CC=g++
CF=-std=c++11 -pedantic -W -pthread

BUILD_DIR=build

all: $(BUILD_DIR) NetworkManager $(BUILD_DIR)/System $(BUILD_DIR)/Switch

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

NetworkManager: NetworkManager.cpp
	$(CC) $(CF) -o NetworkManager NetworkManager.cpp

$(BUILD_DIR)/System: System.cpp System.hpp util.cpp
	$(CC) $(CF) -o $(BUILD_DIR)/System System.cpp util.cpp

$(BUILD_DIR)/Switch: Switch.cpp Switch.hpp util.cpp
	$(CC) $(CF) -o $(BUILD_DIR)/Switch Switch.cpp util.cpp

.PHONY: clean
clean:
	rm -rf NetworkManager $(BUILD_DIR) &> /dev/null