// push constant 7
@7
D=A		// D = 7
@SP
A=M
M=D		// *SP = D
@SP
M=M+1		// SP++
// push local 0
@0
D=A		// D = 0
@LCL
A=D+M		// D += LCL
D=M		// D = *(LCL + D)
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
// pop local 1
@1
D=A		// D = 1
@LCL
D=D+M		// D += LCL
@R13
M=D		// R13 = D
@SP
M=M-1		// SP--
@SP
A=M
D=M		// D = *SP
@R13
A=M
M=D		// *R13 = D
// push constant 42
@42
D=A		// D = 42
@SP
A=M
M=D		// *SP = D
@SP
M=M+1		// SP++
