#include "Switch.hpp"

using namespace std;

Switch::Switch(int _totalPorts, int _id)
    : totalPorts(_totalPorts), id(_id) {}

void Switch::listen() {
    cout << "im here" << endl;
    cout << "My id:" << this->id << ", My totalPorts: " << this->totalPorts << endl; 
}

command reconstructCommand(char* commandBuf) {
    vector<string> strVec;
    auto strStream = istringstream(string(commandBuf));
    string strToken;
    while (strStream >> strToken) {
        strVec.push_back(strToken);
    }
    command newCommand = {"", "", "", "", "", ""};
    newCommand.commandType = strVec[0];
    if (strVec.size() > 1)
        newCommand.arg0 = strVec[1];
    if (strVec.size() > 2)
        newCommand.arg1 = strVec[2];
    if (strVec.size() > 3)
        newCommand.arg2 = strVec[3];
    if (strVec.size() > 4)
        newCommand.arg3 = strVec[4];
    if (strVec.size() > 5)
        newCommand.arg4 = strVec[5];
    return newCommand;
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