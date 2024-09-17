
#ifndef ASMPP_LOG_H
#define ASMPP_LOG_H
#define DEBUG(fmt, ...) log_(LOG_DEBUG, "[%s:%d] " fmt,__FILE_NAME__, __LINE__  ##__VA_ARGS__)
#define INFO(fmt, ...) log_(LOG_INFO, "[%s:%d] " fmt,__FILE_NAME__, __LINE__  ##__VA_ARGS__)
#include <stdio.h>

#define LOG_INFO 0
#define LOG_WARN 1
#define LOG_DEBUG 2
#define LOG_ERROR 3
#define LOG_FATAL 4


void log_(int level, const char* fmt, ...);

#endif //ASMPP_LOG_H
