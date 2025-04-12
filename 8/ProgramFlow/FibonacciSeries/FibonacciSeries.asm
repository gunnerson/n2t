// FibonacciSeries
// [0] push argument 1
	@1
	D=A
	@ARG
	A=D+M
	D=M
	@SP
	M=M+1
	A=M-1
	M=D
// [9] pop pointer 1
	@1
	D=A
	@3
	D=D+A
	@R13
	M=D
	@SP
	AM=M-1
	D=M
	@R13
	A=M
	M=D
// [21] push constant 0
	@0
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [27] pop that 0
	@0
	D=A
	@THAT
	D=D+M
	@R13
	M=D
	@SP
	AM=M-1
	D=M
	@R13
	A=M
	M=D
// [39] push constant 1
	@1
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [45] pop that 1
	@1
	D=A
	@THAT
	D=D+M
	@R13
	M=D
	@SP
	AM=M-1
	D=M
	@R13
	A=M
	M=D
// [57] push argument 0
	@0
	D=A
	@ARG
	A=D+M
	D=M
	@SP
	M=M+1
	A=M-1
	M=D
// [66] push constant 2
	@2
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [72] sub  
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
// [82] pop argument 0
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
// [94] label LOOP 
($LOOP)
// [94] push argument 0
	@0
	D=A
	@ARG
	A=D+M
	D=M
	@SP
	M=M+1
	A=M-1
	M=D
// [103] if-goto COMPUTE_ELEMENT 
	@SP
	AM=M-1
	D=M
	@$COMPUTE_ELEMENT
	D;JNE
// [108] goto END 
	@$END
	0;JMP
// [110] label COMPUTE_ELEMENT 
($COMPUTE_ELEMENT)
// [110] push that 0
	@0
	D=A
	@THAT
	A=D+M
	D=M
	@SP
	M=M+1
	A=M-1
	M=D
// [119] push that 1
	@1
	D=A
	@THAT
	A=D+M
	D=M
	@SP
	M=M+1
	A=M-1
	M=D
// [128] add  
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
// [138] pop that 2
	@2
	D=A
	@THAT
	D=D+M
	@R13
	M=D
	@SP
	AM=M-1
	D=M
	@R13
	A=M
	M=D
// [150] push pointer 1
	@THAT
D=M
	@SP
	M=M+1
	A=M-1
	M=D
// [156] push constant 1
	@1
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [162] add  
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
// [172] pop pointer 1
	@1
	D=A
	@3
	D=D+A
	@R13
	M=D
	@SP
	AM=M-1
	D=M
	@R13
	A=M
	M=D
// [184] push argument 0
	@0
	D=A
	@ARG
	A=D+M
	D=M
	@SP
	M=M+1
	A=M-1
	M=D
// [193] push constant 1
	@1
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [199] sub  
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
// [209] pop argument 0
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
// [221] goto LOOP 
	@$LOOP
	0;JMP
// [223] label END 
($END)

