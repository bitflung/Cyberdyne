#ifndef ADUP_MSG_T_H
#define ADUP_MSG_T_H

#include "c_adup_lib.h"

typedef struct {
    char* buf;
    uint16_t bsize;
    char cmd;
    uint16_t len;
} msg_t;

#endif
