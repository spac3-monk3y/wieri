#ifndef WIERI_H
#define WIERI_H

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <pthread.h>
#include <wiringPi.h>
#include <signal.h>
#include <execinfo.h>
#include <unistd.h>
#include "ini.h"

typedef enum {false=0, true=!false} bool;

int sliceOnesCount(unsigned char* bits, int start, int stop);

bool parityCheck(unsigned char* bits);

int binSlice2Dec(unsigned char* bits, int start, int stop);

void got0(void);

void got1(void);


#endif
