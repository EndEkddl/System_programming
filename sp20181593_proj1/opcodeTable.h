#ifndef __OPCODE_TABLE_H__
#define __OPCODE_TABLE_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX 1001

typedef struct _hashNode{
    int opcode;
    char mnem[MAX];
    struct _hashNode* next;
}hashNode;

void initialize();
void removeTable();
void add(int, char mnem[MAX]);
int getHash(char key[MAX]);
int getOp(char key[MAX]);
void printOp(int);
void opList();

#endif
