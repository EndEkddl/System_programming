#include "20181593.h"
#define MAX 1001
#define MAXADDR 0x100000

int command,a,b,c,cnt; 
char inputCommand[MAX];
char token[MAX][MAX];

enum commands{
    H,
    HELP,
    D,
    _DIR,
    Q,
    QUIT,
    HI,
    HISTORY,
    DU,
    DUMP,
    E,
    EDIT,
    F,
    FILL,
    RESET,
    OPCODE,
    OPCODE_LIST,
    _ASSEMBLE,
    _TYPE,
    _SYMBOL,
    PROGADDR,
    LOADER,
    RUN,
    BP
};

const char* commandList[24]={
    "h", "help", "d", "dir", "q", "quit", "hi", "history", "du", "dump",
    "e","edit", "f", "fill", "reset", "opcode", "opcodelist", "assemble", "type", "symbol", "progaddr", "loader", "run", "bp" 
};

int run(){
    switch(command){
	case H:
	case HELP : help();
		    break;
	case D:
	case _DIR: dir();
		   break;
	case Q:
	case QUIT : return 1;
		    break;
	case HI:
	case HISTORY : history();
		       break;
	case DU:
	case DUMP: dump(a,b);
		   break;
	case E:
	case EDIT: edit(a,b);
		   break;
	case F:
	case FILL: fill(a,b,c);
		   break;
	case RESET: reset();
		    break;
	case OPCODE: printOp(a);
		     break;
	case OPCODE_LIST: opList();
			  break;
	case _ASSEMBLE: assemble(token[1]); 
		       break;
	case _TYPE: type(token[1]); break;
	case _SYMBOL : symbol(); break;	  
	case PROGADDR : progaddr(a); break;
	case LOADER : {
			loader(cnt, token); break;
		      }
	case RUN : run_(); break;
	case BP : bp(a);		   
	default: break;

    
    }

    return 0;
}

int scissor(char* str){
    int i,j, len = strlen(str);
    int x=0, y=0, z=0, flag=0;
    int start[5]={0}, end[5] = {0};
    for(i=0; i<len; i++){
	    //명령한줄 받음
	if(str[i] != ' ' && str[i] != '\n' && str[i] != ',' && str[i] != '\t'){
	    if(x>=4) return -2;
	    if(!flag) end[z++] = i;
	    token[x][y++] = str[i];
	    flag = 1;
	}// 명령 끊어서 저장
	else if(flag){
	    start[z] = i;
	    token[x++][y] = '\0';
	    y=0;
	    flag = 0;
	}
    }

    if(start[z] != len-1){
	for(i=start[z]; i<len; i++){
	    if(str[i] != ' ' && str[i] != '\t' && str[i] != '\n') return -2;
	}
    }

    for(i=0; i<z; i++){
	int comma = 0;
	for(j=start[i]; j<end[i]; j++){
	    if(str[j] == ',') comma++;
	}
	if(i==0 && comma != 0) return -1;
	if(i==1 && comma != 0) return -2;
	if((i!=0 && i!=1) && comma != 1) return -2;
    }
    return x;
}

int toHexa(char c){
    if('A'<=c && c<='F') return (int)(c-'A'+10);
    if('a'<=c && c<='f') return (int)(c-'a'+10);
    if('0'<=c && c<='9') return (int)(c-'0');
    else return -1;
}

int toInt(char* str, int bound){
    int ret = 0;
    int i, len= strlen(str), val;
    //입력범위를 벗어난 것이 있는지 확인
    for(i=0; i<len; i++){
	val = toHexa(str[i]);
	if(val == -1) return -1;
    }
    //문자열을 숫자로 바꿈
    for(i=0; i<len; i++){
	val = toHexa(str[i]);
	ret *= 16;
	ret += val;
	if(ret >= bound) return -2;
    }
    return ret;
}

