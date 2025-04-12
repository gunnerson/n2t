// PointerTest
// [0] push constant 3030
	@3030
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [6] pop pointer 0
	@0
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
// [18] push constant 3040
	@3040
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [24] pop pointer 1
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
// [36] push constant 32
	@32
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [42] pop this 2
	@2
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
// [54] push constant 46
	@46
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [60] pop that 6
	@6
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
// [72] push pointer 0
	@THIS
D=M
	@SP
	M=M+1
	A=M-1
	M=D
// [78] push pointer 1
	@THAT
D=M
	@SP
	M=M+1
	A=M-1
	M=D
// [84] add  
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
// [94] push this 2
	@2
	D=A
	@THIS
	A=D+M
	D=M
	@SP
	M=M+1
	A=M-1
	M=D
// [103] sub  
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
// [113] push that 6
	@6
	D=A
	@THAT
	A=D+M
	D=M
	@SP
	M=M+1
	A=M-1
	M=D
// [122] add  
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

