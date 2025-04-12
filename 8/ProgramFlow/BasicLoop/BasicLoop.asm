// BasicLoop
// [0] push constant 0
	@0
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [6] pop local 0
	@0
	D=A
	@LCL
	D=D+M
	@R13
	M=D
	@SP
	AM=M-1
	D=M
	@R13
	A=M
	M=D
// [18] label LOOP 
($LOOP)
// [18] push argument 0
	@0
	D=A
	@ARG
	A=D+M
	D=M
	@SP
	M=M+1
	A=M-1
	M=D
// [27] push local 0
	@0
	D=A
	@LCL
	A=D+M
	D=M
	@SP
	M=M+1
	A=M-1
	M=D
// [36] add  
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
// [46] pop local 0
	@0
	D=A
	@LCL
	D=D+M
	@R13
	M=D
	@SP
	AM=M-1
	D=M
	@R13
	A=M
	M=D
// [58] push argument 0
	@0
	D=A
	@ARG
	A=D+M
	D=M
	@SP
	M=M+1
	A=M-1
	M=D
// [67] push constant 1
	@1
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [73] sub  
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
// [83] pop argument 0
	@0
	D=A
	@ARG
	D=D+M
	@R13
	M=D
	@SP
	AM=M-1
	D=M
	@R13
	A=M
	M=D
// [95] push argument 0
	@0
	D=A
	@ARG
	A=D+M
	D=M
	@SP
	M=M+1
	A=M-1
	M=D
// [104] if-goto LOOP 
	@SP
	AM=M-1
	D=M
	@$LOOP
	D;JNE
// [109] push local 0
	@0
	D=A
	@LCL
	A=D+M
	D=M
	@SP
	M=M+1
	A=M-1
	M=D

