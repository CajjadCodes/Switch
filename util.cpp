#include "util.hpp"

using namespace std;

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