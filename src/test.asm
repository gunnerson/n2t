// test

// push temp 0
@0
D=A		// D = 0
@5
A=D+A		// A += 5
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
A=D+A		// A += 5
D=M		// D = *(D + 5)
@SP
A=M
M=D		// *SP = D
@SP
M=M+1		// SP++
// gt
@SP
M=M-1		// SP--
A=M
D=M		// D = *SP
@SP
M=M-1		// SP--
A=M
D=M-D		// D = *SP - D
@GT_3
D;JGT		// IF (D > 0) GOTO GT_3
@SP
A=M
M=0		// *SP = false
@CONT_3
0;JMP		// GOTO CONT_3
(GT_3)
@SP
A=M
M=-1		// *SP = true
(CONT_3)
@SP
M=M+1		// SP++
// pop temp 2
@2
D=A		// D = 2
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
// push temp 1
@1
D=A		// D = 1
@5
A=D+A		// A += 5
D=M		// D = *(D + 5)
@SP
A=M
M=D		// *SP = D
@SP
M=M+1		// SP++
// push temp 0
@0
D=A		// D = 0
@5
A=D+A		// A += 5
D=M		// D = *(D + 5)
@SP
A=M
M=D		// *SP = D
@SP
M=M+1		// SP++
// gt
@SP
M=M-1		// SP--
A=M
D=M		// D = *SP
@SP
M=M-1		// SP--
A=M
D=M-D		// D = *SP - D
@GT_7
D;JGT		// IF (D > 0) GOTO GT_7
@SP
A=M
M=0		// *SP = false
@CONT_7
0;JMP		// GOTO CONT_7
(GT_7)
@SP
A=M
M=-1		// *SP = true
(CONT_7)
@SP
M=M+1		// SP++
// pop temp 3
@3
D=A		// D = 3
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
