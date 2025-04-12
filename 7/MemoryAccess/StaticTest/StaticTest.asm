// StaticTest
// [0] push constant 111
	@111
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [6] push constant 333
	@333
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [12] push constant 888
	@888
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [18] pop static 8
	@SP
	AM=M-1
	D=M
	@StaticTest.8
	M=D
// [23] pop static 3
	@SP
	AM=M-1
	D=M
	@StaticTest.3
	M=D
// [28] pop static 1
	@SP
	AM=M-1
	D=M
	@StaticTest.1
	M=D
// [33] push static 3
	@StaticTest.3
	D=M
	@SP
	M=M+1
	A=M-1
	M=D
// [39] push static 1
	@StaticTest.1
	D=M
	@SP
	M=M+1
	A=M-1
	M=D
// [45] sub  
	@SP
	M=M-1
	A=M
	D=M
	@SP
	M=M-1
	A=M
	M=M-D
	@SP
	M=M+1
// [55] push static 8
	@StaticTest.8
	D=M
	@SP
	M=M+1
	A=M-1
	M=D
// [61] add  
	@SP
	M=M-1
	A=M
	D=M
	@SP
	M=M-1
	A=M
	M=D+M
	@SP
	M=M+1

