#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <vector>
#include <string>
#include <cstring>
#include <sstream>
#include <thread>
#include "defs.hpp"
#include "Switch.hpp"
#include "System.hpp"

using namespace std;
typedef struct {
    int totalPorts;
    int id;
    int commandPipeWriteEnd;
    string recievePipeName;
} SwitchInfo;

typedef struct {
    int id;
    int commandPipeWriteEnd;
    string recievePipeName;
} SystemInfo;

vector<string> splitString (string str) {
    vector<string> strVec;
    auto strStream = istringstream(str);
    string strToken;
    while (strStream >> strToken) {
        strVec.push_back(strToken);
    }
    return strVec;
}

bool isNumber(string str) {
    for (unsigned int i = 0; i < str.length(); i++) {
        if (isdigit(str[i]) == 0)
            return false;
    }
    return true;
}

int findSwitchIndexByID(int id, vector<SwitchInfo> switches) {
    for (unsigned int i = 0; i < switches.size(); i++) {
        if (switches[i].id == id) {
            return i;
        }
    }
    return NOT_FOUND;
}

int findSystemIndexByID(int id, vector<SystemInfo> systems) {
    for (unsigned int i = 0; i < systems.size(); i++) {
        if (systems[i].id == id) {
            return i;
        }
    }
    return NOT_FOUND;
}

void createNewSwitch(vector<string> params, vector<SwitchInfo> switches, vector<SystemInfo> systems) {
    if ((params.size() != 3) || (!isNumber(params[1])) || (!isNumber(params[2]))) {
        std::cout << "Wrong Syntax\nSyntax: MySwitch <number_of_ports> <switch_number>\n";
        return;
    }
    if ((findSwitchIndexByID(stoi(params[2]), switches) != NOT_FOUND) 
        || (findSystemIndexByID(stoi(params[2]), systems) != NOT_FOUND)) {
        std::cout << "ID is taken. Please try again." << std::endl;
        return;
    }
    int switchID = stoi(params[2]), switchTotalPorts = stoi(params[1]);
    int unnamedCommandPipe[2]; 
    pipe(unnamedCommandPipe);
    char* recievePipeName = (char*)"/tmp/recievePipeSwitch";
    strcat(recievePipeName, params[2].c_str());
    mkfifo(recievePipeName, 0777);

    SwitchInfo newSwitch = {switchTotalPorts, switchID,unnamedCommandPipe[WRITE_END], recievePipeName};
    switches.push_back(newSwitch);
    pid_t pid = fork();
    if (pid == 0) { // child
        close(unnamedCommandPipe[WRITE_END]);
        char* childArgs[] = {(char*)to_string(switchTotalPorts).c_str(),
                                (char*)to_string(switchID).c_str(),
                                (char*)to_string(unnamedCommandPipe[READ_END]).c_str(),
                                recievePipeName, (char*)0};
        execv("build/Switch", childArgs);
    }
    else { //parent
        close(unnamedCommandPipe[READ_END]);
    }
}

void createNewSystem(vector<string> params, vector<SwitchInfo> switches, vector<SystemInfo> systems) {
    if ((params.size() != 2) || (!isNumber(params[1]))) {
        std::cout << "Wrong Syntax\nSyntax: MySystem <system_number>\n";
        return;
    }
    if ((findSwitchIndexByID(stoi(params[1]), switches) != NOT_FOUND) 
        || (findSystemIndexByID(stoi(params[1]), systems) != NOT_FOUND)) {
        std::cout << "ID is taken. Please try again." << std::endl;
        return;
    }
    int systemID = stoi(params[1]);
    int unnamedCommandPipe[2]; 
    pipe(unnamedCommandPipe);
    char* recievePipeName = (char*)"/tmp/recievePipeSystem";
    strcat(recievePipeName, params[2].c_str());
    mkfifo(recievePipeName, 0777);

    SystemInfo newSystem = {systemID, unnamedCommandPipe[WRITE_END], recievePipeName};
    systems.push_back(newSystem);
    pid_t pid = fork();
    if (pid == 0) { // child
        close(unnamedCommandPipe[WRITE_END]);
        char* childArgs[] = {(char*)to_string(systemID).c_str(),
                                (char*)to_string(unnamedCommandPipe[READ_END]).c_str(),
                                recievePipeName, (char*)0};
        execv("build/System", childArgs);
    }
    else { //parent
        close(unnamedCommandPipe[READ_END]);
    }
}

