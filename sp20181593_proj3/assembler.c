#include "assembler.h"
#define MAX_NUM 1001
#define MAX_MODIFIER 10001
#define MAX_ADDR 0x100000
#define MAX_WORD 0x1000000
#define MAX_SYMBOL 100001
#define TABLE_SIZE 100
#define DIRECTIVE 1
#define OPERATOR 2
#define COMMENT 3

symNode *symTable[TABLE_SIZE];
symNode symArr[MAX_SYMBOL];
int symOn = 0;

char program[7];
int first, last, start;
char asmToken[MAX_LEN][MAX_LEN];
int asmStart, asmEnd;

char *directiveList[7] = {
    "START",
    "END",
    "BYTE",
    "WORD",
    "RESB",
    "RESW",
    "BASE",
};

char *regList[9] = {
    "A", "X", "L", "PC", "SW", "B", "S", "T", "F"
};

//empty line 인지 확인
int is_empty_line(char *str){
    int len = strlen(str);

    for(int i=0; i<len; i++){
	if(str[i] != ' ' && str[i] != '\t' && str[i] != '\n') return 0;
    }

    return 1;
}

//comment인지 확인
int is_comment(char *str){
    if(str[0] == '.') return 1;
    else return 0;
}

//operator인지 확인
int is_operator(char *str){
    if(getOp(str) != -1) return 1;
    else return 0;
}

//directive인지 확인
int is_directive(char *str){
    for(int i=0; i<7; i++){
	if(!strcmp(str,directiveList[i])) return 1;
    }
    return 0;
}

//register인지 확인
int is_reg(char *str){
    for(int i=0; i<9; i++){
	if(!strcmp(str, regList[i])) return 1;
    }
    return 0;
}

//허용되는 범위의 문자와 숫자인지 확인
int is_valid_symbol(char *str){
    int len = strlen(str);
    
    for(int i; i<len; i++){
	int valid = 0;
	if('0' <= str[i] && str[i] <= '9') valid = 1;
	if('A' <= str[i] && str[i] <= 'Z') valid = 1;
	if('a' <= str[i] && str[i] <= 'z') valid = 1;
	if(!valid) return 0;
    }
    
    if('0' <= str[0] && str[0] <= '9') return 0;
    else return 1;
    
}

//symbol이 중복으로 저장되는지 확인
int is_in_symbol_table(char *str){
    if(getAddr(str) != -1) return 1;
    else return 0;
}

int is_valid_const(char *str){
    int len = strlen(str);
    for(int i=0; i<len; i++){
	if('0' <= str[i] && str[i] <= '9') continue;
	else return 0;
    }
    return 1;
}

//string을 10진수로 변환
int strToDec(char *str, int bound){
    int len = strlen(str);
    int res = 0;

    if(!is_valid_const(str)) return -1;
    for(int i=0; i<len; i++){
	res *= 10;
	res += str[i] - '0';
	if(res >= bound) return -1;
    }
    return res;
}

//character를 16진수로 변환
int charToHex(char c){
    int res = -1;

    if('A' <= c && c <= 'F') res = c - 'A' + 10;
    if('0' <= c && c <= '9') res = c- '0';

    return res;
}

//string을 16진수로 변환
int strToHex(char *str, int bound){
    int res=0;
    int len = strlen(str);
    int value;

    for(int i=0; i<len; i++){
	value = charToHex(str[i]);
	if(value == -1) return -1;
	res *= 16;
	res += value;
	if(res >= bound) return -1;
    }
    return res;
}

//asm 코드를 단어별로 자르고 공백의 위치를 저장하는 함수
int asm_tokenize(char *str, char token[MAX_LEN][MAX_LEN], int *s, int *e){
    int i,j,len = strlen(str);
    int x =0, y = 0, z = 0, flag = 0;

    for(i=0; i<len; i++){
	if(str[i] != ' ' && str[i] != ',' && str[i] != '\t' && str[i] != '\n'){

	    if(x>=4) return -1;
	    if(!flag) e[z++] = i;
	    token[x][y++] = str[i];
	    flag = 1;
	}
	else if(flag){
	    s[z] = i;
	    token[x++][y] = '\0';
	    y=0; flag=0;
	}
    }
    if(s[z] != len-1){
	for(i = s[z]; i<len; i++){
	    if(str[i] != ' ' && str[i] != '\t' && str[i] != '\n') return -1;
	}
    }
    return x;
}

