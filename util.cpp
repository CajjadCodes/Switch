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

frame readFrameFromFd (int fifoFd) {
    byte buf[3000];
    int bytesRead = read(fifoFd, buf, 25);
    frame newFrame;
    newFrame.dataType = buf[0];
    newFrame.senderID = buf[1];
    newFrame.destinationID = buf[2];
    memcpy(newFrame.totalSeq, &buf[3], 2);
    memcpy(newFrame.seq, &buf[5], 2);
    memcpy(newFrame.fileNameMD5hash, &buf[7], 16);
    memcpy(newFrame.dataSize, &buf[23], 2);

    int numberOfDataBytes = (newFrame.dataSize[0] << 8) + newFrame.dataSize[1];
    newFrame.data = (byte*) malloc(numberOfDataBytes * sizeof(byte));
    bytesRead = read(fifoFd, newFrame.data, numberOfDataBytes);

    return newFrame;
}

void sendFrameToFd (frame newFrame, int fifoFd) {
    write(fifoFd, &newFrame.dataType, 1);
    write(fifoFd, &newFrame.senderID, 1);
    write(fifoFd, &newFrame.destinationID, 1);
    write(fifoFd, &newFrame.totalSeq, 2);
    write(fifoFd, &newFrame.seq, 2);
    write(fifoFd, &newFrame.fileNameMD5hash, 16);
    write(fifoFd, &newFrame.dataSize, 2);
    int numberOfDataBytes = (newFrame.dataSize[0] << 8) + newFrame.dataSize[1];
    write(fifoFd, &newFrame.data, numberOfDataBytes);
}