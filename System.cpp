#include "System.hpp"

using namespace std;

System::System(int _id)
    : id(_id){}

void System::listen() {
    cout << "im here" << endl;
    cout << "My id:" << this->id << endl; 
}

vector<string> splitString (string str) {
    vector<string> strVec;
    auto strStream = istringstream(str);
    string strToken;
    while (strStream >> strToken) {
        strVec.push_back(strToken);
    }
    return strVec;
}


void listen(int systemPipe)
{
    //wait for first frame
    frame firstFrame = readFrameFromFd(systemPipe);

}


void send(int switchPipe, string receiverPipe, string fileName, int senderId, int receiverId)
{
    //create the first frame and send it
    frame firstFrame;
    firstFrame.data = SEND_FILE_NAME;
    firstFrame.senderID = (byte)senderId;
    firstFrame.destinationID = (byte) receiverId;
    firstFrame.dataSize[0] = (byte) 0;
    firstFrame.dataSize[1] = (byte) 0;
    firstFrame.data = (byte) fileName;

    sendFrameToFd(firstFrame, switchPipe);

    fstream file(fileName, ios::binary);
    byte temp;
    while (file.read(reinterpret_cast<char*> (&temp),1024*sizeof(byte)))
    {
        frame newFrame;
        newFrame.dataType = (byte) FILE_DATA;
        newFrame.senderID = (byte)senderId;
        newFrame.destinationID = (byte) receiverId;
        //some more stuff
        sendFrameToFd(newFrame, switchPipe);
    }

    cout << "System " << senderId << "Sent file " << fileName << "to " << receiverId << endl;
}


void work()
{

}


int main (int argc, char* argv[]) {

    int id = stoi(argv[0]);
    int commandPipe = stoi(argv[1]);
    
    int switchPort;
    int switchNumber;

    while (true) {
        std::cout << "> ";
        char commandBuf[200];
        int bytesRead = read(commandPipe, commandBuf, 200);
        commandBuf[bytesRead] = '\0';
        command newCommand = reconstructCommand(commandBuf);
        if(newCommand.commandType == CONNECT)
        {
            switchNumber = stoi(newCommand.arg1);
            switchPort = stoi(newCommand.arg2);
            cout << "System " << id << "connected to switch" << switchNumber << endl;
        }
        else if(newCommand.commandType == SEND)
        {
            send(switchPort, newCommand.arg0, newCommand.arg2, (id), stoi(newCommand.arg1));
        }
        else if(newCommand.commandType == EXIT_NETWORK)
        {
            exit(EXIT_SUCCESS);
        }
    }
    return 0;
}