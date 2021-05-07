#ifndef UTIL_HPP
#define UTIL_HPP

#include <unistd.h>
#include <string>
#include <cstring>
#include <sstream>
#include <vector>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "defs.hpp"
#include "frame.hpp"
#include "util.hpp"


command reconstructCommand(char* commandBuf);
frame readFrameFromFd (int fifoFd);
void sendFrameToFd (frame newFrame, int fifoFd);

#endif