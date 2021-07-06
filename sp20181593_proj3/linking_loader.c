#include "linking_loader.h"
#define MAX 1001
#define MAXADDR 0x100000
#define IMMEDIATE 1
#define INDIRECT 2
#define SIMPLE 3
#define LT 1
#define EQ 2
#define GT 3

enum regis{
    A,X,L,B,S,T,F,TMP,PC,SW
};

int startAddr = 0;
extNode *extTable[50];
extNode extArr[1001];
int reg[10] = {0};
int debugF = 0;
int errLine=0;//output error line
char errSym[7]= " ";//output error symbol

void makeExt(){
    for(int i=0; i<50; i++) extTable[i] = NULL;
    return ;
}

void clearExt(){
    for(int i=0; i<50; i++){
	extNode *ptr = extTable[i];
	while(ptr != NULL){
	    extNode *tmp = ptr->next;
	    free(ptr);
	    ptr = tmp;
	}
	extTable[i] = NULL;
    }
}

void addExt(int addr, int len, char section[7], char symbol[7]){

    int idx = getExtHash(symbol);
    
    extNode *node = (extNode *)malloc(sizeof(extNode));
    node->addr = addr;
    node->len = len;
    strcpy(node->section, section);
    strcpy(node->symbol, symbol);
    node->next = extTable[idx];
    extTable[idx] = node;

    return;
}

//Hash function for external symbol
int getExtHash(char key[7]){
    int num = 0;

    for(int i=0; i<(int)strlen(key); i++){
	num += (int)key[i];
	return num % 50;
    }
}

int getExtAddr(char key[7]){
    int idx = getExtHash(key);

    for(extNode *ptr = extTable[idx]; ptr != NULL; ptr = ptr->next){
	if(!strcmp(ptr->symbol, key)) return ptr->addr;
    }
    return -1;
}

void printExt(){
    int idx=0, len=0;
    //move the symbol stored on the extTable to extArr
    for(int i=0; i<50; i++){
	for(extNode *ptr = extTable[i]; ptr != NULL; ptr = ptr->next){
	    strcpy(extArr[idx].section, ptr->section);
	    strcpy(extArr[idx].symbol, ptr->symbol);
	    extArr[idx].addr = ptr->addr;
	    extArr[idx++].len = ptr->len;
	}
    }
    
    //print it according to format
    printf("\tcontrol\tsymbol\taddress\tlength\n");
    printf("\tsection\tname\n");
    printf("\t-------------------------------\n");
    for(int i=0; i<idx; i++){
	//control section    
	if(!strcmp(extArr[i].section,extArr[i].symbol)){
	    len += extArr[i].len;
	    printf("\t%s%s\t%04X\t%04X\n", extArr[i].section, "\t", extArr[i].addr, extArr[i].len);
	}

	//symbol name
	else{
	    printf("\t%s%s\t%04X\t%04X\n","\t", extArr[i].symbol, extArr[i].addr, extArr[i].len);
	    
	}
    }
    printf("\t-------------------------------\n");
    printf("\t\ttotal length\t%04X\n", len);
}

void printLoadError(int error, int line, char symbol[7]){

    switch(error){
	case 1 : printf("\tError : Invalid object file.\n"); break;
	case 2 : {
		    printf("\tError : Duplicate external symbol(%s) at line %d.\n",symbol, line); 
		    break;
		 }
	case 3 : {
		    printf("\tError : Undefined external symbol(%s) at line %d.\n",symbol, line); 
		    break;
		 }	    
	default : break;
    }
    return;
}

int updateExt(int tsz, char filename[MAX][MAX]){

    char str[MAX];
    int cs = startAddr;

    for(int fileSize=1; fileSize<tsz; fileSize++){
	char pName[7], tag[7];
	int start, len, line;
	FILE *fp = fopen(filename[fileSize], "r");

	fgets(str, MAX, fp);
	line = sscanf(str, "%1s%6s%6X%6X", tag, pName, &start, &len);

	if(start) return 1;
	if(line == EOF) return 1;
	if(getExtAddr(pName) != -1) {//duplicate external symbol
	    errLine = 1;
	    strcpy(errSym, pName);   
	    return 2;
	}
	
	addExt(start+cs, len, pName, pName);
	int where = 0;//store where the error line is
	while(fgets(str, MAX, fp) != NULL){
	    where++;
	    if(str[0] == 'H') return 1;

	    //Define record
	    else if(str[0] == 'D'){
		char symbol[6][7] = {0};//name of external symbol
		int addr[6] = {-1,-1,-1,-1,-1,-1};//relative address of symbol
		line = sscanf(str, "%1s%6s%6X%6s%6X%6s%6X%6s%6X%6s%6X%6s%6X",
				tag,symbol[0],&addr[0],symbol[1],&addr[1],symbol[2],
				&addr[2],symbol[3],&addr[3], symbol[4],&addr[4],
				symbol[5],&addr[5]);

		if(tag[0] == '.') continue;//comment
		if(line == EOF) return 1;

		for(int i=0; i<6; i++){
		    if(symbol[i][0] != 0 && addr[i] != -1){
			//duplicate external symbol
			if(getExtAddr(symbol[i]) != -1) {
			    errLine = where;
			    strcpy(errSym, symbol[i]);	    
			    return 2;
			}

			addExt(addr[i]+cs,0,pName,symbol[i]);
		    }
		}

	    }
	    else if(str[0] == 'E') break;	    
	}
	cs += len;
	fclose(fp);
    }
    return 0;
}

