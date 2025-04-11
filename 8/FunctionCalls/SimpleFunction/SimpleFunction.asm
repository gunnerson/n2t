
// SimpleFunction
// function SimpleFunction.test 2
(SimpleFunction.test)
	@2
	D=A
	@SimpleFunction.test$__endinit__
	D;JEQ
(SimpleFunction.test$__init__)
	@SP
	M=M+1
	A=M-1
	M=0
	D=D-1
	@SimpleFunction.test$__init__
	D;JGT
(SimpleFunction.test$__endinit__)
// 	push local 0
	@0
	D=A
	@LCL
	A=D+M
	D=M
	@SP
	M=M+1
	A=M-1
	M=D
// 	push local 1
	@1
	D=A
	@LCL
	A=D+M
	D=M
	@SP
	M=M+1
	A=M-1
	M=D
// 	add
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
// 	not
	@SP
	A=M-1
	M=!M
// 	push argument 0
	@0
	D=A
	@ARG
	A=D+M
	D=M
	@SP
	M=M+1
	A=M-1
	M=D
// 	add
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
// 	push argument 1
	@1
	D=A
	@ARG
	A=D+M
	D=M
	@SP
	M=M+1
	A=M-1
	M=D
// 	sub
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
// 	return
	@LCL
	D=M
	@R14
	M=D			// FRAME = LCL
	@5
	A=D-A
	D=M
	@R15
	M=D			// RET = *(FRAME - 5)
	@SP
	AM=M-1
	D=M
	@ARG
	A=M
	M=D			// *ARG = pop()
	D=A+1
	@SP
	M=D			// SP = ARG + 1
	@R14
	AM=M-1
	D=M
	@THAT
	M=D			// THAT = *(FRAME - 1)
	@R14
	AM=M-1
	D=M
	@THIS
	M=D			// THIS = *(FRAME - 2)
	@R14
	AM=M-1
	D=M
	@ARG
	M=D			// ARG = *(FRAME - 3)
	@R14
	A=M-1
	D=M
	@LCL
	M=D			// LCL = *(FRAME - 4)
	@R15
	A=M
	0;JMP			// goto RET
