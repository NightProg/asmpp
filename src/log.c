//
// Created by antoine barbier on 15/09/2024.
//

#include "log.h"
#include <stdarg.h>



void log_(int level, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    switch (level) {
        case LOG_INFO:
            printf("[INFO] ");
            break;
        case LOG_WARN:
            printf("[WARN] ");
            break;
        case LOG_DEBUG:
            printf("[DEBUG] ");
            break;
        case LOG_ERROR:
            printf("[ERROR] ");
            break;
    }
    vprintf(fmt, args);
    printf("\n");
    va_end(args);
}