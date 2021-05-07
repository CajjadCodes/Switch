#ifndef DEFS_HPP
#define DEFS_HPP

#define NOT_FOUND -1

#define READ_END 0
#define WRITE_END 1

#define CONNECT         "connect"
#define CONNECT_SWITCH  "connect_switch"
#define SEND            "send"
#define EXIT_NETWORK    "exit_network"

#define MAX_PORTS   100

#include <string>
typedef struct {
    std::string commandType;
    std::string arg0;
    std::string arg1;
    std::string arg2;
    std::string arg3;
    std::string arg4;
} command;

#endif