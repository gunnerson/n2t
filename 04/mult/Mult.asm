// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Mult.asm

// Multiplies R0 and R1 and stores the result in R2.
// Assumes that R0 >= 0, R1 >= 0, and R0 * R1 < 32768.
// (R0, R1, R2 refer to RAM[0], RAM[1], and RAM[2], respectively.)

//  R2 = 0
//  for (i=0, i<R0, i++){
//      R2 += R1
//  }

    @i
    M=0         // i = 0
    @R2
    M=0         // R2 = 0
(LOOP)
    @i
    D=M         // D = i
    @R0
    D=D-M;      // if (i = R0)
    @END
    D;JEQ       // GOTO END
    @R1
    D=M         // D = R1
    @R2
    M=M+D       // R2 += R1
    @i
    M=M+1       // i++
    @LOOP
    0;JMP
(END)
    @END
    0;JMP
