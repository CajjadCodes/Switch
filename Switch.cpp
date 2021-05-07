#include "Switch.hpp"

using namespace std;

Switch::Switch(int _totalPorts, int _id)
    : totalPorts(_totalPorts), id(_id) {}

void Switch::listen() {
    cout << "im here" << endl;
    cout << "My id:" << this->id << ", My totalPorts: " << this->totalPorts << endl; 
}

int main (int argc, char* argv[]) {
    int totalPorts = atoi(argv[0]);
    int id = atoi(argv[1]);
    int commandPipeReadEnd = atoi(argv[2]);
    string recievePipePrefix(argv[3]);
    
    char commandBuf[200];
    int bytesRead = read(commandPipeReadEnd, commandBuf, 200);
    commandBuf[bytesRead] = '\0';

    command newCommand = reconstructCommand(commandBuf);
    std::cout << "Reconstructed command is:" << endl
            << newCommand.commandType << endl
            << newCommand.arg0 << endl
            << newCommand.arg1 << endl
            << newCommand.arg2 << endl
            << newCommand.arg3 << endl
            << newCommand.arg4 << endl;
    
    return 0;
}