int linkingLoad(int tsz, char filename[MAX][MAX]){
    char str[MAX];

    for(int i=0; i<tsz-1; i++){
	char pName[7], tag[7];
	int start, len, line;
	FILE *fp = fopen(filename[i+1], "r");
	int ref[100], refSize=0;

	fgets(str, MAX, fp);
	line = sscanf(str, "%1s%6s%6X%6X", tag, pName, &start, &len);
	if(line == EOF) return 1;

	ref[1] = getExtAddr(pName);

	//undefine external symbol
	if(ref[1] == -1) {
	    errLine = 1;
	    strcpy(errSym, pName);	    
	    return 3;
	}
	else refSize++;

	int where=0;//store where the error line is
	while(fgets(str, MAX, fp) != NULL){
	    where++;
	    if(str[0] == 'R'){
		int refer[9] = {-1,-1,-1,-1,-1,-1,-1,-1,-1};
		char refSym[9][7] = {0,};//name of external symbol reffered to this section

		line = sscanf(str, "%1s%2d%6s%2d%6s%2d%6s%2d%6s%2d%6s%2d%6s%2d%6s%2d%6s%2d%6s", 
				tag,&refer[0],refSym[0],&refer[1],refSym[1],&refer[2],refSym[2],
				&refer[3],refSym[3],&refer[4],refSym[4],&refer[5],refSym[5],
				&refer[6],refSym[6],&refer[7],refSym[7],&refer[8],refSym[8]);
		for(int i=0; i<9; i++){
		    if(refSym[i][0] != 0 && refer[i] != -1){
			int tmp = getExtAddr(refSym[i]);

			//undefine external symbol
			if(tmp == -1) {
			    errLine = where;
			    strcpy(errSym, refSym[i]);	    
			    return 3;
			}
			ref[refer[i]] = tmp;
			refSize++;
		    }
		}
	    }

	    else if(str[0] == 'T'){
		int textAddr, textLen;
		char text[100];

		line = sscanf(str, "%1s%6X%2X%s", tag, &textAddr, &textLen, text);

		textAddr += ref[1];
		textLen = textLen * 2;
		if(textLen != (int)strlen(text)) return 1;
		
		//convert character to hex
		for(int i=0; i<textLen; i+=2){
		    int num = charToHex(text[i]);
		    num = num<<4;
		    num += charToHex(text[i+1]);

		    if(textAddr >= MAXADDR) return 1;
		    edit(textAddr, num&0x000000ff);
		    textAddr++;
		}
	    }
	    else if(str[0] == 'M'){
		int modAddr, modLen, refer, num=0;
		char plus;

		line = sscanf(str, "%1s%6X%2X%c%2d",tag,&modAddr,&modLen,&plus,&refer);
		if(refer > refSize) return 3;

		modAddr += ref[1];
		modLen = (modLen+1)/2;//change to byte 
		for(int i=0; i<modLen; i++){
		    num += getValue(modAddr+i);
		    num = num << 8;
		}
		
		if(plus == '+') num += ref[refer];
		else if(plus == '-') num -= ref[refer];

		for(int i=modLen-1; i>=0; i--){
		    if(modAddr + i >= MAXADDR)return 1;
		    edit(modAddr+i, num & 0x000000ff);
		    num = num >> 8;
		}
	    }
	    else if(str[0] == 'E') break;
	}
	fclose(fp);
    }
    return 0;
}

void progaddr(int addr){
    startAddr = addr;
    return ;
}

int getStartAddr(){
    return startAddr;
}

void loader(int tsz, char filename[MAX][MAX]){
    initializeProc();
    makeExt();
    int error1 = updateExt(tsz, filename);
    int error2 = linkingLoad(tsz, filename);
    if(error1){
	clearExt();
	printLoadError(error1,errLine, errSym);
	return;
    }
    if(error2){
	clearExt();
	printLoadError(error2,errLine, errSym);
	return;
    }
    printExt();
    clearExt();
    return ;
}

