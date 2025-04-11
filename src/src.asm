// src

// Bootstrap Sys.init
@256
D=A
@SP
M=D
@Sys.init
0;JMP

// test
// push temp 0
	@0
	D=A
	@5
	A=D+A
	D=M
	@SP
	A=M
	M=D
	@SP
	M=M+1
// push temp 1
	@1
	D=A
	@5
	A=D+A
	D=M
	@SP
	A=M
	M=D
	@SP
	M=M+1
// gt
	@SP
	M=M-1
	A=M
	D=M
	@SP
	M=M-1
	A=M
	D=M-D
	@GT_3
	D;JGT
	@SP
	A=M
	M=0
	@CONT_3
	0;JMP
(GT_3)
	@SP
	A=M
	M=-1
(CONT_3)
	@SP
	M=M+1
// pop temp 2
	@2
	D=A
	@5
	D=D+A
	@R13
	M=D
	@SP
	M=M-1
	@SP
	A=M
	D=M
	@R13
	A=M
	M=D
// push temp 1
	@1
	D=A
	@5
	A=D+A
	D=M
	@SP
	A=M
	M=D
	@SP
	M=M+1
// push temp 0
	@0
	D=A
	@5
	A=D+A
	D=M
	@SP
	A=M
	M=D
	@SP
	M=M+1
// gt
	@SP
	M=M-1
	A=M
	D=M
	@SP
	M=M-1
	A=M
	D=M-D
	@GT_7
	D;JGT
	@SP
	A=M
	M=0
	@CONT_7
	0;JMP
(GT_7)
	@SP
	A=M
	M=-1
(CONT_7)
	@SP
	M=M+1
// pop temp 3
	@3
	D=A
	@5
	D=D+A
	@R13
	M=D
	@SP
	M=M-1
	@SP
	A=M
	D=M
	@R13
	A=M
	M=D
