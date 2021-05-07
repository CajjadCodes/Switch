#ifndef FRAME_HPP
#define FRAME_HPP

typedef char byte;

#define MAX_PAYLOAD_SIZE 1024 //1024 byte, 1 KB

#define SEND_FILE_NAME  0
#define FILE_DATA       1
#define SPANNING_TREE   2


typedef struct {
    byte dataType;      // FILE_NAME or FILE_DATA or SPANNING_TREE
    byte senderID;
    byte destinationID;
    byte totalSeq[2];
    byte seq[2];
    byte fileNameMD5hash[16];
    byte dataSize[2];   // up to MAX_PAYLOAD_SIZE
    byte* data;
} frame;

#endif