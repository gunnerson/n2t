// PointerTest

// push constant 3030
@3030
D=A		// D = 3030
@SP
A=M
M=D		// *SP = D
@SP
M=M+1		// SP++
// pop pointer 0
@SP
M=M-1		// SP--
@SP
A=M
D=M		// D = *SP
@THIS
M=D		// THIS/THAT = D
// push constant 3040
@3040
D=A		// D = 3040
@SP
A=M
M=D		// *SP = D
@SP
M=M+1		// SP++
// pop pointer 1
@SP
M=M-1		// SP--
@SP
A=M
D=M		// D = *SP
@THAT
M=D		// THIS/THAT = D
// push constant 32
@32
D=A		// D = 32
@SP
A=M
M=D		// *SP = D
@SP
M=M+1		// SP++
// pop this 2
@2
D=A		// D = 2
@THIS
D=D+M		// D += THIS
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
// push constant 46
@46
D=A		// D = 46
@SP
A=M
M=D		// *SP = D
@SP
M=M+1		// SP++
// pop that 6
@6
D=A		// D = 6
@THAT
D=D+M		// D += THAT
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
// push pointer 0
@THIS
D=M		// D = *(THIS/THAT)
@SP
A=M
M=D		// *SP = D
@SP
M=M+1		// SP++
// push pointer 1
@THAT
D=M		// D = *(THIS/THAT)
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
// push this 2
@2
D=A		// D = 2
@THIS
A=D+M		// D += THIS
D=M		// D = *(THIS + D)
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
// push that 6
@6
D=A		// D = 6
@THAT
A=D+M		// D += THAT
D=M		// D = *(THAT + D)
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
