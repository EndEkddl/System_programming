# SIC/XE Shell Project 1

	SIC/XE 머신을 구현하기 위한 전 단계로서 어셈블러, 링크, 로더들을 실행하게 될 셸과 컴파일을 
	통해서 만들어진 object 코드가 적재되고 실행될 메모리공간과 mnemonic을 opcode 값으로 변환하는
	opcode 테이블과 관련 명령어들을 구현하는 프로그램이다.

&#10004; How to compile

	$ make

&#10004; How to run

	$ ./20181593.out

&#10004; How to clean the file

	$ make clean

&#10004; Shell command

- h[elp] 
- d[ir] 
- q[uit] 
- hi[story] 
- du[mp] [start, end] 
- e[dit] address, value 
- f[ill] start, end, value 	
- reset 
- opcode mnemonic 
- opcodelist 

-------------------

# SIC/XE Shell Project 2

	프로젝트1에서 구현한 셀(shell)에 assemble 기능을 추가하는 프로그램이다. 
	SIC/XE의 assembly program source 파일을 입력 받아서 object파일을 생성하고, 
	어셈블리 과정 중 생성된 symbol table과 결과물인 object 파일을 볼 수 있는 기능을 제공한다.

&#10004; How to compile

	$ make

&#10004; How to run

	$ ./20181593.out

&#10004; How to clean the file

	$ make clean

&#10004; Shell command

- h[elp] 
- d[ir] 
- q[uit] 
- hi[story] 
- du[mp] [start, end] 
- e[dit] address, value 
- f[ill] start, end, value 	
- reset 
- opcode mnemonic 
- opcodelist 
- assemble filename
- type filename
- symbol

------------------------------
# SIC/XE Shell Project 3

	프로젝트 1, 2 에서 구현한 셀(shell)에 linking과 loading 기능을 추가하는 
	프로그램이다. 프로젝트 2 에서 구현된 assemble 명령을 통해서 생성된 
	object 파일을 link시켜 메모리에 올리는 일을 수행한다. 

&#10004; How to compile

	$ make

&#10004; How to run

	$ ./20181593.out

&#10004; How to clean the file

	$ make clean

&#10004; Shell command

- h[elp] 
- d[ir] 
- q[uit] 
- hi[story] 
- du[mp] [start, end] 
- e[dit] address, value 
- f[ill] start, end, value 	
- reset 
- opcode mnemonic 
- opcodelist 
- assemble filename
- type filename
- symbol
- progaddr
- loader filename
- bp 
- bp address
- bp clear

---------------------------------

# SIC/XE Shell Project 4
  MNIST 예제를 바탕으로 프로젝트 4에서는 Google Colab에서 Keras를 이용하여 이미지 분류를 하는 머신러닝 모델을 개발한다.
  CIFAR-10이라는 dataset을 이용하여 이미지 분류 성능을 측정하고 직접 모델의 파라미터들을 바꿔가며 테스트 셋에 대한 
  정확도 80% 이상을 얻는 것을 목표로 한다.
	
&#10004; Reshaping the data<br>
&#10004; Creating the model
<img width="170" alt="model1" src="https://user-images.githubusercontent.com/78426705/126148793-c7fbaec4-c409-445f-8ea6-2f9b869d7d70.png">
<img width="170" alt="model2" src="https://user-images.githubusercontent.com/78426705/126148847-a91dbd2e-bc2e-4960-963b-d9ddf2b3c680.png"><br>
&#10004; Compiling and training the model(loss function)
<img width="350" alt="loss" src="https://user-images.githubusercontent.com/78426705/126148843-42147d18-dad1-431d-86ff-3602cf3815b4.png"><br>
&#10004; Confusion matrix
<img width="350" alt="ㅇㅇ" src="https://user-images.githubusercontent.com/78426705/126148787-0b7c54ef-d9dc-46b6-90f4-379858523de3.png"><br>
&#10004; Test accuracy
<img width="370" alt="ㄷㄱ" src="https://user-images.githubusercontent.com/78426705/126148791-9a2ec1f7-3d04-4e63-a06c-012278460e85.png"><br>

