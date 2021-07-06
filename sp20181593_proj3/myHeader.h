#pragma once
#ifndef __MYHEADER_H__
#define __MYHEADER_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#define MAX_LEN 1001

typedef struct _node{
	char comm[MAX_LEN];
	struct _node* next;	
}node;

void help();
void dir();
void quit();
void history();
void addHistory();
void removeHistory();
void type(char filename[MAX_LEN]);
#endif
