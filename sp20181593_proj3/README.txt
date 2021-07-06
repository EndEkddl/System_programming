# SIC/XE Shell Project

	프로젝트 1, 2 에서 구현한 셀(shell)에 linking과 loading 기능을 추가하는 
	프로그램이다. 프로젝트 2 에서 구현된 assemble 명령을 통해서 생성된 
	object 파일을 link시켜 메모리에 올리는 일을 수행한다. 

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
	progaddr
	loader filename
	bp 
	bp address
	bp clear
