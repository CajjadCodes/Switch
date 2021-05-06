#ifndef SYSTEM_CPP
#define SYSTEM_CPP

#include <iostream>
class System {
public:
    System(int _id);
    void listen();
    
    int getId() {return this->id;}
private:
    int id;
};

#endif