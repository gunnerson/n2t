// BasicTest

// push constant 10
@10
D=A		// D = 10
@SP
A=M
M=D		// *SP = D
@SP
M=M+1		// SP++
// pop local 0
@0
D=A		// D = 0
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
// push constant 21
@21
D=A		// D = 21
@SP
A=M
M=D		// *SP = D
@SP
M=M+1		// SP++
// push constant 22
@22
D=A		// D = 22
@SP
A=M
M=D		// *SP = D
@SP
M=M+1		// SP++
// pop argument 2
@2
D=A		// D = 2
@ARG
D=D+M		// D += ARG
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
// pop argument 1
@1
D=A		// D = 1
@ARG
D=D+M		// D += ARG
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
// push constant 36
@36
D=A		// D = 36
@SP
A=M
M=D		// *SP = D
@SP
M=M+1		// SP++
// pop this 6
@6
D=A		// D = 6
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
// push constant 42
@42
D=A		// D = 42
@SP
A=M
M=D		// *SP = D
@SP
M=M+1		// SP++
// push constant 45
@45
D=A		// D = 45
@SP
A=M
M=D		// *SP = D
@SP
M=M+1		// SP++
// pop that 5
@5
D=A		// D = 5
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
// pop that 2
@2
D=A		// D = 2
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
// push constant 510
@510
D=A		// D = 510
@SP
A=M
M=D		// *SP = D
@SP
M=M+1		// SP++
// pop temp 6
@6
D=A		// D = 6
@5
D=D+A		// D += 5
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
// push that 5
@5
D=A		// D = 5
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
// push argument 1
@1
D=A		// D = 1
@ARG
A=D+M		// D += ARG
D=M		// D = *(ARG + D)
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
// push this 6
@6
D=A		// D = 6
@THIS
A=D+M		// D += THIS
D=M		// D = *(THIS + D)
@SP
A=M
M=D		// *SP = D
@SP
M=M+1		// SP++
// push this 6
@6
D=A		// D = 6
@THIS
A=D+M		// D += THIS
D=M		// D = *(THIS + D)
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
// push temp 6
@6
D=A		// D = 6
@5
A=D+A		// A += 5
D=M		// D = *(D + 5)
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
