// push constant 16
@16
D=A		// D = 16
@SP
A=M
M=D		// *SP = D
@SP
M=M+1		// SP++
// push temp 0
@0
D=A		// D = 0
@5
A=D+A
D=M		// D = *(D + 5)
@SP
A=M
M=D		// *SP = D
@SP
M=M+1		// SP++
// push temp 1
@1
D=A		// D = 1
@5
A=D+A
D=M		// D = *(D + 5)
@SP
A=M
M=D		// *SP = D
@SP
M=M+1		// SP++
// sub
@SP
M=M-1		// SP--
A=M
D=M		// D = *SP
@SP
M=M-1		// SP--
A=M
M=M-D		// *SP -= D
@SP
M=M+1		// SP++