int isReside(char filename[MAX]){
    int res=0;
    DIR* dir = opendir(".");
    struct dirent* ptr;
    struct stat state;

    for(int i=0; ptr = readdir(dir); i++){
	if(!strcmp(ptr->d_name, filename)) res++;
    }
    closedir(dir);
    if(!res) return 0;
    else{
	stat(filename, &state);
	if(state.st_mode & S_IFDIR) return 0;
	else return 1;
    }
}
int makeCommand(char* str){

    cnt = scissor(str);
    //입력받은 명령어와 일치하는 명령어 찾기
    for(command=0; command<24; command++){
	if(!strcmp(token[0], commandList[command])) break;
    }
    //명령 형식에 어긋나는 것 오류처리
    if(!cnt) return 1565;
    else if(command >= 24 || !strlen(token[0]) || cnt == -1) return 1;
    else if(command == LOADER){
	cnt = comment_tokenize(str, token);
	if(cnt == 1 || cnt > 4) return 2;
	for(int i=1; i<cnt; i++){
	    if(!(isReside(token[i])) || strcmp(token[i]+strlen(token[i])-4, ".obj")){
		    
		return 6;
	    } 
	}
    }
    else if(cnt == -2) return 2;
    else if(command == DU || command == DUMP){
	if(cnt > 3) return 2;
	else if(cnt == 2){
	    a = toInt(token[1], MAX);
	    if(a == -1) return 2;
	    else if(a == -2) return 3;
	}
	else if(cnt == 3){
	    a = toInt(token[1], MAXADDR);
	    b = toInt(token[2], MAXADDR);

	    if(a == -1 || b == -1) return 2;
	    else if(a == -2 || b == -2) return 3;
	    else if(a > b) return 4;
	}
    }
    else if(command == E || command == EDIT){
	if(cnt != 3) return 2;

	    a = toInt(token[1], MAXADDR);
	    b = toInt(token[2], MAX);

	    if(a == -1 || b == -1) return 2;
	    else if(a == -2 || b == -2) return 3;
    }
    else if(command == F || command == FILL){
	if(cnt != 4) return 2;
	    a = toInt(token[1], MAXADDR);
	    b = toInt(token[2], MAXADDR);
	    c = toInt(token[3], MAX);
	    if(a == -1 || b == -1 || c == -1) return 2;
	    else if(a == -2 || b == -2 || c == -2) return 3;
	    else if(a > b) return 4;

    }
    else if(command == OPCODE){
	a = getOp(token[1]);
	if(cnt != 2) return 2;
	if(a == -1) return 5;
    }
    else if(command == _ASSEMBLE){
	if(cnt != 2) return 2;
	if(!isReside(token[1])) return 6;
	if(strcmp(token[1]+strlen(token[1])-4, ".asm")) return 6;
    }
    else if(command == _TYPE){
	if(cnt != 2) return 2;
	if(!isReside(token[1])) return 6;
    }
    else if(command == PROGADDR){
	a = toInt(token[1], MAXADDR);
	if(cnt != 2) return 2;
	if(a == -1) return 2;
	if(a == -2) return 2;
    }
    else if(command == BP){
	if(cnt > 2) return 2;
	if(cnt == 1) a = -1;
	else if(cnt == 2){
	    a = toInt(token[1], MAXADDR);
	    if(!strcmp(token[1], "clear")) a = -2;
	    else if(a == -1) return 2;
	    else if(a == -2) return 3;
	}
    }
    else if(cnt>1) return 2;

    return 0;
    
}

void errorPrint(int error){
    switch(error){
	case 1: printf("Error : Invalid command.\n");break;
	case 2: printf("Error : Invalid Argument.\n");break;
	case 3: printf("Error : Out of Range.\n");break;
	case 4: printf("Error : Invalid Address Range.\n");break;
	case 5: printf("Error : Invalid Mnemonic.\n"); break;
	case 6: printf("Error : Invalid Filename.\n"); break;
	default: break;
    }
}
int main(){

    int exit =0;
    //opcode table 생성
    initialize();
    
    while(!exit){
	int error, len;

	command = -1;
	a=-1, b=-1, c=-1, cnt=0;
	memset(inputCommand,0,sizeof(inputCommand));	
	memset(token,0,sizeof(token));

	printf("sicsim> ");
	fgets(inputCommand, sizeof(inputCommand), stdin);

	//flush buffer
	len = strlen(inputCommand);
	if(inputCommand[len-1] != '\n'){

	    char tmp = 0;
	    inputCommand[len-1] = '\n';
	    while(tmp != '\n') tmp = getchar();

	}
	error = makeCommand(inputCommand);
	//에러가 아니면 history에 추가
	if(!error){
	    exit = run();
	    addHistory(inputCommand);
	}
	else errorPrint(error);
    }
    //생성한 opcode table, history list를 삭제
    clear_sym_table();
    removeTable();
    removeHistory();
    return 0;
}
