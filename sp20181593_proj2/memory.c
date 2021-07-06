#include "memory.h"
#define MAX 0x100000

int memory[MAX];
int last = -1;

void dump(int start, int end){
    //dump 일때 범위 정하기
    if(start == -1){
	start = last+1;
	if(start >= MAX) start = 0;
    }
    //dump start일때 범위 정하기
    if(end == -1){
	if(start+160-1>MAX-1) end = MAX-1;
	else end = start+160-1;
    }

    //start부터 end까지 memory 출력
    int i, j;
    for(i=start/16; i <= end/16; i++){
	printf("%05X ", i*16);
    
	for(j=i*16; j<i*16+16; j++){
	    if(j>=start && j<=end){
	        printf("%02X ", memory[j]);
	    }
	    else printf("   ");
	}
	printf(" ; ");

	for(j=i*16; j<i*16+16; j++){
	    if(j>=start && j<=end && memory[j]>= 0x20 && memory[j] <= 0x7E){
	        printf("%c", (char)memory[j]);
	    }
	    else printf(".");

	}		
	printf("\n");
    }
    last = end;
    return;
}

void edit(int addr, int val){
    memory[addr] = val;
    return;
}

void fill(int start, int end, int val){

    for(int i=start; i<=end; i++) memory[i] = val;

    return;
}

void reset(){

    for(int i=0; i<MAX; i++) memory[i]=0;
    return;
}
