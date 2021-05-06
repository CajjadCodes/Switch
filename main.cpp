#include <iostream>
#include <unistd.h>
#include <vector>
#include <string>
#include <sstream>
#include <thread>
#include "Switch.hpp"
#include "System.hpp"
#include "defs.hpp"

using namespace std;

vector<Switch> switches;
vector<System> systems;

vector<string> splitString (string str) {
    vector<string> strVec;
    auto strStream = istringstream(str);
    string strToken;
    while (strStream >> strToken) {
        strVec.push_back(strToken);
    }
    return strVec;
}

int findSwitchIndexByID(int id) {
    for (unsigned int i = 0; i < switches.size(); i++) {
        if (switches[i].getId() == id) {
            return i;
        }
    }
    return NOT_FOUND;
}

int findSystemIndexByID(int id) {
    for (unsigned int i = 0; i < systems.size(); i++) {
        if (systems[i].getId() == id) {
            return i;
        }
    }
    return NOT_FOUND;
}

void createNewSwitch(vector<string> params) {
    if (params.size() != 3) {
        cout << "Wrong Syntax\nSyntax: MySwitch <number_of_ports> <switch_number>\n";
        return;
    }
    if ((findSwitchIndexByID(stoi(params[2])) != NOT_FOUND) || (findSystemIndexByID(stoi(params[2])) != NOT_FOUND)) {
        cout << "ID is taken. Please try again." << endl;
        return;
    }
    Switch newSwitch = Switch(stoi(params[1]), stoi(params[2]));
    switches.push_back(newSwitch);
    thread newSwitchListeningThread(&Switch::listen, newSwitch);
    newSwitchListeningThread.join();
    cout << "done!" << endl;
}


int main() {
    string input;

    while (true) {
        cout << "> ";
        getline(cin, input);
        vector<string> parsedInput = splitString(input);
        if (parsedInput[0] == "MySwitch") 
            createNewSwitch(parsedInput);
        // else if (parsedInput[0] == "MySystem") 
        //     createNewSystem(parsedInput);
        // else if (parsedInput[0] == "Send") 
        //     createNewSystem(parsedInput);
        else if (parsedInput[0] == "Exit") 
            break;

    }

    return 0;
}