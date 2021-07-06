#include "opcodeTable.h"
#define MAX 1001
#define SIZE 20

hashNode* hashTable[SIZE];
hashNode opTable[0xFF];

void initialize(){
    //opcode.txt 읽어서 hashTable에 저장
    FILE* fp = fopen("./opcode.txt", "r");
    int opcode,i,format,curr;
    char mnem[MAX];

    for(i=0; i<SIZE; i++) hashTable[i] = NULL;
    
    while(fscanf(fp,"%x %s %d/%d",&opcode, mnem, &format, &curr) != EOF){
	add(opcode, mnem, format);
	strcpy(opTable[opcode].mnem, mnem);
	opTable[opcode].opcode = opcode;
	opTable[opcode].format = format;
    }
    fclose(fp);

    return;
}

void removeTable(){
    //생성한 hashTable 모두 삭제
    int i;
    for(int i=0; i<SIZE; i++){
	hashNode* ptr = hashTable[i];
	while(ptr != NULL){
	    hashNode* tmp = ptr->next;
	    free(ptr);
	    ptr = tmp;
	}
	hashTable[i]=NULL;
    }

    return;
}

void add(int opcode, char mnem[MAX], int format){
    //mnemonic에 대한 hashcode 받기
    int idx = getHash(mnem);
    
    //해당하는 해쉬에 linkedlist로 opcode 연결
    hashNode* ptr = (hashNode*)malloc(sizeof(hashNode));
    
    ptr->opcode = opcode;
    ptr->format = format;
    strcpy(ptr->mnem, mnem);
    ptr->next = hashTable[idx];
    hashTable[idx] = ptr;

    return;
}

//사용자 정의 해쉬 함수
int getHash(char key[MAX]){
    int x=0;
    int len = strlen(key);
    for(int i=0; i<len; i++){
	x = ((key[i] * 17) + x) % SIZE;
    }
    if(x<0) x = -x;

    return x % SIZE;
}

//해쉬코드를 이용하여 opcode 찾기
int getOp(char key[MAX]){

    int idx = getHash(key);
    hashNode* ptr = hashTable[idx];

    while(ptr != NULL){
	if(!strcmp(key, ptr->mnem)) return ptr->opcode;
	ptr = ptr->next;
    }

    return -1;
}

int getFormat(char key[MAX]){
    int idx = getHash(key);
    for(hashNode* ptr = hashTable[idx]; ptr != NULL; ptr = ptr->next){
	if(!strcmp(key, ptr->mnem)) return ptr->format;
    }
    return -1;

}
void printOp(int opcode){

    printf("opcode is %X\n", opcode);
    return;
}
//hashTable 출력
void opList(){

    for(int i=0; i<SIZE; i++){
	hashNode* ptr = hashTable[i]; 
	printf("%2d : ",i);
	while(ptr != NULL){
	    printf("[%s,%X]", ptr->mnem, ptr->opcode);
	    if(ptr->next != NULL) printf(" -> ");
	    ptr = ptr->next;
	} 
	printf("\n");
    }
    return;
}

int getFormatByOp(int opcode){
    return opTable[opcode].format;
}
