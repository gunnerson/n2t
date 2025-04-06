// StaticTest

// push constant 111
@111
D=A		// D = 111
@SP
A=M
M=D		// *SP = D
@SP
M=M+1		// SP++
// push constant 333
@333
D=A		// D = 333
@SP
A=M
M=D		// *SP = D
@SP
M=M+1		// SP++
// push constant 888
@888
D=A		// D = 888
@SP
A=M
M=D		// *SP = D
@SP
M=M+1		// SP++
// pop static 8
@SP
M=M-1		// SP--
A=M
D=M		// D = *SP
@StaticTest.8
M=D		// *StaticTest.8 = D
// pop static 3
@SP
M=M-1		// SP--
A=M
D=M		// D = *SP
@StaticTest.3
M=D		// *StaticTest.3 = D
// pop static 1
@SP
M=M-1		// SP--
A=M
D=M		// D = *SP
@StaticTest.1
M=D		// *StaticTest.1 = D
// push static 3
@StaticTest.3
D=M		// D = *StaticTest.3
@SP
A=M
M=D		// *SP = D
@SP
M=M+1		// SP++
// push static 1
@StaticTest.1
D=M		// D = *StaticTest.1
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
// push static 8
@StaticTest.8
D=M		// D = *StaticTest.8
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
