20181593.out: 20181593.o memory.o shell.o opcodeTable.o
	gcc -W -o 20181593.out 20181593.o memory.o shell.o opcodeTable.o
20181593.o: 20181593.c 20181593.h
	gcc -W -c 20181593.c
memory.o: memory.c memory.h
	gcc -W -c memory.c
shell.o: shell.c myHeader.h
	gcc -W -c shell.c
opcodeTable.o: opcodeTable.c opcodeTable.h
	gcc -W -c opcodeTable.c
clean:
	rm 20181593.out 20181593.o shell.o memory.o opcodeTable.o
