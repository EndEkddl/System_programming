#ifndef __LINKING_LOADER_H__
#define __LINKING_LOADER_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "memory.h"
#include "20181593.h"
#include "opcodeTable.h"
#include "debug.h"

#define MAX 1001

typedef struct _extNode{
    int addr;
    int len;
    char section[7];
    char symbol[7];
    struct _extNode *next;
}extNode;

void makeExt();
void clearExt();
void addExt();
int getExtHash(char key[7]);
int getExtAddr(char key[7]);
void printExt();
void printLoadError(int, int, char symbol[7]);
int updateExt(int, char filename[MAX][MAX]);
int linkingLoad(int, char filename[MAX][MAX]);
void loader(int, char filename[MAX][MAX]);
void progaddr(int);
int getStartAddr();
void printReg(int);
void printRuntimeError(int);
void implement(int, int, int, int, int);
int runProcess();
void initializeProc();
void run_();

#endif
