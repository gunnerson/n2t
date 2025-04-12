// SimpleAdd
// [0] push constant 7
	@7
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [6] push constant 8
	@8
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [12] add  
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

