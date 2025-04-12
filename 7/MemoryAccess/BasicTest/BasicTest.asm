// BasicTest
// [0] push constant 10
	@10
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
// [18] push constant 21
	@21
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [24] push constant 22
	@22
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [30] pop argument 2
	@2
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
// [42] pop argument 1
	@1
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
// [54] push constant 36
	@36
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [60] pop this 6
	@6
	D=A
	@THIS
	D=D+M
	@R13
	M=D
	@SP
	AM=M-1
	D=M
	@R13
	A=M
	M=D
// [72] push constant 42
	@42
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [78] push constant 45
	@45
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [84] pop that 5
	@5
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
// [96] pop that 2
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
// [108] push constant 510
	@510
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [114] pop temp 6
	@6
	D=A
	@5
	D=D+A
	@R13
	M=D
	@SP
	AM=M-1
	D=M
	@R13
	A=M
	M=D
// [126] push local 0
	@0
	D=A
	@LCL
	A=D+M
	D=M
	@SP
	M=M+1
	A=M-1
	M=D
// [135] push that 5
	@5
	D=A
	@THAT
	A=D+M
	D=M
	@SP
	M=M+1
	A=M-1
	M=D
// [144] add  
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
// [154] push argument 1
	@1
	D=A
	@ARG
	A=D+M
	D=M
	@SP
	M=M+1
	A=M-1
	M=D
// [163] sub  
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
// [173] push this 6
	@6
	D=A
	@THIS
	A=D+M
	D=M
	@SP
	M=M+1
	A=M-1
	M=D
// [182] push this 6
	@6
	D=A
	@THIS
	A=D+M
	D=M
	@SP
	M=M+1
	A=M-1
	M=D
// [191] add  
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
// [201] sub  
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
// [211] push temp 6
	@6
	D=A
	@5
	A=D+A
	D=M
	@SP
	M=M+1
	A=M-1
	M=D
// [220] add  
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

