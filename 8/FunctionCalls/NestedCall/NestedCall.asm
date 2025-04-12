// NestedCall

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
// [56] push constant 4000
	@4000
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [62] pop pointer 0
	@0
	D=A
	@3
	D=D+A
	@R13
	M=D
	@SP
	AM=M-1
	D=M
	@R13
	A=M
	M=D
// [74] push constant 5000
	@5000
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [80] pop pointer 1
	@1
	D=A
	@3
	D=D+A
	@R13
	M=D
	@SP
	AM=M-1
	D=M
	@R13
	A=M
	M=D
// [92] call Sys.main 0
	@Sys.main$__return__
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
	@Sys.main
	0;JMP		// goto FOO
(Sys.main$__return__)
// [133] pop temp 1
	@1
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
// [145] label LOOP 
(Sys.main$LOOP)
// [145] goto LOOP 
	@Sys.main$LOOP
	0;JMP
// [147] function Sys.main 5
(Sys.main)
	@5
	D=A
	@Sys.main$__endinit__
	D;JEQ
(Sys.main$__init__)
	@SP
	M=M+1
	A=M-1
	M=0
	D=D-1
	@Sys.main$__init__
	D;JGT
(Sys.main$__endinit__)
// [158] push constant 4001
	@4001
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [164] pop pointer 0
	@0
	D=A
	@3
	D=D+A
	@R13
	M=D
	@SP
	AM=M-1
	D=M
	@R13
	A=M
	M=D
// [176] push constant 5001
	@5001
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [182] pop pointer 1
	@1
	D=A
	@3
	D=D+A
	@R13
	M=D
	@SP
	AM=M-1
	D=M
	@R13
	A=M
	M=D
// [194] push constant 200
	@200
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [200] pop local 1
	@1
	D=A
	@LCL
	D=D+M
	@R13
	M=D
	@SP
	AM=M-1
	D=M
	@R13
	A=M
	M=D
// [212] push constant 40
	@40
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [218] pop local 2
	@2
	D=A
	@LCL
	D=D+M
	@R13
	M=D
	@SP
	AM=M-1
	D=M
	@R13
	A=M
	M=D
// [230] push constant 6
	@6
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [236] pop local 3
	@3
	D=A
	@LCL
	D=D+M
	@R13
	M=D
	@SP
	AM=M-1
	D=M
	@R13
	A=M
	M=D
// [248] push constant 123
	@123
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [254] call Sys.add12 1
	@Sys.add12$__return__
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
	@Sys.add12
	0;JMP		// goto FOO
(Sys.add12$__return__)
// [295] pop temp 0
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
// [307] push local 0
	@0
	D=A
	@LCL
	A=D+M
	D=M
	@SP
	M=M+1
	A=M-1
	M=D
// [316] push local 1
	@1
	D=A
	@LCL
	A=D+M
	D=M
	@SP
	M=M+1
	A=M-1
	M=D
// [325] push local 2
	@2
	D=A
	@LCL
	A=D+M
	D=M
	@SP
	M=M+1
	A=M-1
	M=D
// [334] push local 3
	@3
	D=A
	@LCL
	A=D+M
	D=M
	@SP
	M=M+1
	A=M-1
	M=D
// [343] push local 4
	@4
	D=A
	@LCL
	A=D+M
	D=M
	@SP
	M=M+1
	A=M-1
	M=D
// [352] add  
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
// [362] add  
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
// [372] add  
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
// [382] add  
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
// [392] return  
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
// [433] function Sys.add12 0
(Sys.add12)
	@0
	D=A
	@Sys.add12$__endinit__
	D;JEQ
(Sys.add12$__init__)
	@SP
	M=M+1
	A=M-1
	M=0
	D=D-1
	@Sys.add12$__init__
	D;JGT
(Sys.add12$__endinit__)
// [444] push constant 4002
	@4002
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [450] pop pointer 0
	@0
	D=A
	@3
	D=D+A
	@R13
	M=D
	@SP
	AM=M-1
	D=M
	@R13
	A=M
	M=D
// [462] push constant 5002
	@5002
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [468] pop pointer 1
	@1
	D=A
	@3
	D=D+A
	@R13
	M=D
	@SP
	AM=M-1
	D=M
	@R13
	A=M
	M=D
// [480] push argument 0
	@0
	D=A
	@ARG
	A=D+M
	D=M
	@SP
	M=M+1
	A=M-1
	M=D
// [489] push constant 12
	@12
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [495] add  
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
// [505] return  
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

