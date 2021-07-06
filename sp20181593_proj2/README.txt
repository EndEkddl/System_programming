# SIC/XE Shell Project

	프로젝트1에서 구현한 셀(shell)에 assemble 기능을 추가하는 프로그램이다. 
	SIC/XE의 assembly program source 파일을 입력 받아서 object파일을 생성하고, 
	어셈블리 과정 중 생성된 symbol table과 결과물인 object 파일을 볼 수 있는 기능을 제공한다.

## How to compile

	$ make

## How to run

	$ ./20181593.out

## How to clean the file

	$ make clean

## shell command

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
	assemble filename
	type filename
	symbol