void printReg(int addr){

    printf("\tA : %06X   X : %06X\n",reg[A], reg[X]);
    printf("\tL : %06X  PC : %06X\n",reg[L], reg[PC]);
    printf("\tB : %06X   S : %06X\n",reg[B], reg[S]);
    printf("\tT : %06X\t\n",reg[T]);

    if(addr != -1) printf("\t\tStop at checkpoint[%X]\n",addr);
    else printf("\t\tEnd program\n");

    return ;
}

void printRuntimeError(int error){
    if(error == 1){
	printf("Runtime Error : Wrong Memory Access.\n");
    }
    else if(error == 2){
	printf("Runtime Error : Wrong Execution.\n");
    }
    return ;
}

void implement(int opcode, int r1, int r2, int mode, int target){

    int num=0, rd=0;
    int tmp;

    if(mode == IMMEDIATE) num = target;//immediate addressing mode
    else if(mode == SIMPLE){//simple addressing mode
	num += getValue(target);
	num = num << 8;
	num += getValue(target+1);
	num = num << 8;
	num += getValue(target+2);
    }   
    else{//indirect addressing mode
	int tmp2;
	tmp2 = getValue(target);
	tmp2 = tmp2 << 8;
	tmp2 += getValue(target+1);
	tmp2 = tmp2 << 8;
	tmp2 += getValue(target+2);
	target = tmp2;
	num += getValue(target);
	num = num << 8;
	num += getValue(target+1);
	num = num << 8;
	num += getValue(target+2);
    }

    if(num >= (int)(1<<23)) num |= 0xff000000;
   

    switch(opcode){
	    case 0x18: reg[A] += num;	break;//ADD
	    case 0x90: reg[r2] += reg[r1]; break;//ADDR
	    case 0x40: reg[A] &= num; break;//AND
	    case 0xB4: reg[r1] = 0; break;//CLEAR
	    case 0x28: {
			    if(reg[A]>num) reg[SW] = GT;
			    else if(reg[A] == num) reg[SW] = EQ;
			    else reg[SW] = LT; 
			    break;
		       }//COMP
	    case 0xA0:{
			if(rd==1) {
			    reg[SW]=EQ; 
			    rd = 0; 
			    break;
			}
			if(reg[r1]>reg[r2]) reg[SW] = GT;
			else if(reg[r1]==reg[r2]) reg[SW] = EQ;
			else reg[SW] = LT;
			break;
		      }//COMPR
	    case 0x24: if(num) reg[A] /= num; break;//DIV
	    case 0x9C: if(reg[r1]) reg[r2] /= reg[r1]; break;//DIVR
	    case 0x3C: reg[PC] = target; break;//J
	    case 0x30: if(reg[SW]==EQ) reg[PC] = target; break;//JEQ
	    case 0x34: if(reg[SW]==GT) reg[PC] = target; break;//JGT
	    case 0x38: if(reg[SW]==LT) reg[PC] = target; break;//JLT
	    case 0x48: reg[L] = reg[PC]; reg[PC] = target; break;//JSUB
	    case 0x00: reg[A] = num; break;//LDA
	    case 0x68: reg[B] = num; break;//LDB
	    case 0x50: reg[A] = getValue(target); break;//LDCH
	    case 0x08: reg[L] = num; break;//LDL
	    case 0x6C: reg[S] = num; break;//LDS
	    case 0x74: reg[T] = num; break;//LDT
	    case 0x04: reg[X] = num; break;//LDX
	    case 0x20: reg[A] *= num; break;//MUL
	    case 0x98: reg[r2] *= reg[r1]; break;//MULR
	    case 0x44: reg[A] |= num; break;//OR
	    case 0xAC: reg[r2] = reg[r1]; break;//RMO
	    case 0x4C: reg[PC] = reg[L]; break;//RSUB
	    case 0x0C: {
			    tmp = reg[A];
			    edit(target+2,tmp%256);
			    tmp = tmp >> 8;
			    edit(target+1, tmp%256);
			    tmp = tmp >> 8;
			    edit(target, tmp%256);
			    break;
		       }//STA
	    case 0x78: {
			    tmp = reg[B];
			    edit(target+2,tmp%256);
			    tmp = tmp >> 8;
			    edit(target+1, tmp%256);
			    tmp = tmp >> 8;
			    edit(target, tmp%256);
			    break;
		       }//STB
	    case 0x14: {
			    tmp = reg[L];
			    edit(target+2,tmp%256);
			    tmp = tmp >> 8;
			    edit(target+1, tmp%256);
			    tmp = tmp >> 8;
			    edit(target, tmp%256);
			    break;
		       }//STL
	    case 0x7C: {
			    tmp = reg[S];
			    edit(target+2,tmp%256);
			    tmp = tmp >> 8;
			    edit(target+1, tmp%256);
			    tmp = tmp >> 8;
			    edit(target, tmp%256);
			    break;
		       }//STS
	    case 0xE8: {
			    tmp = reg[SW];
			    edit(target+2,tmp%256);
			    tmp = tmp >> 8;
			    edit(target+1, tmp%256);
			    tmp = tmp >> 8;
			    edit(target, tmp%256);
			    break;
		       }//STSW
	    case 0x84: {
			    tmp = reg[T];
			    edit(target+2,tmp%256);
			    tmp = tmp >> 8;
			    edit(target+1, tmp%256);
			    tmp = tmp >> 8;
			    edit(target, tmp%256);
			    break;
		       }//STT
	    case 0x10: {
			    tmp = reg[X];
			    edit(target+2,tmp%256);
			    tmp = tmp >> 8;
			    edit(target+1, tmp%256);
			    tmp = tmp >> 8;
			    edit(target, tmp%256);
			    break;
		       }//STX
	    case 0x54: {
			    tmp = reg[A];
			    edit(target, tmp%256);
			    break;
		       }//STCH
	    case 0x1C: reg[A] -= num; break;//SUB
	    case 0x94: reg[r2] -= reg[r1]; break;//SUBR
	    case 0xE0: reg[SW] = LT; break;//TD
	    case 0x2C: {
			    reg[X]++;
			    if(reg[X]>num) reg[SW] = GT;
			    else if(reg[X]==num) reg[SW] = EQ;
			    else reg[SW] = LT;
			    break;
		       }//TIX
	    case 0xB8: {
			    reg[X]++;
			    if(reg[X]>reg[r1]) reg[SW] = GT;
			    else if(reg[X]==reg[r1]) reg[SW] = EQ;
			    else reg[SW] = LT;
			    break;
		       }//TIXR
	    case 0xD8: {
			    rd = 1;
			    break;
		       }//RD
	    default : break;		      
    }

    return;
}

