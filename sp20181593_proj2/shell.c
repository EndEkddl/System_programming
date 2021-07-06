#include "myHeader.h"
#define MAX_LEN 1001

//history를 위한 linkedlist node
node* head = NULL;
node* tail = NULL;
int cnt=0;

void help(){
	printf("\th[elp]\n");
	printf("\td[ir]\n");
	printf("\tq[uit]\n");
	printf("\thi[story]\n");
	printf("\tdu[mp] [start, end]\n");
	printf("\te[dit] address, value\n");
	printf("\tf[ill] start, end, value\n");
	printf("\treset\n");
	printf("\topcode mnemonic\n");
	printf("\topcodelist\n");
	printf("\tassemble filename\n");
	printf("\ttype filename\n");
	printf("\tsymbol\n");
	return;
}

void dir(){

	DIR* dir = opendir(".");
	struct dirent* tmp;
	struct stat state;

	for(int i=0; tmp = readdir(dir); i++){

		int len, dFlag=0, eFlag=0;
		char filename[101], c = ' ';
		//파일의 형식을 state에 저장
		strncpy(filename, tmp->d_name, 100);
		stat(tmp->d_name, &state);

		//현재 파일이 directory인지 exe파일인지 확인
		if(state.st_mode & S_IFDIR) c = '/', dFlag = 1;
		else if(state.st_mode & S_IXUSR) c = '*', eFlag = 1;

		len = strlen(filename);
		filename[len++] = c;
		filename[len] = '\0';

		if(i%4==0 && i) printf("\n");
		printf("\t%s", filename);

	} 
	printf("\n");
	closedir(dir);
	
	return;
}

void history(){
	//history가 비어있는 경우 
	if(!cnt) return;

	//history의 목록 출력
	int i=1;
	node* ptr;

	for(ptr = head; ptr != NULL; ptr = ptr->next){
	    printf("\t%d    %s\n",i++, ptr->comm);
	}
	
	return;
}

//유효한 명령어가 들어온 경우 linkedlist에 추가
void addHistory(char comm[MAX_LEN]){
	
	node* ptr = (node*)malloc(sizeof(node));
	strcpy(ptr->comm, comm);
	ptr->next = NULL;
	
	if(head == NULL) head = ptr, tail = ptr;
	else tail->next = ptr, tail = ptr;

	cnt++;
	
	return;
} 

//linkedlist에 저장된 history 삭제
void removeHistory(){

	node* ptr = head;
	while(ptr != NULL){
	    node* tmp = ptr->next;
	    free(ptr);
	    ptr = tmp;
	}
	cnt = 0;
    	return;
}

void type(char filename[MAX_LEN]){

    FILE* fp = fopen(filename, "r");
    char str[MAX_LEN];
    while(fgets(str, MAX_LEN, fp) != NULL) printf("%s", str);
    fclose(fp);
    return ;
}

