// StackTest

// push constant 17
@17
D=A		// D = 17
@SP
A=M
M=D		// *SP = D
@SP
M=M+1		// SP++
// push constant 17
@17
D=A		// D = 17
@SP
A=M
M=D		// *SP = D
@SP
M=M+1		// SP++
// eq
@SP
M=M-1		// SP--
A=M
D=M		// D = *SP
@SP
M=M-1		// SP--
A=M
D=M-D		// D = *SP - D
@EQ_10
D;JEQ		// IF (D = 0) GOTO EQ_10
@SP
A=M
M=0		// *SP = false
@CONT_10
0;JMP		// GOTO CONT_10
(EQ_10)
@SP
A=M
M=-1		// *SP = true
(CONT_10)
@SP
M=M+1		// SP++
// push constant 17
@17
D=A		// D = 17
@SP
A=M
M=D		// *SP = D
@SP
M=M+1		// SP++
// push constant 16
@16
D=A		// D = 16
@SP
A=M
M=D		// *SP = D
@SP
M=M+1		// SP++
// eq
@SP
M=M-1		// SP--
A=M
D=M		// D = *SP
@SP
M=M-1		// SP--
A=M
D=M-D		// D = *SP - D
@EQ_13
D;JEQ		// IF (D = 0) GOTO EQ_13
@SP
A=M
M=0		// *SP = false
@CONT_13
0;JMP		// GOTO CONT_13
(EQ_13)
@SP
A=M
M=-1		// *SP = true
(CONT_13)
@SP
M=M+1		// SP++
// push constant 16
@16
D=A		// D = 16
@SP
A=M
M=D		// *SP = D
@SP
M=M+1		// SP++
// push constant 17
@17
D=A		// D = 17
@SP
A=M
M=D		// *SP = D
@SP
M=M+1		// SP++
// eq
@SP
M=M-1		// SP--
A=M
D=M		// D = *SP
@SP
M=M-1		// SP--
A=M
D=M-D		// D = *SP - D
@EQ_16
D;JEQ		// IF (D = 0) GOTO EQ_16
@SP
A=M
M=0		// *SP = false
@CONT_16
0;JMP		// GOTO CONT_16
(EQ_16)
@SP
A=M
M=-1		// *SP = true
(CONT_16)
@SP
M=M+1		// SP++
// push constant 892
@892
D=A		// D = 892
@SP
A=M
M=D		// *SP = D
@SP
M=M+1		// SP++
// push constant 891
@891
D=A		// D = 891
@SP
A=M
M=D		// *SP = D
@SP
M=M+1		// SP++
// lt
@SP
M=M-1		// SP--
A=M
D=M		// D = *SP
@SP
M=M-1		// SP--
A=M
D=M-D		// D = *SP - D
@LT_19
D;JLT		// IF (D < 0) GOTO LT_19
@SP
A=M
M=0		// *SP = false
@CONT_19
0;JMP		// GOTO CONT_19
(LT_19)
@SP
A=M
M=-1		// *SP = true
(CONT_19)
@SP
M=M+1		// SP++
// push constant 891
@891
D=A		// D = 891
@SP
A=M
M=D		// *SP = D
@SP
M=M+1		// SP++
// push constant 892
@892
D=A		// D = 892
@SP
A=M
M=D		// *SP = D
@SP
M=M+1		// SP++
// lt
@SP
M=M-1		// SP--
A=M
D=M		// D = *SP
@SP
M=M-1		// SP--
A=M
D=M-D		// D = *SP - D
@LT_22
D;JLT		// IF (D < 0) GOTO LT_22
@SP
A=M
M=0		// *SP = false
@CONT_22
0;JMP		// GOTO CONT_22
(LT_22)
@SP
A=M
M=-1		// *SP = true
(CONT_22)
@SP
M=M+1		// SP++
// push constant 891
@891
D=A		// D = 891
@SP
A=M
M=D		// *SP = D
@SP
M=M+1		// SP++
// push constant 891
@891
D=A		// D = 891
@SP
A=M
M=D		// *SP = D
@SP
M=M+1		// SP++
// lt
@SP
M=M-1		// SP--
A=M
D=M		// D = *SP
@SP
M=M-1		// SP--
A=M
D=M-D		// D = *SP - D
@LT_25
D;JLT		// IF (D < 0) GOTO LT_25
@SP
A=M
M=0		// *SP = false
@CONT_25
0;JMP		// GOTO CONT_25
(LT_25)
@SP
A=M
M=-1		// *SP = true
(CONT_25)
@SP
M=M+1		// SP++
// push constant 32767
@32767
D=A		// D = 32767
@SP
A=M
M=D		// *SP = D
@SP
M=M+1		// SP++
// push constant 32766
@32766
D=A		// D = 32766
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
@GT_28
D;JGT		// IF (D > 0) GOTO GT_28
@SP
A=M
M=0		// *SP = false
@CONT_28
0;JMP		// GOTO CONT_28
(GT_28)
@SP
A=M
M=-1		// *SP = true
(CONT_28)
@SP
M=M+1		// SP++
// push constant 32766
@32766
D=A		// D = 32766
@SP
A=M
M=D		// *SP = D
@SP
M=M+1		// SP++
// push constant 32767
@32767
D=A		// D = 32767
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
@GT_31
D;JGT		// IF (D > 0) GOTO GT_31
@SP
A=M
M=0		// *SP = false
@CONT_31
0;JMP		// GOTO CONT_31
(GT_31)
@SP
A=M
M=-1		// *SP = true
(CONT_31)
@SP
M=M+1		// SP++
// push constant 32766
@32766
D=A		// D = 32766
@SP
A=M
M=D		// *SP = D
@SP
M=M+1		// SP++
// push constant 32766
@32766
D=A		// D = 32766
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
@GT_34
D;JGT		// IF (D > 0) GOTO GT_34
@SP
A=M
M=0		// *SP = false
@CONT_34
0;JMP		// GOTO CONT_34
(GT_34)
@SP
A=M
M=-1		// *SP = true
(CONT_34)
@SP
M=M+1		// SP++
// push constant 57
@57
D=A		// D = 57
@SP
A=M
M=D		// *SP = D
@SP
M=M+1		// SP++
// push constant 31
@31
D=A		// D = 31
@SP
A=M
M=D		// *SP = D
@SP
M=M+1		// SP++
// push constant 53
@53
D=A		// D = 53
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
// push constant 112
@112
D=A		// D = 112
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
// neg
@SP
M=M-1		// SP--
A=M
M=-M		// *SP = -(*SP)
@SP
M=M+1		// SP++
// and
@SP
M=M-1		// SP--
A=M
D=M		// D = *SP
@SP
M=M-1		// SP--
A=M
M=D&M		// *SP = D & M
@SP
M=M+1		// SP++
// push constant 82
@82
D=A		// D = 82
@SP
A=M
M=D		// *SP = D
@SP
M=M+1		// SP++
// or
@SP
M=M-1		// SP--
A=M
D=M		// D = *SP
@SP
M=M-1		// SP--
A=M
M=D|M		// *SP = D | M
@SP
M=M+1		// SP++
// not
@SP
M=M-1		// SP--
A=M
M=!M		// *SP = !M
@SP
M=M+1		// SP++