void connectSystemToSwitch(vector<string> params, vector<SwitchInfo> switches, vector<SystemInfo> systems) {
    if ((params.size() != 4) || (!isNumber(params[1])) || (!isNumber(params[2])) || (!isNumber(params[3]))) {
        std::cout << "Wrong Syntax\nSyntax: Connect <system_number> <switch_number> <port_number>\n";
        return;
    }
    int systemIndex = findSystemIndexByID(stoi(params[1]), systems);
    if (systemIndex == NOT_FOUND) {
        std::cout << "System with ID " << params[1] << " not found." << endl;
        return;
    }
    int switchIndex = findSwitchIndexByID(stoi(params[2]), switches);
    if (switchIndex == NOT_FOUND) {
        std::cout << "Switch with ID " << params[2] << " not found." << endl;
        return;
    }
    command newCommand = {CONNECT, systems[systemIndex].recievePipeName, to_string(systems[systemIndex].id),
                            switches[switchIndex].recievePipeName, to_string(switches[switchIndex].id),
                            params[3]};
    write(systems[systemIndex].commandPipeWriteEnd, &newCommand, sizeof(newCommand));
    write(switches[switchIndex].commandPipeWriteEnd, &newCommand, sizeof(newCommand));
}

void sendFileToSystem(vector<string> params, vector<SwitchInfo> switches, vector<SystemInfo> systems) {
    if ((params.size() != 4) || (!isNumber(params[1])) || (!isNumber(params[2]))) {
        std::cout << "Wrong Syntax\nSyntax: Connect <sender_system_number> <reciever_system_number> <file_name>\n";
        return;
    }
    int senderSystemIndex = findSystemIndexByID(stoi(params[1]), systems);
    if (senderSystemIndex == NOT_FOUND) {
        std::cout << "System with ID " << params[1] << " not found." << endl;
        return;
    }
    int recieverSystemIndex = findSwitchIndexByID(stoi(params[2]), switches);
    if (recieverSystemIndex == NOT_FOUND) {
        std::cout << "System with ID " << params[2] << " not found." << endl;
        return;
    }
    if (senderSystemIndex == recieverSystemIndex) {
        std::cout << "Can't send file to itself" << endl;
        return;
    }
    command newCommand = {SEND, systems[recieverSystemIndex].recievePipeName, to_string(systems[recieverSystemIndex].id),
                            params[3], "\0", "\0"};
    write(systems[senderSystemIndex].commandPipeWriteEnd, &newCommand, sizeof(newCommand));
}

void exitFromNetwork(vector<SwitchInfo> switches, vector<SystemInfo> systems) {
    command newCommand = {EXIT_NETWORK, "\0", "\0", "\0", "\0", "\0"};
    for (size_t i = 0; i < switches.size(); i++)
        write(switches[i].commandPipeWriteEnd, &newCommand, sizeof(newCommand));
    for (size_t i = 0; i < systems.size(); i++)
        write(systems[i].commandPipeWriteEnd, &newCommand, sizeof(newCommand));
}


int main() {
    vector<SwitchInfo> switches;
    vector<SystemInfo> systems;

    string input;

    while (true) {
        std::cout << "> ";
        getline(cin, input);
        vector<string> parsedInput = splitString(input);
        if (parsedInput[0] == "MySwitch") 
            createNewSwitch(parsedInput, switches, systems);
        else if (parsedInput[0] == "MySystem") 
            createNewSystem(parsedInput, switches, systems);
        else if (parsedInput[0] == "Connect") 
            connectSystemToSwitch(parsedInput, switches, systems);
        else if (parsedInput[0] == "Send") 
            sendFileToSystem(parsedInput, switches, systems);
        else if (parsedInput[0] == "Exit") 
            exitFromNetwork(switches, systems);
        else
            std::cout << "Command Not Found." << endl;

    }

    int status;
    while (wait(&status) > 0);

    for (size_t i = 0; i < switches.size(); i++) {
        close(switches[i].commandPipeWriteEnd);
        unlink(switches[i].recievePipeName.c_str());
    }
    for (size_t i = 0; i < systems.size(); i++) {
        close(systems[i].commandPipeWriteEnd);
        unlink(switches[i].recievePipeName.c_str());
    }

    return 0;
}