int runProcess(){

    if(!debugF) reg[PC] = getStartAddr();

    for(;reg[PC]<MAXADDR;){
	int n=0,i=0,x=0,b=0,p=0,e=0;
	int opcode = getValue(reg[PC]++);
	int r1=-1,r2=-1,target=-1;
	int mode = opcode % 4;
	int format;
	
	debugF = 1;
	opcode -= mode;
	format = getFormatByOp(opcode);

	if(format == 1 && mode != 0) return 2;
	else if(format == 2){
	    int regs = getValue(reg[PC]++);

	    if((opcode % 4)) return 2;
	    r2 = regs % 16;
	    r1 = regs / 16;
	    if(r1>9 || r2>9) return 2;
	}
	else if(format == 3){
	    int tmp = getValue(reg[PC]++);
	    int r = tmp % 16;
	    int q = tmp / 16;

	    //set e,p,b,x bit
	    if(!(q % 2)) e=0;
	    else e=1;

	    q /= 2;
	    if(!(q % 2)) p=0;
	    else p=1;
	    
	    q /= 2;
	    if(!(q % 2)) b=0;
	    else b=1;

	    q /= 2;
	    if(!(q % 2)) x=0;
	    else x=2;

	    target = getValue(reg[PC]++);
	    target += (r<<8);
	    
	    //format 4
	    if(e){
		int extVal = getValue(reg[PC]++);
		target = target << 8;
		target += extVal;
		if(b||p) return 2;
	    }

	    //indexing mode
	    if(x) target += reg[X];
	    
	    //pc relative
	    if(b==0 && p==1){
		if(target>2047) target |= 0xfffff000;
		target += reg[PC];
	    }

	    //base relative
	    else if(b==1 && p==0) target += reg[B];

	    if(target<0 || target>=MAXADDR) return 1;
	    if(mode == INDIRECT){
		if(getValue(target)<0 || getValue(target)>=MAXADDR) return 1;
	    }
	    
	}

            int bpFlag = reg[PC];
	    implement(opcode, r1, r2, mode, target);
	    if(!reg[PC]) break;
	    if(getBp(bpFlag)) return -1*bpFlag;//stop at breakpoint
    }
    if(reg[PC] >= MAXADDR) reg[PC] = 0;
    return 0;
}

void initializeProc(){
    for(int i=0; i<10; i++){
	reg[i]=0;
    }
    debugF = 0;
    return ;
}

void run_(){
    int error = runProcess();

    if(error>0) {
	printRuntimeError(error);//error
    }
    else if(error==0){
	printReg(-1);//end program
    }
    else{//breakpoint
	printReg(-1*error); return;
    }
    initializeProc();
    return ;
}
