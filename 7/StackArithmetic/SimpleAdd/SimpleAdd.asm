// SimpleAdd

// push constant 7
@7
D=A		// D = 7
@SP
A=M
M=D		// *SP = D
@SP
M=M+1		// SP++
// push constant 8
@8
D=A		// D = 8
@SP
A=M
M=D		// *SP = D
@SP
M=M+1		// SP++
// add
@SP
M=M-1		// SP--
A=M
D=M		// D = *SP
@SP
M=M-1		// SP--
A=M
M=D+M		// *SP += D
@SP
M=M+1		// SP++