//comment를 단어별로 잘라 저장하는 함수
int comment_tokenize(char *str, char token[MAX_LEN][MAX_LEN]){
    int len = strlen(str);
    int x=0, y=0, flag=0;

    for(int i=0; i<len; i++){
	if(str[i] != ' ' && str[i] != '\t' && str[i] != '\n'){
	    token[x][y++] = str[i];
	    flag = 1;
	}
	else if(flag){
	    token[x++][y] = '\0';
	    y=0; 
	    flag=0;
	}
    }
    return x;
}

void strPop(char *str){
    int len = strlen(str);
    for(int i=1; i<len; i++){
	str[i-1] = str[i];
    }
    str[len-1] = '\0';
}

//byte 상수 처리를 위한 함수
int byteCalculate(char *str){
    int res = -1;
    int len = strlen(str);

    if(len < 4) return -1;

    //hexa (X'')
    if(str[1] != 39 || str[len-1] != 39) return -1;
    if(str[0] == 'X'){
	for(int i=2; i<len-1; i++){
	    if(charToHex(str[i]) == -1) return -1;
	}
	res = (len-2)/2;
    }

    //상수 처리
    else if(str[0] == 'C') res = len-3;
    
    return res;
}

//중간 파일(sic/xe)코드를 만드는 함수
int makeSIC_code(char *asmFile, char *mediumFile, char *str, int *line){
    int error = 0;
    FILE *fp1 = fopen(asmFile, "r");
    FILE *fp2 = fopen(mediumFile, "w");
    int loc=0;

    while(fgets(str, MAX_LEN, fp1) != NULL){
	int type=0,n=0,i=0,x=0,e=0;
	int sBlank[5] = {0}; //공백의 시작을 저장
	int eBlank[5] = {0};//공백의 끝을 저장
	int tsz=0, locInc=0, idx;
	int isDir=0, isOp=0, isCo=0;
	char symbol[7], op[MAX_LEN], p1[MAX_LEN], p2[MAX_LEN];

	tsz = asm_tokenize(str,asmToken,sBlank, eBlank);

	if(!tsz) continue;
	else *line += 5;
	if(is_comment(asmToken[0])) isCo = 1;
	
	//comment를 입력받은 경우
	if(isCo){
	    type = COMMENT;
	    tsz = comment_tokenize(str, asmToken);

	    fprintf(fp2, "%d %d %d %d %d\t%05X\t", type,n,i,x,e, 0);
	    fprintf(fp2, "%s\t", asmToken[0]);
	    for(idx=1; idx<tsz; idx++){
		fprintf(fp2, "%s ", asmToken[idx]);
	    }
	    fprintf(fp2, "\n");
	}

	else{
	    int isByte=0;
	    //BYTE 상수처리
	    if(!strcmp(asmToken[0], "BYTE")){
	
	        char cpyStr[MAX_LEN];
		int len = strlen(str), idx=0;
		int check = 0;
		for(int i=eBlank[1]; i<len-1; i++){
		    cpyStr[idx++] = str[i];
		}
		cpyStr[idx] = '\0';
		len = strlen(cpyStr);
		for(int i=len-1; i>=0; i--){
		    if(cpyStr[i] == ' ')cpyStr[i] = '\0';
		    else break;
		}

		strcpy(asmToken[1], cpyStr);
		tsz = 2;
		isByte=1;//Byte flag 켬
	    }
	    else if(!strcmp(asmToken[1], "BYTE")){
		char cpyStr[MAX_LEN];
		int len = strlen(str), idx=0;
		int check =0;
		for(int i=eBlank[2]; i<len-1; i++){
		    cpyStr[idx++] = str[i];
		}
		cpyStr[idx] = '\0';
		len = strlen(cpyStr);
		for(int i=len-1; i>=0; i--){
		    if(cpyStr[i] == ' ')cpyStr[i] = '\0';
		    else break;
		}
		strcpy(asmToken[2], cpyStr);
		tsz = 3;
		isByte = 1;
	    }
	    else if(tsz == -1) return error=1;

	    if(!isByte){
		for(idx=0; idx<tsz; idx++){
		    if(strlen(asmToken[idx]) > 7) return error=1;
		}
	    }
	    //Byte 상수인 경우
	    else{
		for(idx = 0; idx<tsz-1; idx++){
		    if(strlen(asmToken[idx]) > 7) return error=1;
		}
		if(strlen(asmToken[tsz-1]) > 33) return error=1;
	    }

	    //상황에 맞는 플래그를 켬
	    if(is_directive(asmToken[0])) isDir=1;
	    else if(is_operator(asmToken[0])) isOp = 1;
	    else if(asmToken[0][0] == '+' && is_operator(asmToken[0]+1)) isOp = 1;

	    if(!isDir && !isOp){
		int comma, jdx;
		if(!isByte){
		    for(jdx=0; jdx<tsz-1; jdx++){
			comma=0;
			for(idx=sBlank[jdx]; idx<eBlank[jdx]; idx++){
			    if(str[idx] == ',') comma++;
			}
			if(comma) return error =1;
		    }
		    comma=0;
		    for(idx=sBlank[tsz-1]; idx<eBlank[tsz-1]; idx++){
			if(str[idx] == ',') comma++;
		    }
		    if(tsz == 4 && comma != 1) return error=1;
		    else if(tsz !=4 && comma) return error=1;
		}

		else{
		    for(jdx=0; jdx<tsz; jdx++){
			comma=0;
			for(idx=sBlank[jdx]; idx<eBlank[jdx]; idx++){
			    if(str[idx]==',') comma++;
			    if(comma) return error=1;
			}
		    }
		}
	    
		if(!is_valid_symbol(asmToken[0])) return error=1;
		else strcpy(symbol, asmToken[0]);

		if(is_in_symbol_table(symbol)) return error=2;
		else if(!strcmp(asmToken[1], "START")){
		    if(strlen(symbol)>6) return error=7;
		    strcpy(program, symbol);
		}
		else addSymbol(loc, symbol);

		tsz--;
		//한 단어 처리 후 다음 단어를 끌어옴
		for(idx=0; idx<tsz; idx++) strcpy(asmToken[idx], asmToken[idx+1]);
	    }

	    else{
		int comma, jdx;

		for(jdx=0; jdx<tsz-1; jdx++){
		    comma=0;
		    for(idx=sBlank[jdx]; idx<eBlank[jdx]; idx++){
			if(str[idx] == ',') comma++;
		    }
		    if(comma) return error=1;
		}
		comma = 0;

		for(idx = sBlank[tsz-1]; idx<eBlank[tsz-1]; idx++){
		    if(str[idx] == ',') comma++;
		}
		if(tsz==3 && comma != 1) return error=1;
		else if(tsz !=3 && comma) return error=1;
		strcpy(symbol, ".");

	    }
	
	    if(is_directive(asmToken[0])) isDir=1;
	    if(is_operator(asmToken[0])) isOp=1;
	    if(asmToken[0][0] == '+' && is_operator(asmToken[0]+1)) isOp=1;

	    //Directive flag on
	    if(isDir){
		if(!strcmp(asmToken[0], "END")){
		    if(tsz == 1) strcmp(asmToken[1], ".");
		    else if(tsz==2){
			if(strToHex(asmToken[1], MAX_ADDR) == -1 && !is_valid_symbol(asmToken[1])) return error=1;
		    }
		    else return error=1;
		    
		    if(asmEnd) return error=1;

		    strcpy(op, "END");
		    strcpy(p1, asmToken[1]);
		    last = loc;
		    loc = 0;
		    asmEnd = 1;
		}
		else{
		    
		    if(tsz != 2) return error=1;

		    if(!strcmp(asmToken[0], "START")){
			int addr = strToHex(asmToken[1], MAX_ADDR);
			if(asmStart) return error=1;
			if(addr<=-1) return error=1;

			strcpy(op, "START");
			strcpy(p1, asmToken[1]);
			first = addr;
			loc = addr;
			locInc = 0;
		    }
		    else if(!strcmp(asmToken[0], "BASE")){
			if(strToHex(asmToken[1], MAX_ADDR) == -1 && !is_valid_symbol(asmToken[1])) return error=1;
			strcpy(op, "BASE");
			strcpy(p1, asmToken[1]);
			locInc = loc;
			loc = 0;
		    }
		    else if(!strcmp(asmToken[0], "BYTE")){
			int val = byteCalculate(asmToken[1]);
			if(val<0) return error=1;
			strcpy(op, "BYTE");
			strcpy(p1, asmToken[1]);
			locInc = val;
		    }
		    else if(!strcmp(asmToken[0], "WORD")){
			int val = strToDec(asmToken[1], MAX_WORD);
			if(val<0) return error=1;
			strcpy(op, "WORD");
			strcpy(p1, asmToken[1]);
			locInc = val;
		    }
		    else if(!strcmp(asmToken[0], "RESB")){
			int val = strToDec(asmToken[1], MAX_WORD);
			if(val<0) return error=1;
			strcpy(op, "RESB");
			strcpy(p1, asmToken[1]);
			locInc = val;
		    }
		    else if(!strcmp(asmToken[0], "RESW")){
			int val = strToDec(asmToken[1], MAX_WORD);
			if(val<0) return error=1;
			strcpy(op, "RESW");
			strcpy(p1, asmToken[1]);
			locInc = val*3;
		    }
		    strcpy(p2, ".");
		    
		}
		type = DIRECTIVE;
	    }

	    else if(isOp){
		int format;

		//format 4
		if(asmToken[0][0] == '+') {
			e=1; 
			strPop(asmToken[0]);
		}
		
		format = getFormat(asmToken[0]);
		strcpy(op, asmToken[0]);

		if(!strcmp(asmToken[0], "RSUB")){
		    if(tsz != 1 || asmToken[0][0] == '+') return error=1;
		    strcpy(p1, ".");
		    strcpy(p2, ".");
		    n=0, i=0, x=0;
		    locInc = 3;
		    
		}
		else if(format == 1){
		    if(tsz != 1 || e==1) return error=1;
		    strcpy(p1, ".");
		    strcpy(p2, ".");
		    n=0, i=0, x=0;
		    locInc = 1; 
		}
		else if(format == 2){
		    if(!(2<=tsz && tsz<=3) || e==1)return error=1;
		    if(tsz == 2){
			if(!is_reg(asmToken[1])) return error=1;
			strcpy(p1, asmToken[1]);
			strcpy(p2, "A");
		    }
		    else if(tsz == 3){
			if(!is_reg(asmToken[1]) || !is_reg(asmToken[2])) return error=1;
			strcpy(p1, asmToken[1]);
			strcpy(p2, asmToken[2]);
		    }
		    n=0, i=0, x=0;
		    locInc = 2;
		}
		else{
		    if(!(2<=tsz && tsz<=3)) return error=1;
		    if(tsz != 3) {
			x=0; 
			strcpy(p2, ".");
		    }
		    else{
			if(!strcmp(asmToken[2], "X")){
			    x=1; 
			    strcpy(p2, ".");
			}
			else return error=1;
		    }
		    //indirect addressing
		    if(asmToken[1][0] == '@'){
			if(x) return error=1;
			n=1, i=0;
			strPop(asmToken[1]);
		    }
		    //immediate addressing
		    else if(asmToken[1][0] == '#'){
			if(x) return error=1;
			n=0, i=1;
			strPop(asmToken[1]);
		    }
		    //simple addressing
		    else n=1, i=1;

		    if(!e){
			if(strToDec(asmToken[1], 0x1000)<0 && !is_valid_symbol(asmToken[1])) return error=1;
			locInc = 3;
		    }
		    else{
			if(strToDec(asmToken[1], 0x100000)<0 && !is_valid_symbol(asmToken[1])) return error=1;
			locInc=4;
		    }
		    strcpy(p1, asmToken[1]);
				
		}
		type = OPERATOR;
	    }	    
	    else return error=3;

	    if(loc+locInc >= MAX_ADDR) return error=4;

	    fprintf(fp2, "%d %d %d %d %d\t%05X\t%s\t%s\t%s\t%s\n", type, n,i,x,e,loc,symbol,op, p1,p2);
	    asmStart = 1;
	    loc += locInc;
	}	
	
    }

    fclose(fp1);
    fclose(fp2);
    return error=0;
}

