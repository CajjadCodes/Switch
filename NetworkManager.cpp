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

void sendCommand(command _command, int DestFd) {
    char buf[200];
    strcpy(buf, _command.commandType.c_str());
    if (_command.arg0 != "") {
        strcat(buf, (char*)" ");
        strcat(buf, _command.arg0.c_str());
    }
    if (_command.arg1 != "") {
        strcat(buf, (char*)" ");
        strcat(buf, _command.arg1.c_str());
    }
    if (_command.arg2 != "") {
        strcat(buf, (char*)" ");
        strcat(buf, _command.arg2.c_str());
    }
    if (_command.arg3 != "") {
        strcat(buf, (char*)" ");
        strcat(buf, _command.arg3.c_str());
    }
    if (_command.arg4 != "") {
        strcat(buf, (char*)" ");
        strcat(buf, _command.arg4.c_str());
    }
    strcat(buf, (char*)"\0");
    write(DestFd, buf, strlen(buf));
}

void createNewSwitch(vector<string> params, vector<SwitchInfo>& switches, vector<SystemInfo>& systems) {
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
    char recievePipeName[100];
    strcpy(recievePipeName, (char*)"/tmp/recievePipeSwitch");
    strcat(recievePipeName, params[2].c_str());
    for (int i = 1; i <= switchTotalPorts; i++) {
        char portDedicatedFifo[50];
        strcpy(portDedicatedFifo, recievePipeName);
        strcat(portDedicatedFifo, "_");
        strcat(portDedicatedFifo, to_string(i).c_str());
        mkfifo(portDedicatedFifo, 0777);
    }

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

void createNewSystem(vector<string> params, vector<SwitchInfo>& switches, vector<SystemInfo>& systems) {
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
    char recievePipeName[100]; 
    strcpy(recievePipeName, (char*)"/tmp/recievePipeSystem");
    strcat(recievePipeName, params[1].c_str());
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
    if (stoi(params[3]) > switches[switchIndex].totalPorts) {
        std::cout << "Port number out of bound." << endl;
        return;
    }
    command newCommand = {CONNECT, systems[systemIndex].recievePipeName, to_string(systems[systemIndex].id),
                            switches[switchIndex].recievePipeName + "_" + to_string(switches[switchIndex].id),
                            to_string(switches[switchIndex].id),
                            params[3]};
    sendCommand(newCommand, switches[switchIndex].commandPipeWriteEnd);
    sendCommand(newCommand, systems[systemIndex].commandPipeWriteEnd);
}

void connectSwitchToSwitch(vector<string> params, vector<SwitchInfo> switches) {
    if ((params.size() != 5) || (!isNumber(params[1])) || (!isNumber(params[2])) 
            || (!isNumber(params[3])) || (!isNumber(params[4]))) {
        std::cout << "Wrong Syntax\nSyntax: ConnectSwitch <1st_switch_number> <1st_port_number> "
            << "<2nd_switch_number> <2nd_port_number>\n";
        return;
    }
    int firstSwitchIndex = findSwitchIndexByID(stoi(params[1]), switches);
    if (firstSwitchIndex == NOT_FOUND) {
        std::cout << "System with ID " << params[1] << " not found." << endl;
        return;
    }
    int secondSwitchIndex = findSwitchIndexByID(stoi(params[3]), switches);
    if (secondSwitchIndex == NOT_FOUND) {
        std::cout << "Switch with ID " << params[3] << " not found." << endl;
        return;
    }
    if ((stoi(params[2]) > switches[firstSwitchIndex].totalPorts) 
        || (stoi(params[4]) > switches[secondSwitchIndex].totalPorts))  {
        std::cout << "Port number out of bound." << endl;
        return;
    }
    command newCommand = {CONNECT_SWITCH, 
                            to_string(switches[firstSwitchIndex].id),
                            params[2],
                            to_string(switches[secondSwitchIndex].id),
                            params[4],
                            ""};
    sendCommand(newCommand, switches[firstSwitchIndex].commandPipeWriteEnd);
    sendCommand(newCommand, switches[secondSwitchIndex].commandPipeWriteEnd);
}

void sendFileToSystem(vector<string> params, vector<SwitchInfo> switches, vector<SystemInfo> systems) {
    if ((params.size() != 4) || (!isNumber(params[1])) || (!isNumber(params[2]))) {
        std::cout << "Wrong Syntax\nSyntax: Send <sender_system_number> <reciever_system_number> <file_name>\n";
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
                            params[3], "", ""};
    sendCommand(newCommand, systems[senderSystemIndex].commandPipeWriteEnd);
}

void exitFromNetwork(vector<SwitchInfo> switches, vector<SystemInfo> systems) {
    command newCommand = {EXIT_NETWORK, "", "", "", "", ""};
    for (size_t i = 0; i < switches.size(); i++)
        sendCommand(newCommand, switches[i].commandPipeWriteEnd);
    for (size_t i = 0; i < systems.size(); i++)
        sendCommand(newCommand, systems[i].commandPipeWriteEnd);
}

void showHelp() {
    std::cout << "Available Commands:\n";
    std::cout << "\tMySwitch <number_of_ports> <switch_number>\n";
    std::cout << "\tMySystem <system_number>\n";
    std::cout << "\tConnect <system_number> <switch_number> <port_number>\n";
    std::cout << "\tConnectSwitch <1st_switch_number> <1st_port_number> "
                    << "<2nd_switch_number> <2nd_port_number>\n";
    std::cout << "\tSend <sender_system_number> <reciever_system_number> <file_name>\n";
    std::cout << "\tExit\n";
    std::cout << endl;
}

int main() {
    vector<SwitchInfo> switches;
    vector<SystemInfo> systems;

    string input;

    showHelp();
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
        else if (parsedInput[0] == "ConnectSwitch") 
            connectSwitchToSwitch(parsedInput, switches);
        else if (parsedInput[0] == "Send") 
            sendFileToSystem(parsedInput, switches, systems);
        else if (parsedInput[0] == "Exit") {
            exitFromNetwork(switches, systems);
            break;
        }
        else
            std::cout << "Command Not Found." << endl;

    }

    int status;
    while (wait(&status) > 0);

    for (size_t i = 0; i < switches.size(); i++) {
        close(switches[i].commandPipeWriteEnd);
        for (int j = 1; j <= switches[i].totalPorts; j++) {
            char portDedicatedFifo[50];
            strcpy(portDedicatedFifo, switches[i].recievePipeName.c_str());
            strcat(portDedicatedFifo, "_");
            strcat(portDedicatedFifo, to_string(i).c_str());
            unlink(portDedicatedFifo);
        }
    }

    for (size_t i = 0; i < systems.size(); i++) {
        close(systems[i].commandPipeWriteEnd);
        unlink(switches[i].recievePipeName.c_str());
    }

    return 0;
}