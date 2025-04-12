// StackTest
// [0] push constant 17
	@17
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [6] push constant 17
	@17
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [12] eq  
	@SP
	AM=M-1
	D=M
	@SP
	AM=M-1
	D=M-D
	@$__eq_10__
	D;JEQ
	@SP
	A=M
	M=0
	@$__cont_10__
	0;JMP
($__eq_10__)
	@SP
	A=M
	M=-1
($__cont_10__)
	@SP
	M=M+1
// [30] push constant 17
	@17
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [36] push constant 16
	@16
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [42] eq  
	@SP
	AM=M-1
	D=M
	@SP
	AM=M-1
	D=M-D
	@$__eq_13__
	D;JEQ
	@SP
	A=M
	M=0
	@$__cont_13__
	0;JMP
($__eq_13__)
	@SP
	A=M
	M=-1
($__cont_13__)
	@SP
	M=M+1
// [60] push constant 16
	@16
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [66] push constant 17
	@17
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [72] eq  
	@SP
	AM=M-1
	D=M
	@SP
	AM=M-1
	D=M-D
	@$__eq_16__
	D;JEQ
	@SP
	A=M
	M=0
	@$__cont_16__
	0;JMP
($__eq_16__)
	@SP
	A=M
	M=-1
($__cont_16__)
	@SP
	M=M+1
// [90] push constant 892
	@892
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [96] push constant 891
	@891
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [102] lt  
	@SP
	AM=M-1
	D=M
	@SP
	AM=M-1
	D=M-D
	@$__lt_19__
	D;JLT
	@SP
	A=M
	M=0
	@$__cont_19__
	0;JMP
($__lt_19__)
	@SP
	A=M
	M=-1
($__cont_19__)
	@SP
	M=M+1
// [120] push constant 891
	@891
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [126] push constant 892
	@892
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [132] lt  
	@SP
	AM=M-1
	D=M
	@SP
	AM=M-1
	D=M-D
	@$__lt_22__
	D;JLT
	@SP
	A=M
	M=0
	@$__cont_22__
	0;JMP
($__lt_22__)
	@SP
	A=M
	M=-1
($__cont_22__)
	@SP
	M=M+1
// [150] push constant 891
	@891
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [156] push constant 891
	@891
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [162] lt  
	@SP
	AM=M-1
	D=M
	@SP
	AM=M-1
	D=M-D
	@$__lt_25__
	D;JLT
	@SP
	A=M
	M=0
	@$__cont_25__
	0;JMP
($__lt_25__)
	@SP
	A=M
	M=-1
($__cont_25__)
	@SP
	M=M+1
// [180] push constant 32767
	@32767
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [186] push constant 32766
	@32766
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [192] gt  
	@SP
	AM=M-1
	D=M
	@SP
	AM=M-1
	D=M-D
	@$__gt_28__
	D;JGT
	@SP
	A=M
	M=0
	@$__cont_28__
	0;JMP
($__gt_28__)
	@SP
	A=M
	M=-1
($__cont_28__)
	@SP
	M=M+1
// [210] push constant 32766
	@32766
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [216] push constant 32767
	@32767
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [222] gt  
	@SP
	AM=M-1
	D=M
	@SP
	AM=M-1
	D=M-D
	@$__gt_31__
	D;JGT
	@SP
	A=M
	M=0
	@$__cont_31__
	0;JMP
($__gt_31__)
	@SP
	A=M
	M=-1
($__cont_31__)
	@SP
	M=M+1
// [240] push constant 32766
	@32766
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [246] push constant 32766
	@32766
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [252] gt  
	@SP
	AM=M-1
	D=M
	@SP
	AM=M-1
	D=M-D
	@$__gt_34__
	D;JGT
	@SP
	A=M
	M=0
	@$__cont_34__
	0;JMP
($__gt_34__)
	@SP
	A=M
	M=-1
($__cont_34__)
	@SP
	M=M+1
// [270] push constant 57
	@57
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [276] push constant 31
	@31
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [282] push constant 53
	@53
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [288] add  
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
// [298] push constant 112
	@112
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [304] sub  
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
// [314] neg  
	@SP
	A=M-1
	M=-M
// [317] and  
	@SP
	AM=M-1
	D=M
	@SP
	A=M-1
	M=D&M
// [323] push constant 82
	@82
	D=A
	@SP
	M=M+1
	A=M-1
	M=D
// [329] or  
	@SP
	AM=M-1
	D=M
	@SP
	A=M-1
	M=D|M
// [335] not  
	@SP
	A=M-1
	M=!M

