#ifndef __20181593_H__
#define __20181593_H__

#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include "memory.h"
#include "myHeader.h"
#include "opcodeTable.h"

#define MAX 1001

int run();
int scissor(char* str);
int makeCommand(char*);
void errorPrint(int);

#endif
