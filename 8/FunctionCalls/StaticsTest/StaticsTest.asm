// StaticsTest

// [0] Bootstrap Sys.init
	@256
	D=A
	@SP
	M=D
	@Sys.init$__return__
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
(Sys.init$__return__)

// Class2
// [45] function Class2.set 0
(Class2.set)
	@0
	D=A
	@Class2.set$__endinit__
	D;JEQ
(Class2.set$__init__)
	@SP
	M=M+1
	A=M-1
	M=0
	D=D-1
	@Class2.set$__init__
	D;JGT
(Class2.set$__endinit__)
// [56] push argument 0
	@0
	D=A
	@ARG
	A=D+M
	D=M
	@SP
	M=M+1
	A=M-1
	M=D
// [65] pop static 0
	@SP
	AM=M-1
	D=M
	@Class2.0
	M=D
// [70] push argument 1
	@1
	D=A
	@ARG
	A=D+M
	D=M
	@SP
	M=M+1
	A=M-1
	M=D
// [79] pop static 1
	@SP
	AM=M-1
	D=M
	@Class2.1
	M=D
// [84] push constant 0
	@0
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [90] return  
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
// [131] function Class2.get 0
(Class2.get)
	@0
	D=A
	@Class2.get$__endinit__
	D;JEQ
(Class2.get$__init__)
	@SP
	M=M+1
	A=M-1
	M=0
	D=D-1
	@Class2.get$__init__
	D;JGT
(Class2.get$__endinit__)
// [142] push static 0
	@Class2.0
	D=M
	@SP
	M=M+1
	A=M-1
	M=D
// [148] push static 1
	@Class2.1
	D=M
	@SP
	M=M+1
	A=M-1
	M=D
// [154] sub  
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
// [164] return  
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

// Sys
// [205] function Sys.init 0
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
// [216] push constant 6
	@6
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [222] push constant 8
	@8
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [228] call Class1.set 2
	@Class1.set$__return__
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
	@7
	D=D-A
	@ARG
	M=D			// ARG = SP-n-5
	@SP
	D=M
	@LCL
	M=D			// LCL = SP
	@Class1.set
	0;JMP		// goto FOO
(Class1.set$__return__)
// [269] pop temp 0
	@0
	D=A
	@5
	D=D+A
	@R13
	M=D
	@SP
	AM=M-1
	D=M
	@R13
	A=M
	M=D
// [281] push constant 23
	@23
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [287] push constant 15
	@15
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [293] call Class2.set 2
	@Class2.set$__return__
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
	@7
	D=D-A
	@ARG
	M=D			// ARG = SP-n-5
	@SP
	D=M
	@LCL
	M=D			// LCL = SP
	@Class2.set
	0;JMP		// goto FOO
(Class2.set$__return__)
// [334] pop temp 0
	@0
	D=A
	@5
	D=D+A
	@R13
	M=D
	@SP
	AM=M-1
	D=M
	@R13
	A=M
	M=D
// [346] call Class1.get 0
	@Class1.get$__return__
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
	@Class1.get
	0;JMP		// goto FOO
(Class1.get$__return__)
// [387] call Class2.get 0
	@Class2.get$__return__
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
	@Class2.get
	0;JMP		// goto FOO
(Class2.get$__return__)
// [428] label END 
(Class2.get$END)
// [428] goto END 
	@Class2.get$END
	0;JMP

// Class1
// [430] function Class1.set 0
(Class1.set)
	@0
	D=A
	@Class1.set$__endinit__
	D;JEQ
(Class1.set$__init__)
	@SP
	M=M+1
	A=M-1
	M=0
	D=D-1
	@Class1.set$__init__
	D;JGT
(Class1.set$__endinit__)
// [441] push argument 0
	@0
	D=A
	@ARG
	A=D+M
	D=M
	@SP
	M=M+1
	A=M-1
	M=D
// [450] pop static 0
	@SP
	AM=M-1
	D=M
	@Class1.0
	M=D
// [455] push argument 1
	@1
	D=A
	@ARG
	A=D+M
	D=M
	@SP
	M=M+1
	A=M-1
	M=D
// [464] pop static 1
	@SP
	AM=M-1
	D=M
	@Class1.1
	M=D
// [469] push constant 0
	@0
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [475] return  
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
// [516] function Class1.get 0
(Class1.get)
	@0
	D=A
	@Class1.get$__endinit__
	D;JEQ
(Class1.get$__init__)
	@SP
	M=M+1
	A=M-1
	M=0
	D=D-1
	@Class1.get$__init__
	D;JGT
(Class1.get$__endinit__)
// [527] push static 0
	@Class1.0
	D=M
	@SP
	M=M+1
	A=M-1
	M=D
// [533] push static 1
	@Class1.1
	D=M
	@SP
	M=M+1
	A=M-1
	M=D
// [539] sub  
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
// [549] return  
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

