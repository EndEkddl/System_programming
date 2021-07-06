#include "debug.h"
#define MAX 1001

int breakpoint[MAX];
int bpCnt=0;

//Check if there is a bp corresponding to addr
int getBp(int addr){
    for(int i=0; i<bpCnt; i++){
	if(breakpoint[i] == addr) return 1;
    }
    return 0;
}

void printBp(){
    printf("\tbreakpoint\n");
    printf("\t----------\n");

    if(!bpCnt) {
	printf("\tno breakpoint\n"); 
	return;
    }

    for(int i=0; i<bpCnt; i++){
	printf("\t%X\n", breakpoint[i]);
    }
    return;
}

void swap(int a, int b){
    int tmp;
    tmp = a;
    a = b;
    b = a;
    return;
}

void setBp(int addr){

    //check for duplicate bp
    if(getBp(addr)){
	printf("\tduplicate breakpoint\n");
	return;
    }

    breakpoint[bpCnt++] = addr;

    //sorting bp array
    for(int i= bpCnt-1; i>=0; i--){
	for(int j=0; j<i; j++){
	    if(breakpoint[j] > breakpoint[j+1]){
		swap(breakpoint[j], breakpoint[j+1]);
	    }
	}

    }
    printf("\t[ok] create breakpoint %x\n",addr);

    return;
}

void clearBp(){
    for(int i=0; i<bpCnt; i++){
	breakpoint[i] = 0;
    }
    bpCnt = 0;
    printf("\t[ok] clear all breakpoints\n");
    return;
}

void bp(int addr){
    if(addr == -1) printBp();//user input : bp
    else if(addr == -2) clearBp();//user input : bp clear
    else setBp(addr);//user input : bp [address]
}
