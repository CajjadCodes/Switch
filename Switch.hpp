#ifndef SWITCH_HPP
#define SWITCH_HPP

#include <iostream>
#include <unistd.h>

class Switch {
public:
    Switch(int _totalPorts, int _id);
    void listen();
    
    int getId() {return this->id;}
private:
    int id;
    int totalPorts;
};

#endif