#ifndef SYSTEM_CPP
#define SYSTEM_CPP

#include <iostream>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sstream>
#include <fstream>
#include "defs.hpp"
#include "frame.hpp"
#include "util.hpp"


class System {
public:
    System(int _id);
    void listen();

    
    int getId() {return this->id;}
private:
    int id;

};

#endif