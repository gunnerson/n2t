// FibonacciElement

// [0] Bootstrap Sys.init
	@256
	D=A
	@SP
	M=D
	@Sys.init$__return_0__
	D=A
	@SP
	M=M+1
	A=M-1
	M=D			// push return-address
	@LCL
	D=M
	@SP
	M=M+1
	A=M-1
	M=D			// push LCL
	@ARG
	D=M
	@SP
	M=M+1
	A=M-1
	M=D			// push ARG
	@THIS
	D=M
	@SP
	M=M+1
	A=M-1
	M=D			// push THIS
	@THAT
	D=M
	@SP
	M=M+1
	A=M-1
	M=D			// push THAT
	D=A+1
	@5
	D=D-A
	@ARG
	M=D			// ARG = SP-n-5
	@SP
	D=M
	@LCL
	M=D			// LCL = SP
	@Sys.init
	0;JMP		// goto FOO
(Sys.init$__return_0__)

// Sys
// [45] function Sys.init 0
(Sys.init)
	@0
	D=A
	@Sys.init$__endinit__
	D;JEQ
(Sys.init$__init__)
	@SP
	M=M+1
	A=M-1
	M=0
	D=D-1
	@Sys.init$__init__
	D;JGT
(Sys.init$__endinit__)
// [56] push constant 4
	@4
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [62] call Main.fibonacci 1
	@Main.fibonacci$__return_16__
	D=A
	@SP
	M=M+1
	A=M-1
	M=D			// push return-address
	@LCL
	D=M
	@SP
	M=M+1
	A=M-1
	M=D			// push LCL
	@ARG
	D=M
	@SP
	M=M+1
	A=M-1
	M=D			// push ARG
	@THIS
	D=M
	@SP
	M=M+1
	A=M-1
	M=D			// push THIS
	@THAT
	D=M
	@SP
	M=M+1
	A=M-1
	M=D			// push THAT
	D=A+1
	@6
	D=D-A
	@ARG
	M=D			// ARG = SP-n-5
	@SP
	D=M
	@LCL
	M=D			// LCL = SP
	@Main.fibonacci
	0;JMP		// goto FOO
(Main.fibonacci$__return_16__)
// [103] label END 
(Main.fibonacci$END)
// [103] goto END 
	@Main.fibonacci$END
	0;JMP

// Main
// [105] function Main.fibonacci 0
(Main.fibonacci)
	@0
	D=A
	@Main.fibonacci$__endinit__
	D;JEQ
(Main.fibonacci$__init__)
	@SP
	M=M+1
	A=M-1
	M=0
	D=D-1
	@Main.fibonacci$__init__
	D;JGT
(Main.fibonacci$__endinit__)
// [116] push argument 0
	@0
	D=A
	@ARG
	A=D+M
	D=M
	@SP
	M=M+1
	A=M-1
	M=D
// [125] push constant 2
	@2
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [131] lt  
	@SP
	AM=M-1
	D=M
	@SP
	AM=M-1
	D=M-D
	@Main.fibonacci$__lt_15__
	D;JLT
	@SP
	A=M
	M=0
	@Main.fibonacci$__cont_15__
	0;JMP
(Main.fibonacci$__lt_15__)
	@SP
	A=M
	M=-1
(Main.fibonacci$__cont_15__)
	@SP
	M=M+1
// [149] if-goto N_LT_2 
	@SP
	AM=M-1
	D=M
	@Main.fibonacci$N_LT_2
	D;JNE
// [154] goto N_GE_2 
	@Main.fibonacci$N_GE_2
	0;JMP
// [156] label N_LT_2 
(Main.fibonacci$N_LT_2)
// [156] push argument 0
	@0
	D=A
	@ARG
	A=D+M
	D=M
	@SP
	M=M+1
	A=M-1
	M=D
// [165] return  
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
// [206] label N_GE_2 
(Main.fibonacci$N_GE_2)
// [206] push argument 0
	@0
	D=A
	@ARG
	A=D+M
	D=M
	@SP
	M=M+1
	A=M-1
	M=D
// [215] push constant 2
	@2
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [221] sub  
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
// [231] call Main.fibonacci 1
	@Main.fibonacci$__return_25__
	D=A
	@SP
	M=M+1
	A=M-1
	M=D			// push return-address
	@LCL
	D=M
	@SP
	M=M+1
	A=M-1
	M=D			// push LCL
	@ARG
	D=M
	@SP
	M=M+1
	A=M-1
	M=D			// push ARG
	@THIS
	D=M
	@SP
	M=M+1
	A=M-1
	M=D			// push THIS
	@THAT
	D=M
	@SP
	M=M+1
	A=M-1
	M=D			// push THAT
	D=A+1
	@6
	D=D-A
	@ARG
	M=D			// ARG = SP-n-5
	@SP
	D=M
	@LCL
	M=D			// LCL = SP
	@Main.fibonacci
	0;JMP		// goto FOO
(Main.fibonacci$__return_25__)
// [272] push argument 0
	@0
	D=A
	@ARG
	A=D+M
	D=M
	@SP
	M=M+1
	A=M-1
	M=D
// [281] push constant 1
	@1
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [287] sub  
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
// [297] call Main.fibonacci 1
	@Main.fibonacci$__return_29__
	D=A
	@SP
	M=M+1
	A=M-1
	M=D			// push return-address
	@LCL
	D=M
	@SP
	M=M+1
	A=M-1
	M=D			// push LCL
	@ARG
	D=M
	@SP
	M=M+1
	A=M-1
	M=D			// push ARG
	@THIS
	D=M
	@SP
	M=M+1
	A=M-1
	M=D			// push THIS
	@THAT
	D=M
	@SP
	M=M+1
	A=M-1
	M=D			// push THAT
	D=A+1
	@6
	D=D-A
	@ARG
	M=D			// ARG = SP-n-5
	@SP
	D=M
	@LCL
	M=D			// LCL = SP
	@Main.fibonacci
	0;JMP		// goto FOO
(Main.fibonacci$__return_29__)
// [338] add  
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
// [348] return  
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