void byteToHex(char *str, char* target){
    int len = strlen(str);
    if(str[0] == 'C'){
	int j=0;
	for(int i=2; i<len-1; i++){
	    j += sprintf(target+j, "%02X", (int)(str[i]));
	}
    }
    else if(str[0] == 'X'){
	int j=2;
	if((len-3)%2 == 1) target[0] = '0', j=1;
	for(int i=2; i<len-1; i++){
	    target[i-j] = str[i];
	}
	target[len-1] = '\0';
    }
}

int makeObjFile(char *mediumFile, char *asmFile, char *objFile, char *lstFile, char *str, int* line){

    int error=0;
    FILE *readMed = fopen(mediumFile, "r");
    FILE *readAsm = fopen(asmFile, "r");
    FILE *writeObj = fopen(objFile, "w");
    FILE *writeLst = fopen(lstFile, "w");

    int modifier[MAX_MODIFIER], midx=0, idx;
    char objLine[MAX_LEN] = {0,};
    int objIdx=0;
    int type,n,i,x,e,loc;
    char symbol[7], op[MAX_LEN], p1[MAX_LEN], p2[MAX_LEN];
    int base=-1;

    fprintf(writeObj, "H%-7s%06X%06X\n", program, first, last-first);

    while(fscanf(readMed, "%d %d %d %d %d %X", &type, &n, &i, &x, &e, &loc) != EOF){
	char objCode[MAX_LEN] = {0,};
	int objIdx2=0;
	int noLoc=0;

	*line += 5;

	fgets(str, MAX_LEN, readAsm);
	while(is_empty_line(str)) fgets(str, MAX_LEN, readAsm);

	if(type == COMMENT){
	    fscanf(readMed, "%[^\n]", objCode);
	    fprintf(writeLst, "%4d\t%s\n", *line, objCode);
	    continue;
	}


	fscanf(readMed, "%s %s", symbol, op);
	if(!strcmp(op, "BYTE")){
	    fscanf(readMed, "\t%[^.] %s", p1,p2);
	    p1[strlen(p1)-1] = '\0';
	}
	else fscanf(readMed, "%s %s", p1, p2);

	if(type == DIRECTIVE){
	    if(!strcmp(op, "END")){
		if(!strcmp(p1, ".")) start = 0;
		else if(is_valid_symbol(p1)){
		    if(!is_in_symbol_table(p1)) return error=5;
		    else start = getAddr(p1);
		}		
		else start = strToHex(p1, MAX_ADDR);
		noLoc = 1;	
	    }
	    
	    else if(!strcmp(op, "BASE")){
		if(is_valid_symbol(p1)){
		    if(!is_in_symbol_table(p1)) return error=5;
		    else base = getAddr(p1);
		}
		else base = strToHex(p1, MAX_ADDR);
		noLoc = 1;
	    }
	    
	    else if(!strcmp(op, "BYTE")) byteToHex(p1, objCode);
	    else if(!strcmp(op, "WORD")) sprintf(objCode, "%06X", strToDec(p1,MAX_WORD));
	    else if(!strcmp(op, "RESB") || !strcmp(op, "RESW")){
		int len = strlen(objLine);
		if(len>0){
		    char tmp[5];
		    sprintf(tmp, "%02X", (len-9)/2);
		    objLine[7] = tmp[0];
		    objLine[8] = tmp[1];
		    fprintf(writeObj, "%s\n", objLine);
		    for(int i=0; i<(int)sizeof(objLine); i++){
			objLine[i] = 0;
		    }
		    objIdx = 0;
		}
	    }	    
	}
	else if(type == OPERATOR){
	    int opcode = getOp(op);
	    int format = getFormat(op);

	    if(!strcmp(op, "RSUB")) sprintf(objCode, "4F0000");
	    else if(format==1) sprintf(objCode, "%02X", opcode);
	    else if(format==2) {
		objIdx2 += sprintf(objCode, "%02X%1X%1X", opcode, getAddr(p1), getAddr(p2));
		if(strcmp(op, "CLEAR") && strcmp(op, "TIXR")){
		    strcat(p1, ",");
		    strcat(p1,p2);
		}

	    }
	    else{
		int ta=0, xbpe=0, pc = loc+3; 
		int isSym=0;

		if(is_valid_symbol(p1)) ta = getAddr(p1), isSym=1;
		else ta = strToDec(p1, 0x100000);

		if(ta == -1) return error=5;

		if(n) opcode += 2;
		if(i) opcode += 1;
		if(x) xbpe += 8;
		if(e) xbpe += 1;

		objIdx2 += sprintf(objCode+objIdx2, "%02X", opcode);
		
		//format 3
		if(!e){
		    if(isSym){
			if(pc-2048 <= ta && ta<=pc+2047){
			    ta -= pc;
			    if(ta<0) ta &= 0x00000FFF;
			    xbpe += 2;
			}
			else if(base != -1 && base <= ta && ta <= base+4095){
			    ta -= base;
			    xbpe += 4;
			}
			else return error =6;
		    }
		    sprintf(objCode + objIdx2, "%1X%03X", xbpe, ta);
		}

		//format 4
		else{
		    if(isSym) modifier[midx++] = loc+1;
		    sprintf(objCode+objIdx2, "%1X%05X", xbpe, ta);
		}
		
		//indirect addressing
		if(n==1 && i==0){
		    char *tmp = strdup(p1);
		    strcpy(p1, "@");
		    strcpy(p1, tmp);
		}
		//immediate addressing
		if(n==0 && i==1){
		    char *tmp = strdup(p1);
		    strcpy(p1, "#");
		    strcpy(p1, tmp);
		}
		//format 4
		if(e){
		    char *tmp = strdup(op);
		    strcpy(op, "+");
		    strcpy(op, tmp);
		}
		//Array register
		if(x) strcat(p1, ",X");

	    }
	}

	//write obj file
	if(strlen(objCode)){
	    int len = strlen(objLine);
	    int recordLen = len + strlen(objCode);
	    if(recordLen > 69){
		char tmp[5];
		sprintf(tmp, "%02X", (len-9)/2);
		objLine[7] = tmp[0];
		objLine[8] = tmp[1];
		fprintf(writeObj, "%s\n", objLine);
		for(int i=0; i<(int)sizeof(objLine); i++){
		    objLine[i] = 0;
		}
		objIdx=0;
	    }
	    if(!objIdx) objIdx += sprintf(objLine+objIdx, "T%06X00", loc);
	    objIdx += sprintf(objLine+objIdx, "%s", objCode);
	}

	//write lst file
	if(!strcmp(symbol, ".")) strcpy(symbol, " ");
	if(!strcmp(p1, ".")) strcpy(p1, " ");
	if(noLoc) fprintf(writeLst, "%4d\t    %s\t%s\t%-30s%s\n",*line, symbol, op, p1, objCode);
	else fprintf(writeLst, "%4d\t%04X\t%s\t%s\t%-20s%s\n", *line, loc, symbol, op, p1, objCode);
    }

    int len = strlen(objLine);
    if(len) {
	char tmp[5];
	sprintf(tmp, "%02X", (len-9)/2);
	objLine[7] = tmp[0];
	objLine[8] = tmp[1];
	fprintf(writeObj, "%s\n", objLine);
	for(int i=0; i<(int)sizeof(objLine); i++){
	    objLine[i] = 0;
	}
	objIdx=0;
    }
    for(idx=0; idx<midx; idx++){
	fprintf(writeObj, "M%06X05\n", modifier[idx]);
    }
    fprintf(writeObj, "E%06X\n", start);

    fclose(readMed);
    fclose(readAsm);
    fclose(writeObj);
    fclose(writeLst);
    
    return error=0;

}

