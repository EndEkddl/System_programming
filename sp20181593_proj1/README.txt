# SIC/XE Shell Project

	SIC/XE 머신을 구현하기 위한 전 단계로서 어셈블러, 링크, 로더들을 실행하게 될 셸과 컴파일을 
	통해서 만들어진 object 코드가 적재되고 실행될 메모리공간과 mnemonic을 opcode 값으로 변환하는
	opcode 테이블과 관련 명령어들을 구현하는 프로그램이다.

## How to compile

	$ make

## How to run

	$ ./20181593.out

## How to clean the file

	$ make clean

## hell command

 	h[elp] 
	d[ir] 
	q[uit] 
	hi[story] 
	du[mp] [start, end] 
	e[dit] address, value 
	f[ill] start, end, value 	
	reset 
	opcode mnemonic 
	opcodelist 
