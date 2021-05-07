#ifndef SWITCH_HPP
#define SWITCH_HPP

#include <iostream>
#include <unistd.h>
#include <string>
#include <cstring>
#include <sstream>
#include <vector>
#include <thread>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <mutex>
#include "defs.hpp"
#include "frame.hpp"
#include "util.hpp"
class Switch {
public:
    Switch(int _totalPorts, int _id, std::string _fifoPrefix);
    void listenOnPort(int port);
    void connectToSystem(command _command);
    void connectToSwitch(command _command);
    int getPortOfDestination(int id);
    void updateIdToPortTable(int senderID, int senderPort);
    
    int getId() {return id;}
    void closeOutputPorts();
    void exitThreads() {this->isExiting = true;}
private:
    int id;
    int totalPorts;
    std::string fifoPrefix;
    std::vector<std::string> portsInputFifo;
    std::vector<int> portsOutputFifoFd;
    std::vector<bool> isPortOpen;
    bool isExiting;
    std::vector<std::pair<int, int>> idToPortTable;
};

#endif