void printAsmError(int error, char *str, int n){
    
    switch(error){
	    case 1: printf("Error(ASM) : Invalid Syntax.\n\t      -->[line:%d] %s",n,str); break;
	    case 2: printf("Error(ASM) : Duplicate symbol.\n\t    -->[line:%d] %s",n,str); break;
	    case 3: printf("Error(ASM) : Invalid Operation Code.\n\t   -->[line:%d] %s",n,str); break;
	    case 4: printf("Error(ASM) : Out of memory range.\n\t   -->[line:%d] %s",n,str); break;
	    case 5: printf("Error(ASM) : Undefined symbol.\n\t   -->[line:%d] %s",n,str); break;
	    case 6: printf("Error(ASM) : Use Format 4.\n\t\t-->[line:%d] %s",n,str); break;
	    case 7: printf("Error(ASM) : Invalid program name.\n\t   -->[line:%d] %s",n,str); break;
	    default : break;
	    
    }
} 

void assemble(char *asmFile){
    char str[MAX_LEN];
    char lstFile[MAX_LEN], objFile[MAX_LEN];
    char mediumFile[] = "mediumFile";
    int len = strlen(asmFile);
    int error, line;

    //change file name
    strcpy(lstFile, asmFile);
    lstFile[len-3] = 'l';
    lstFile[len-2] = 's';
    lstFile[len-1] = 't';

    strcpy(objFile, asmFile);
    objFile[len-3] = 'o';
    objFile[len-2] = 'b';
    objFile[len-1] = 'j';
    
    strcpy(program, "MAIN");
    first = 0, last = 0, start = 0;
    asmStart = 0, asmEnd = 0;
    clear_sym_table();
    make_sym_table();
    line = 0;

    error = makeSIC_code(asmFile, mediumFile, str, &line);
    if(error){
	clear_sym_table();
	if(isReside(mediumFile)) remove(mediumFile);
	printAsmError(error, str, line);
	return;
    }

    line = 0;
    error = makeObjFile(mediumFile, asmFile, objFile, lstFile, str, &line);
    if(error){
	clear_sym_table();
	if(isReside(mediumFile)) remove(mediumFile);
	if(isReside(objFile)) remove(objFile);
	if(isReside(lstFile)) remove(lstFile);
	printAsmError(error, str, line);
	return ;
    }
    else{
	if(isReside(mediumFile)) remove(mediumFile);
	printf("\tSuccessfully assemble %s.\n", asmFile);
    }

}

