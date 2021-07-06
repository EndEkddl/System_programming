#ifndef __ASSEMBLER_H__
#define __ASSEMBLER_H__
#define MAX 1001

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "opcodeTable.h"
#include "20181593.h"

typedef struct _symNode{
    int addr;
    char symbol[7];
    struct _symNode *next;
} symNode;

int is_empty(char *str);
int is_comment(char *str);
int is_operator(char *str);
int is_directive(char *str);
int is_reg(char *str);
int is_valid_symbol(char *str);
int is_in_symbol_table(char *str);
int is_valid_const(char *str);
int strToDec(char *str, int bound);
int charToHex(char c);
int strToHex(char *str, int bound);
int asm_tokenize(char *str, char token[MAX][MAX], int *s, int *e);
int comment_tokenize(char *str, char token[MAX][MAX]);
void strPop(char *str);
int byteCalculate(char *str);
int makeSIC_code(char *asmFile, char *mediumFile, char *str, int *line);
void byteToHex(char *str, char *target);
int makeObjFile(char *mediumFile, char *asmFile, char *objFile, char *lstFile, char *str, int *line);
void printAsmError(int error, char *str, int n);
void assemble(char *filename);
void make_sym_table();
void clear_sym_table();
void addSymbol(int addr, char symbol[7]);
int getSymHash(char key[7]);
int getAddr(char key[7]);
void symbol();

#endif
