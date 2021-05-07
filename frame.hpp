#ifndef FRAME_HPP
#define FRAME_HPP

typedef char byte;

#define MAX_PACKET_SIZE 1024 //1024 byte, 1 KB

#define SEND_FILE_NAME  "send_file_name"
#define FILE_DATA       "file_data"
#define SPANNING_TREE   "spanning_tree"


typedef struct {
    byte dataType;      // FILE_NAME or FILE_DATA or SPANNING_TREE
    byte senderID;
    byte recieverID;
    byte totalSeq[2];
    byte seq[2];
    byte dataSize[2];   // up to MAX_PACKET_SIZE
    byte* data;
} frame;

#endif