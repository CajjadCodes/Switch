#include "Switch.hpp"

using namespace std;
 
mutex idToPortTableMutex;
mutex portOutputMutex[MAX_PORTS];

Switch::Switch(int _totalPorts, int _id, string _fifoPrefix)
    : totalPorts(_totalPorts), id(_id), fifoPrefix(_fifoPrefix) {
        for (int i = 1; i <= _totalPorts; i++) {
            portsInputFifo.push_back(fifoPrefix + "_" + to_string(i));
            portsOutputFifoFd.push_back(-1);
            isPortOpen.push_back(false);
        }
        isExiting = false;
    }

int Switch::getPortOfDestination(int destinationID) {
    for (size_t i = 0; i < idToPortTable.size(); i++) {
        if (destinationID == idToPortTable[i].first)
            return idToPortTable[i].second;
    }
    return NOT_FOUND;
}

void Switch::updateIdToPortTable(int senderID, int senderPort) {
    for (size_t i = 0; i < idToPortTable.size(); i++) {
        if (senderID == idToPortTable[i].first)
            return;
    }
    idToPortTable.push_back(pair<int,int>(senderID, senderPort));
}

void Switch::listenOnPort(int port) {
    int portListenFifoFd = open(portsInputFifo[port-1].c_str(), O_RDONLY);
    byte buf[3000];
    while (true) {
        if (isExiting)
            break;
        frame newFrame = readFrameFromFd(portListenFifoFd);
        
        idToPortTableMutex.lock();
        int targetPort = getPortOfDestination((int)newFrame.destinationID);
        updateIdToPortTable((int)newFrame.senderID, port);
        idToPortTableMutex.unlock();

        if (targetPort == NOT_FOUND) {
            for (int i = 1; i <= this->totalPorts; i++) {
                if (isPortOpen[i-1] && (i != port)) {
                    portOutputMutex[i-1].lock();
                    sendFrameToFd(newFrame, portsOutputFifoFd[i-1]);
                    portOutputMutex[i-1].unlock();
                }
            }
        }
        else {
            portOutputMutex[targetPort-1].lock();
            sendFrameToFd(newFrame, portsOutputFifoFd[targetPort-1]);
            portOutputMutex[targetPort-1].unlock();
        }

        free(newFrame.data);
    }

    close(portListenFifoFd);
}

void Switch::connectToSystem(command _command) {
    int portNumber = stoi(_command.arg4);
    portsOutputFifoFd[portNumber-1] = open(_command.arg0.c_str(), O_WRONLY);;
    isPortOpen[portNumber-1] = true;
}

void Switch::connectToSwitch(command _command) {
    int portNumber;
    string outputPort;
    if (stoi(_command.arg0) == getId()) {
        portNumber = stoi(_command.arg1);
        outputPort = "/tmp/recievePipeSwitch_" + stoi(_command.arg3);
    }
    else {
        portNumber = stoi(_command.arg3);
        outputPort = "/tmp/recievePipeSwitch_" + stoi(_command.arg1);
    }
    portsOutputFifoFd[portNumber-1] = open(outputPort.c_str(), O_WRONLY);;
    isPortOpen[portNumber-1] = true;
}

void Switch::closeOutputPorts() {
        for (size_t i = 0; i < portsOutputFifoFd.size(); i++)
            close(portsOutputFifoFd[i]);
}

int main (int argc, char* argv[]) {
    int totalPorts = atoi(argv[0]);
    int id = atoi(argv[1]);
    int commandPipeReadEnd = atoi(argv[2]);
    string recievePipePrefix(argv[3]);

    Switch thisSwitch(totalPorts, id, recievePipePrefix);

    char commandBuf[200];
    vector<thread> portListenThreads;

    while (true) {
        int bytesRead = read(commandPipeReadEnd, commandBuf, 200);
        commandBuf[bytesRead] = '\0';
        command newCommand = reconstructCommand(commandBuf);
        if (newCommand.commandType == CONNECT) {
            thisSwitch.connectToSystem(newCommand);
            int portNumber = stoi(newCommand.arg4);
            portListenThreads.push_back(thread(&Switch::listenOnPort, thisSwitch, portNumber));
        }
        else if (newCommand.commandType == CONNECT_SWITCH) {
            thisSwitch.connectToSwitch(newCommand);
            int portNumber;
            if (stoi(newCommand.arg0) == thisSwitch.getId())
                portNumber = stoi(newCommand.arg1);
            else
                portNumber = stoi(newCommand.arg3);
            portListenThreads.push_back(thread(&Switch::listenOnPort, thisSwitch, portNumber));
        }
        else if (newCommand.commandType == EXIT_NETWORK) {
            thisSwitch.exitThreads();
            break;
        }
    }

    for (size_t i = 0; i < portListenThreads.size(); i++) {
        if (portListenThreads[i].joinable())
            portListenThreads[i].joinable();
    }

    thisSwitch.closeOutputPorts();
    
    exit(0);
}