void make_sym_table(){

    for(int i=0; i<TABLE_SIZE; i++){
	symTable[i] = NULL;
    }

    addSymbol(0x0, "A");
    addSymbol(0x1, "X");
    addSymbol(0x2, "L");
    addSymbol(0x8, "PC");
    addSymbol(0x9, "SW");
    addSymbol(0x3, "B");
    addSymbol(0x4, "S");
    addSymbol(0x5, "T");
    addSymbol(0x6, "F");
    symOn=1;

    return ; 
}

void clear_sym_table(){
    
    for(int i=0; i<TABLE_SIZE; i++){
	symNode *ptr = symTable[i];
	while(ptr != NULL){
	    symNode *tmp = ptr->next;
	    free(ptr);
	    ptr = tmp;
	}
	symTable[i] = NULL;
    }
    symOn=0;
    return ;
}

void addSymbol(int addr, char symbol[7]){
    int idx = getSymHash(symbol);

    symNode *ptr = (symNode*)malloc(sizeof(symNode));
    ptr->addr = addr;
    strcpy(ptr->symbol, symbol);
    ptr->next = symTable[idx];
    symTable[idx] = ptr;
    return ;
}

int getSymHash(char key[7]){
    long long res=0;

    for(int i=0; i<7; i++){
	if(key[i] == '\0') break;
	res += (long long)key[i];
    }

    return (int)(res % TABLE_SIZE);
}

int getAddr(char key[7]){
    int idx = getSymHash(key);

    symNode *ptr = symTable[idx];
    for(; ptr != NULL; ptr = ptr->next){
	if(!strcmp(ptr->symbol, key)) return ptr->addr;
    }
    return -1;
}

void symbol(){

    int idx=0;
    if(!symOn){
	printf("Symbol Table is empty.\n");
	return;
    }

    for(int i=0; i<TABLE_SIZE; i++){
	symNode *ptr;
	for(ptr = symTable[i]; ptr != NULL; ptr = ptr->next){
	    if(!is_reg(ptr->symbol)){
		strcpy(symArr[idx].symbol, ptr->symbol);
		symArr[idx++].addr = ptr->addr;
	    }
	}

    }

    for(int i=0; i<idx-1; i++){
	for(int j=0; j<idx-i-1; j++){
	    if(strcmp(symArr[j].symbol, symArr[j+1].symbol)>0){
		symNode tmp = symArr[j];
		symArr[j] = symArr[j+1];
		symArr[j+1] = tmp;
	    }
	}
    }

    for(int i=0; i<idx; i++){
	printf("\t%s\t%04X\n", symArr[i].symbol, symArr[i].addr);
    }
    return ;
}
